/***
 * @file mqtt.h
 * @brief Wrapper pour les opérations MQTT
 * @details
 * Ce fichier fournit des fonctions pour initialiser, publier et s'abonner à un broker MQTT.
 * Il permet également de gérer le système de LWT (Last Will and Testament).
 * @author Lukas Grando
 * @date 2025-10-20
 */

#ifndef CCU_MQTT_H
#define CCU_MQTT_H

#include "core/common.h"
#include "core/check.h"

#define MQTT_KEEP_ALIVE_INTERVAL_SEC 60 //!< Intervalle de keep-alive en secondes
#define MQTT_DEFAULT_TIMEOUT_SEC 5        //!< Timeout par défaut pour les opérations MQTT


/**
 * @brief Niveaux de QoS MQTT
 * @details 
 * Définit les niveaux de qualité de service pour les messages MQTT :
 * - **MQTT_QOS_AT_MOST_ONCE (0)** : Le message est livré au plus une fois, sans accusé de réception. (peut être perdu)
 * - **MQTT_QOS_AT_LEAST_ONCE (1)** : Le message est livré au moins une fois, avec accusé de réception. (risque de doublon)
 * - **MQTT_QOS_EXACTLY_ONCE (2)** : Le message est livré exactement une fois (très fiable mais plus lent).
 */
typedef enum{
	MQTT_QOS_AT_MOST_ONCE = 0,
	MQTT_QOS_AT_LEAST_ONCE = 1,
	MQTT_QOS_EXACTLY_ONCE = 2
} mqtt_qos_enum_t;

/** @brief Pointeur de fonction pour le callback de réception de message. */
typedef void (*mqtt_message_callback_t)(const char* topic, const char* payload);

/**
 * @brief Définit le callback à appeler lors de la réception d'un message.
 * @details Doit être appelé avant mqtt_connect().
 * @param callback La fonction à appeler.
 */
void mqtt_set_message_callback(mqtt_message_callback_t callback);

/**
 * @brief Initialise et connecte le client MQTT, avec support du LWT.
 * @details Lance la boucle réseau dans un thread séparé.
 * @param broker_ip IP du broker.
 * @param port Port du broker.
 * @param client_id ID unique pour ce client.
 * @param lwt_topic Topic sur lequel publier le LWT si la connexion est perdue. (Peut être NULL).
 * @param lwt_payload Message du LWT. (Peut être NULL si lwt_topic est NULL).
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int mqtt_connect(const char* broker_ip, int port, const char* client_id, const char* lwt_topic, const char* lwt_payload);

/**
 * @brief Attend que la tentative de connexion initiale soit terminée.
 * @details Attend la notification du thread réseau (via sémaphore)
 * que la connexion a réussi, échoué, ou que le timeout est atteint.
 * @param timeout_sec Le temps maximum d'attente en secondes.
 * @return 0 si la connexion est établie avec succès, -1 si échec ou timeout.
 */
int mqtt_wait_for_connection(int timeout_sec);

/**
 * @brief Déconnecte proprement le client MQTT et arrête le thread réseau.
 */
void mqtt_disconnect(void);

/**
 * @brief Publie un message sur un topic.
 * @param topic Le topic.
 * @param payload Le message.
 * @param qos Le niveau de QoS à utiliser.
 * @param retain Flag de rétention du message.
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int mqtt_publish(const char* topic, const char* payload, mqtt_qos_enum_t qos, bool retain);

/**
 * @brief S'abonne à un topic.
 * @param topic Le topic.
 * @param qos Le niveau de QoS à utiliser.
 * 
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int mqtt_subscribe(const char* topic, mqtt_qos_enum_t qos);

/**
 * @brief Vérifie si le client est actuellement connecté.
 * @return true si connecté, false sinon.
 */
bool mqtt_is_connected(void);

#endif // CCU_MQTT_H