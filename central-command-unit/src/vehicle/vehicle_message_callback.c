/**
 * @file vehicle_message_callback.c
 * @brief Implémentation de la logique métier du vehicle.
 */
#include "vehicle/vehicle_message_callback.h"

/**
 * @brief Callback pour les messages de vehicle reçus.
 * @param topic Topic MQTT du message reçu.
 * @param payload Payload du message reçu.
 */
void vehicle_message_callback(const char* topic, const char* payload) {
    LOG_DEBUG_SYNC("Received message on topic: %s", topic);

}

/**
 * @brief get_vehicle_lwt_message
 * @param carId L'identifiant du véhicule
 * @param online true si le véhicule est en ligne, false sinon
 * @param buffer Buffer pour stocker le message LWT
 * @param bufferSize Taille du buffer
 */
void get_vehicle_lwt_message(int carId, bool online, char* buffer, size_t bufferSize) {
    if (buffer == NULL || bufferSize == 0) return;

    // Utilisation de guillemets échappés \" pour le JSON
    if(online) {
        snprintf(buffer, bufferSize, "{\"vehicle_id\":%d,\"status\":\"online\"}", carId);
    } else {
        snprintf(buffer, bufferSize, "{\"vehicle_id\":%d,\"status\":\"offline\"}", carId);
    }
}

/**
 * @brief get_vehicle_lwt_topic
 * @param carId L'identifiant du véhicule
 * @param buffer Buffer pour stocker le topic LWT
 * @param bufferSize Taille du buffer
 */
void get_vehicle_lwt_topic(int carId, char* buffer, size_t bufferSize) {
    if (buffer == NULL || bufferSize == 0) return;

    snprintf(buffer, bufferSize, "vehicles/%d/status", carId);
}