/**
 * @file heartbeat_message_callback.h
 * @brief Callbacks pour les messages de heartbeat
 * @details
 * Définit les callbacks pour gérer les messages de heartbeat reçus via MQTT.
 * @author Lukas Grando
 */


#ifndef HEARTBEAT_MESSAGE_CALLBACK_H
#define HEARTBEAT_MESSAGE_CALLBACK_H

#include "core/mqtt.h"
#include "core/mqtt_messages/cancel_vehicle_route_request.h"
#include "core/mqtt_messages/revoke_vehicle_access_request.h"
#include "core/mqtt_messages/set_safe_route_mode_request.h"
#include "core/mqtt_messages/set_railway_mode_request.h"

/**
 * @brief Callback pour les messages de heartbeat reçus.
 * @param topic Topic MQTT du message reçu.
 * @param payload Payload du message reçu.
 */
void heartbeat_message_callback(const char* topic, const char* payload);

#endif // HEARTBEAT_MESSAGE_CALLBACK_H