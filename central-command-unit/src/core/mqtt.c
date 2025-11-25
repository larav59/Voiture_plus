/**
 * @file mqtt.c
 * @brief Implémentation d'un client MQTT simple avec support du LWT.
 */

#include "core/check.h"
#include "core/logger.h"
#include "core/mqtt.h"

#include <mosquitto.h>
#include <pthread.h>

#define MQTT_FAILED_CLEANUP(mosq) \
	do { \
		if (mosq) { \
			mosquitto_destroy(mosq); \
			mosq = NULL; \
		} \
		mosquitto_lib_cleanup(); \
		sem_destroy(&connectSemaphore); \
		return -1; \
	} while(0)

/**
 * @brief Instance principale du client mosquitto
 */
static struct mosquitto *mosq = NULL;

/**
 * @brief thread gérant la boucle réseau MQTT
 */
static pthread_t mqttThread;

/**
 * @brief pointeur vers la fonction de callback pour les messages reçus
 */
static mqtt_message_callback_t mqttOnMessageCallback = NULL;

static bool isConnected = false;
static sem_t connectSemaphore; // <-- NOTRE SÉMAPHORE DE NOTIFICATION

static void on_connect_callback(struct mosquitto *mosq, void *data, int rc) {
    UNUSED(data); UNUSED(mosq);
	if (rc == MOSQ_ERR_SUCCESS) {
        LOG_INFO_SYNC("MQTT: Connected to broker successfully.");
        isConnected = true;
    } else {
        LOG_ERROR_SYNC("MQTT: Connection failed: %s", mosquitto_connack_string(rc));
        isConnected = false;
    }
	sem_post(&connectSemaphore);
}

static void on_disconnect_callback(struct mosquitto *m, void *data, int rc) {
	UNUSED(m); UNUSED(data);
    if (rc == MOSQ_ERR_SUCCESS) {
        LOG_INFO_SYNC("MQTT: Disconnected cleanly.");
    } else {
        LOG_WARNING_ASYNC("MQTT: Disconnected unexpectedly (rc: %d).", rc);
    }
    isConnected = false;
}

static void on_message_callback(struct mosquitto *m, void *data, const struct mosquitto_message *message) {
	UNUSED(m); UNUSED(data);
    if (mqttOnMessageCallback && message->payloadlen > 0) {
        char* payload_copy = (char *) malloc(message->payloadlen + 1);

        if (payload_copy) {
            memcpy(payload_copy, message->payload, message->payloadlen);
            payload_copy[message->payloadlen] = '\0';
            mqttOnMessageCallback(message->topic, payload_copy);
            free(payload_copy);
        } else {
            LOG_ERROR_ASYNC("MQTT: Failed to allocate memory for message payload copy.");
        }
    }
}

static void *mqtt_loop(void *arg) {
	UNUSED(arg);
	LOG_DEBUG_SYNC("MQTT: Network thread started.");
	mosquitto_loop_forever(mosq, -1, 1);

	LOG_DEBUG_SYNC("MQTT: Network thread exiting.");
	return NULL;
}

/**
 * @brief Définit le callback à appeler lors de la réception d'un message.
 * @details Doit être appelé avant mqtt_connect().
 * @param callback La fonction à appeler.
 */
void mqtt_set_message_callback(mqtt_message_callback_t callback) {
	mqttOnMessageCallback = callback;
}

