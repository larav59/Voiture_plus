/**
 * @file test_mqtt.c
 * @brief Tests unitaires pour le wrapper MQTT (mqtt.c)
 * @details Ces tests nécessitent un broker MQTT local tournant sur le port 1883.
 */

#include "tests/runner.h"
#include "core/mqtt.h"
#include "core/logger.h"


#define LOCAL_BROKER_IP "127.0.0.1"
#define LOCAL_BROKER_PORT 1883
#define CONNECTION_TIMEOUT_SEC 5
#define MESSAGE_TIMEOUT_SEC 5

static void log_callback(log_level_t level, const char *msg) {
    UNUSED(level);
    UNUSED(msg);
    //printf("[TEST-MQTT] %s\n", msg);
}

static sem_t messageSemaphore;
static char lastReceivedTopic[256];
static char lastReceivedPayload[256];


static void test_message_callback(const char* topic, const char* payload) {
    strncpy(lastReceivedTopic, topic, sizeof(lastReceivedTopic) - 1);
    strncpy(lastReceivedPayload, payload, sizeof(lastReceivedPayload) - 1);
    sem_post(&messageSemaphore);
}

TEST_REGISTER(test_mqtt_connect_disconnect, "Test MQTT connexion et déconnexion") {
    logger_init(LOG_LEVEL_DEBUG, log_callback);
    mqtt_set_message_callback(NULL);

    int connectResult = mqtt_connect(LOCAL_BROKER_IP, LOCAL_BROKER_PORT, "test-client-1", NULL, NULL);
    TEST_ASSERT(connectResult == 0, "mqtt_connect doit retourner 0 (succès)");

    int waitResult = mqtt_wait_for_connection(CONNECTION_TIMEOUT_SEC);
    TEST_ASSERT(waitResult == 0, "L'attente de connexion doit réussir");
    TEST_ASSERT(mqtt_is_connected() == true, "Le client doit être connecté");

    mqtt_disconnect();
    TEST_ASSERT(mqtt_is_connected() == false, "Le client doit être déconnecté");
    
    logger_destroy();
}


TEST_REGISTER(test_mqtt_pub_sub, "Test MQTT publication et souscription (loopback)") {
    logger_init(LOG_LEVEL_DEBUG, log_callback);
    sem_init(&messageSemaphore, 0, 0);
    mqtt_set_message_callback(test_message_callback);
    
    memset(lastReceivedTopic, 0, sizeof(lastReceivedTopic));
    memset(lastReceivedPayload, 0, sizeof(lastReceivedPayload));

    const char* testTopic = "core/test/pubsub";
    const char* testPayload = "Hello MQTT!";

    int connectResult = mqtt_connect(LOCAL_BROKER_IP, LOCAL_BROKER_PORT, "test-client-2", NULL, NULL);
    TEST_ASSERT(connectResult == 0, "mqtt_connect doit réussir");

    int waitResult = mqtt_wait_for_connection(CONNECTION_TIMEOUT_SEC);
    TEST_ASSERT(waitResult == 0, "L'attente de connexion doit réussir");
    TEST_ASSERT(mqtt_is_connected() == true, "Le client doit être connecté");

    int subResult = mqtt_subscribe(testTopic, MQTT_QOS_AT_LEAST_ONCE);
    TEST_ASSERT(subResult == 0, "La souscription doit réussir");

    // Petit délai pour que le broker enregistre l'abonnement
    usleep(500 * 1000); 
    

    int pubResult = mqtt_publish(testTopic, testPayload, MQTT_QOS_AT_LEAST_ONCE, false);
    TEST_ASSERT(pubResult == 0, "La publication doit réussir");


    struct timespec msgTimeout;
    clock_gettime(CLOCK_REALTIME, &msgTimeout);
    msgTimeout.tv_sec += MESSAGE_TIMEOUT_SEC;

    int semResult = sem_timedwait(&messageSemaphore, &msgTimeout);
    TEST_ASSERT(semResult == 0, "Doit recevoir un message (sem_timedwait doit réussir)");

    if (semResult == 0) {
        TEST_ASSERT(strcmp(lastReceivedTopic, testTopic) == 0, "Le topic reçu est incorrect");
        TEST_ASSERT(strcmp(lastReceivedPayload, testPayload) == 0, "Le payload reçu est incorrect");
    }

    mqtt_disconnect();
    sem_destroy(&messageSemaphore);
    logger_destroy();
}

/**
 * @brief Test que la configuration du LWT ne fait pas échouer la connexion.
 */
TEST_REGISTER(test_mqtt_lwt_setup, "Test de la configuration du LWT") {
    logger_init(LOG_LEVEL_DEBUG, log_callback);
    mqtt_set_message_callback(NULL);

    const char* lwtTopic = "core/test/lwt";
    const char* lwtPayload = "client-mort";

    int connectResult = mqtt_connect(
        LOCAL_BROKER_IP, 
        LOCAL_BROKER_PORT, 
        "test-client-lwt", 
        lwtTopic, 
        lwtPayload
    );
    TEST_ASSERT(connectResult == 0, "La connexion avec LWT doit réussir");
    
    int waitResult = mqtt_wait_for_connection(CONNECTION_TIMEOUT_SEC);
    TEST_ASSERT(waitResult == 0, "L'attente de connexion avec LWT doit réussir");
    TEST_ASSERT(mqtt_is_connected() == true, "Le client (avec LWT) doit être connecté");

    mqtt_disconnect();
    logger_destroy();
}