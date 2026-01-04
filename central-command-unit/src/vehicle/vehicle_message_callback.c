/**
 * @file vehicle_message_callback.c
 * @brief Implémentation de la logique métier du vehicle.
 */
#include "vehicle/vehicle_message_callback.h"

static void on_start_route_request(void) {
	vehicle_state_t* vehicleState = vehicle_get_state();
	if (!vehicleState) {
		LOG_ERROR_ASYNC("Vehicle state is not initialized.");
		return;
	}
	if (vehicleState->routeLen == 0 || vehicleState->route == NULL) {
		LOG_WARNING_ASYNC("No route set for the vehicle. Cannot start navigation.");
		return;
	}
	vehicleState->currentWpIndex = 0;
	vehicleState->isNavigating = true;
	protocol_send_set_speed(vehicleState->uartFd, vehicleState->targetSpeedLimit);
	waypoint_t *wp = &vehicleState->route[vehicleState->currentWpIndex];
	protocol_send_set_position_command(vehicleState->uartFd, (int16_t)wp->x, (int16_t)wp->y, 0);
	LOG_INFO_ASYNC("Vehicle: Starting navigation on route with %d waypoints", vehicleState->routeLen);
}

static void on_set_waypoints_request(const set_waypoints_request_t* request) {
	vehicle_state_t* vehicleState = vehicle_get_state();
	if (!vehicleState) {
		LOG_ERROR_ASYNC("Vehicle state is not initialized.");
		return;
	}
	LOG_INFO_ASYNC("Vehicle: Received route with %d waypoints", request->waypointCount);
	if (vehicleState->route) free(vehicleState->route);
	
	vehicleState->routeLen = request->waypointCount;
	vehicleState->route = malloc(sizeof(waypoint_t) * request->waypointCount);
	if (vehicleState->route) {
		memcpy(vehicleState->route, request->waypoints, sizeof(waypoint_t) * request->waypointCount);
		vehicleState->currentWpIndex = 0;
		vehicleState->isNavigating = false;
	}
	set_waypoints_request_destroy((set_waypoints_request_t*)request);
}

/**
 * @brief Callback pour les messages de vehicle reçus.
 * @param topic Topic MQTT du message reçu.
 * @param payload Payload du message reçu.
 */
void vehicle_message_callback(const char* topic, const char* payload) {
	// pas très smart de ma part ici ça aurait été mieux de passer en paramètre du callback un void* context
	// pour avoir accès à des info comme l'id du véhicule
	// pour l'instant je peux le récupérer depuis le vehicle context global
	LOG_DEBUG_SYNC("Received message on topic: %s", topic);

	vehicle_state_t* vehicleState = vehicle_get_state();
	if (!vehicleState) {
		LOG_ERROR_ASYNC("Vehicle state is not initialized.");
		return;
	}

	// Gestion des réponses à nos requêtes
	char vehicleResponseTopic[255];
	snprintf(vehicleResponseTopic, sizeof(vehicleResponseTopic), "vehicles/%d/response", vehicleState->carId);
	if (strcmp(topic, vehicleResponseTopic) == 0) {
		request_manager_process_response(payload);
		return;
	}

	// Gestion des commandes reçues
	char vehicleRequestTopic[255];
	snprintf(vehicleRequestTopic, sizeof(vehicleRequestTopic), "vehicles/%d/request", vehicleState->carId);
	if (strcmp(topic, vehicleRequestTopic) == 0) {
		command_header_t header = {0};
		cJSON *root = cJSON_Parse(payload);
		if (!root) {
			LOG_ERROR_ASYNC("The payload is not valid JSON.");
			return;
		}
		if(command_header_deserialize(root, &header) != 0) {
			LOG_ERROR_ASYNC("Failed to deserialize command header.");
			cJSON_Delete(root);
			return;
		}
		LOG_DEBUG_ASYNC("Processing command: %s, action: %s, replyTopic: %s", header.commandId, header.action, header.replyTopic);

		if(strcmp(header.action, ACTION_SET_WAYPOINTS_REQUEST) == 0) {
			set_waypoints_request_t request = { .header = header };
			if(set_waypoints_request_data_deserialize(root, &request) != 0) {
				LOG_ERROR_ASYNC("Failed to deserialize set waypoints request.");
				cJSON_Delete(root);
				return;
			}
			else {
				on_set_waypoints_request(&request);
			}
		}
		else if(strcmp(header.action, ACTION_START_ROUTE) == 0) {
			on_start_route_request();
		}
		else {
			LOG_ERROR_ASYNC("Unknown action received: %s", header.action);
		}

		cJSON_Delete(root);
		return;
	}
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