/**
 * @brief Initialise et connecte le client MQTT, avec support du LWT.
 * @details Lance la boucle réseau dans un thread séparé.
 * @param brokerIp IP du broker.
 * @param port Port du broker.
 * @param clientId ID unique pour ce client.
 * @param lwtTopic Topic sur lequel publier le LWT si la connexion est perdue. (Peut être NULL).
 * @param lwtPayload Message du LWT. (Peut être NULL si lwtTopic est NULL).
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int mqtt_connect(const char* brokerIp, int port, const char* clientId, const char* lwtTopic, const char* lwtPayload) {
	int rc;
	if(mosq) {
		LOG_WARNING_ASYNC("MQTT: Client already initialized.");
		return 0; // double appel innoffensif
	}

	sem_init(&connectSemaphore, 0, 0);
	mosquitto_lib_init();
	mosq = mosquitto_new(clientId, true, NULL);
	if(!mosq) {
		LOG_ERROR_SYNC("MQTT: Failed to create mosquitto instance.");
		return -1;
	}

	if(lwtTopic && lwtPayload) {
		rc = mosquitto_will_set(mosq, lwtTopic, strlen(lwtPayload), lwtPayload, MQTT_QOS_EXACTLY_ONCE, true);
		if(rc != MOSQ_ERR_SUCCESS) {
			LOG_ERROR_SYNC("MQTT: Failed to set LWT: %s", mosquitto_strerror(rc));
			MQTT_FAILED_CLEANUP(mosq);
		}
		LOG_INFO_SYNC("MQTT: LWT set on topic '%s'.", lwtTopic);
	}

	mosquitto_connect_callback_set(mosq, on_connect_callback);
	mosquitto_disconnect_callback_set(mosq, on_disconnect_callback);
	mosquitto_message_callback_set(mosq, on_message_callback);

	rc = mosquitto_connect(mosq, brokerIp, port, MQTT_KEEP_ALIVE_INTERVAL_SEC);
	if(rc != MOSQ_ERR_SUCCESS) {
		LOG_ERROR_SYNC("MQTT: Failed to connect to broker: %s", mosquitto_strerror(rc));
		MQTT_FAILED_CLEANUP(mosq);
	}

	rc = pthread_create(&mqttThread, NULL, mqtt_loop, NULL);
	if(rc != 0) {
		LOG_ERROR_SYNC("MQTT: Failed to create network thread.");
		mosquitto_disconnect(mosq);
		MQTT_FAILED_CLEANUP(mosq);
	}

	LOG_INFO_SYNC("MQTT: Client initialized and connecting to %s:%d", brokerIp, port);
	return 0;
}

/**
 * @brief Attend que la tentative de connexion initiale soit terminée.
 * @details Attend la notification du thread réseau (via sémaphore)
 * que la connexion a réussi, échoué, ou que le timeout est atteint.
 * @param timeout_sec Le temps maximum d'attente en secondes.
 * @return 0 si la connexion est établie avec succès, -1 si échec ou timeout.
 */
int mqtt_wait_for_connection(int timeout_sec) {
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += timeout_sec;

	int semResult = sem_timedwait(&connectSemaphore, &ts);
	if (semResult != 0) {
		LOG_ERROR_SYNC("MQTT: Connection timed out after %d seconds.", timeout_sec);
		return -1;
	}

	LOG_INFO_SYNC("MQTT: Connection established successfully.");
	return 0;
}

/**
 * @brief Déconnecte proprement le client MQTT et arrête le thread réseau.
 */
void mqtt_disconnect(void) {
	if(!mosq) {
		LOG_WARNING_ASYNC("MQTT: Client not initialized.");
		return;
	}

	mosquitto_disconnect(mosq);
	pthread_join(mqttThread, NULL);
	mosquitto_destroy(mosq);
	mosq = NULL;
	mosquitto_lib_cleanup();
	sem_destroy(&connectSemaphore);

	LOG_INFO_SYNC("MQTT: Client disconnected and cleaned up.");
}

/**
 * @brief Publie un message sur un topic.
 * @param topic Le topic.
 * @param payload Le message.
 * @param qos Le niveau de QoS à utiliser.
 * @param retain Flag de rétention du message.
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int mqtt_publish(const char* topic, const char* payload, mqtt_qos_enum_t qos, bool retain) {
	if(!mosq) {
		LOG_ERROR_ASYNC("MQTT: Client not initialized.");
		return -1;
	}

	int rc = mosquitto_publish(mosq, NULL, topic, strlen(payload), payload, (int)qos, retain);
	if(rc != MOSQ_ERR_SUCCESS) {
		LOG_ERROR_ASYNC("MQTT: Failed to publish message: %s", mosquitto_strerror(rc));
		return -1;
	}
	return 0;
}

/**
 * @brief S'abonne à un topic.
 * @param topic Le topic.
 * @param qos Le niveau de QoS à utiliser.
 * 
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int mqtt_subscribe(const char* topic, mqtt_qos_enum_t qos) {
	if(!mosq) {
		LOG_ERROR_ASYNC("MQTT: Client not initialized.");
		return -1;
	}

	int rc = mosquitto_subscribe(mosq, NULL, topic, (int)qos);
	if(rc != MOSQ_ERR_SUCCESS) {
		LOG_ERROR_ASYNC("MQTT: Failed to subscribe to topic '%s': %s", topic, mosquitto_strerror(rc));
		return -1;
	}

	LOG_INFO_ASYNC("MQTT: Subscribed to topic '%s'.", topic);
	return 0;
}

/**
 * @brief Vérifie si le client est actuellement connecté.
 * @return true si connecté, false sinon.
 */
bool mqtt_is_connected(void) {
	return isConnected;
}