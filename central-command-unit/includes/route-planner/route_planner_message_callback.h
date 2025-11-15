/**
 * @file route_planner_message_callback.h
 * @brief Callbacks pour les messages de heartbeat
 * @details
 * Définit les callbacks pour gérer les messages de heartbeat reçus via MQTT.
 * @author Lukas Grando
 */

#ifndef ROUTE_PLANNER_MESSAGE_CALLBACK_H
#define ROUTE_PLANNER_MESSAGE_CALLBACK_H

#include "core/mqtt.h"
#include "core/logger.h"
#include "core/graph.h"
#include "core/request_manager.h"
#include "core/action_codes.h"
#include "route-planner/dijkstra.h"

#include "core/mqtt_messages/command_header.h"
#include "core/mqtt_messages/command_response_header.h"
#include "core/mqtt_messages/get_map_request.h"
#include "core/mqtt_messages/plan_route_request.h"
#include "core/mqtt_messages/set_railway_mode_request.h"
#include "core/mqtt_messages/set_safe_route_mode_request.h"
#include "core/mqtt_messages/set_waypoints_request.h"


#define ROUTE_PLANNER_REPLY_TOPIC "services/route-planner/response"
#define ROUTE_PLANNER_REQUEST_TOPIC "services/route-planner/request"

#define LWT_MESSAGE_OFFLINE "{\"service\":\"route_planner\",\"status\":\"offline\"}"
#define LWT_MESSAGE_ONLINE "{\"service\":\"route_planner\",\"status\":\"online\"}"

#define LWT_TOPIC "services/route-planner/status"


/**
 * @brief Callback pour les messages de route planner reçus.
 * @param topic Topic MQTT du message reçu.
 * @param payload Payload du message reçu.
 */
void route_planner_message_callback(const char* topic, const char* payload);

#endif // ROUTE_PLANNER_MESSAGE_CALLBACK_H