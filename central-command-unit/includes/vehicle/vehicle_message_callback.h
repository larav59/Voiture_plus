/**
 * @file route_planner_message_callback.h
 * @brief Callbacks pour les messages de route-planner
 * @details
 * Définit les callbacks pour gérer les messages de route-planner reçus via MQTT.
 * @author Lukas Grando
 */

#ifndef VEHICLE_MESSAGE_CALLBACK_H
#define VEHICLE_MESSAGE_CALLBACK_H

#include "core/mqtt.h"
#include "core/logger.h"
#include "core/graph.h"
#include "core/request_manager.h"
#include "core/action_codes.h"


#define VEHICLE_REPLY_TOPIC "services/vehicle/response"
#define VEHICLE_REQUEST_TOPIC "services/vehicle/request"

/**
 * @brief Callback pour les messages de vehicle reçus.
 * @param topic Topic MQTT du message reçu.
 * @param payload Payload du message reçu.
 */
void vehicle_message_callback(const char* topic, const char* payload);

/**
 * @brief get_vehicle_lwt_message
 * @param carId L'identifiant du véhicule
 * @param online true si le véhicule est en ligne, false sinon
 * @return Payload du LWT pour le service vehicle.
 */
void get_vehicle_lwt_message(int carId, bool online, char* buffer, size_t bufferSize);

/**
 * @brief get_vehicle_lwt_topic
 * @param carId L'identifiant du véhicule
 * @param buffer Buffer pour stocker le topic LWT
 * @param bufferSize Taille du buffer
 */
void get_vehicle_lwt_topic(int carId, char* buffer, size_t bufferSize);



#endif // VEHICLE_MESSAGE_CALLBACK_H