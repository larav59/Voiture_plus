/**
 * @file heartbeat_message_callback.h
 * @brief Callbacks pour les messages de heartbeat
 * @details
 * Définit les callbacks pour gérer les messages de heartbeat reçus via MQTT.
 * @author Lukas Grando
 */

#include "heartbeat/heartbeat_message_callback.h"

#define MATCH_SERVICE(topic, service) (strncmp(topic, "services/", 9) == 0 && strstr(topic, "/status") != NULL && strstr(topic, service) != NULL)

static int extract_car_id_int(const char *topic, int *carId, const char *prefix) {
    const char *start = strstr(topic, prefix);
    if (!start) return -1;
    start += strlen(prefix);

    const char *end = strchr(start, '/');
    if (!end) return -1;

    char carIdString[20];
    size_t len = end - start;
    if (len == 0 || len >= sizeof(carIdString)) return -1;

    strncpy(carIdString, start, len);
    carIdString[len] = '\0';

    *carId = atoi(carIdString);
    return 0; 
}

/**
 * @brief Callback pour les messages de heartbeat reçus.
 * @param topic Topic MQTT du message reçu.
 * @param payload Payload du message reçu.
 */
void heartbeat_message_callback(const char* topic, const char* payload) {
	UNUSED(payload);

	if(strncmp(topic, "vehicles/", 9) == 0 && strstr(topic, "/status") != NULL) {
		int carId;
		if(extract_car_id_int(topic, &carId, "vehicles/") != 0) {
			LOG_WARNING_ASYNC("Failed to extract carId from topic: %s", topic);
			return;
		}
		LOG_WARNING_ASYNC("Vehicle ID %d is down.", carId);

		cancel_vehicle_route_request_t cancelRequest = {
			.header = create_command_header(ACTION_CANCEL_VEHICLE_ROUTE),
			.carId = carId
		};
		char *jsonPayload = cancel_vehicle_route_request_serialize_json(&cancelRequest);

		if(!jsonPayload) {
			LOG_ERROR_ASYNC("Unable to serialize CANCEL_VEHICLE_ROUTE_REQUEST for vehicle ID %d", carId);
			return;
		}
		
		mqtt_publish("services/route-planner/request", jsonPayload, MQTT_QOS_AT_MOST_ONCE, false);
		free(jsonPayload);

		revoke_vehicle_access_t revokeAccess = {
			.header = create_command_header(ACTION_REVOKE_VEHICLE_ACCESS),
			.carId = carId
		};
		jsonPayload = revoke_vehicle_access_serialize_json(&revokeAccess);

		if(!jsonPayload) {
			LOG_ERROR_ASYNC("Unable to serialize REVOKE_VEHICLE_ACCESS for vehicle ID %d", carId);
			return;
		}

		mqtt_publish("services/conflict-manager/request", jsonPayload, MQTT_QOS_AT_MOST_ONCE, false);
		free(jsonPayload);

		LOG_INFO_ASYNC("Sent CANCEL_VEHICLE_ROUTE_REQUEST for vehicle ID %d", carId);
		return;
	}

	if(MATCH_SERVICE(topic, "route-planner")) {
		LOG_WARNING_ASYNC("Route Planner service is down.");
		// Réfléchir pour une future version un traitement plus avancé :
		return;

	} 

	if(MATCH_SERVICE(topic, "conflict-manager")) {
		LOG_WARNING_ASYNC("Conflict Manager service is down.");

		// Prévenir le route planner pour qu'il planifie des routes "safe" sans zone de conflit
		set_safe_route_mode_request_t safeRouteModeRequest = {
			.header = create_command_header(ACTION_SET_SAFE_ROUTE_MODE),
			.enabled = true
		};
		char *jsonPayload = set_safe_route_mode_request_serialize_json(&safeRouteModeRequest);

		if(!jsonPayload) {
			LOG_ERROR_ASYNC("Unable to serialize SET_SAFE_ROUTE_MODE");
			return;
		}

		mqtt_publish("services/route-planner/request", jsonPayload, MQTT_QOS_AT_MOST_ONCE, false);
		free(jsonPayload);

		return;
	}

	if(MATCH_SERVICE(topic, "railway-sync")) {
		LOG_DEBUG_ASYNC("Railway synchronizer service is down.");

		// Prévenir le route-planner pour qu'il désactive le mode ferroviaire si nécessaire
		set_railway_mode_request_t railwayModeRequest = {
			.header = create_command_header(ACTION_SET_RAILWAY_MODE),
			.enabled = false
		};
		char *jsonPayload = set_railway_mode_request_serialize_json(&railwayModeRequest);
		if(!jsonPayload) {
			LOG_ERROR_ASYNC("Unable to serialize SET_RAILWAY_MODE");
			return;
		}

		mqtt_publish("services/route-planner/request", jsonPayload, MQTT_QOS_AT_MOST_ONCE, false);
		free(jsonPayload);
		return;
	}
	LOG_WARNING_ASYNC("Received heartbeat message for unknown topic: %s", topic);
}
