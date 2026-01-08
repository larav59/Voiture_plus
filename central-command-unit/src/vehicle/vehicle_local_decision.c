#include "vehicle/vehicle_local_decision.h"

/**
 * @brief Traite les objets détectés par la caméra pour la prise de décision locale.
 * @param objects Tableau des objets détectés par la caméra.
 * @param count Nombre d'objets dans le tableau.
 */
void on_camera_objects_logic(const camera_detected_object_t* objects, int count) {
	vehicle_state_t *state = vehicle_get_state();
	
	bool carSeen = false;
	bool speedLimitSeen = false;
	int16_t newSpeedLimit = state->targetSpeedLimit;

	for (int i = 0; i < count; i++) {
		switch (objects[i].category) {
			case CAM_CAT_CARS:
				if (objects[i].confidence > DETECTION_CONFIDENCE_THRESHOLD) carSeen = true;
				break;
			case CAM_CAT_SPEED_LIMIT_30:
				if (objects[i].confidence > DETECTION_CONFIDENCE_THRESHOLD) {
					newSpeedLimit = SPEED_LIMIT_30;
					speedLimitSeen = true;
				}
				break;
			case CAM_CAT_END_SPEED_LIMIT_30:
				if (objects[i].confidence > DETECTION_CONFIDENCE_THRESHOLD) {
					newSpeedLimit = SPEED_LIMIT_DEFAULT;
					speedLimitSeen = true;
				}
				break;
			case CAM_CAT_RAILROAD_CROSSING:
				if(objects[i].confidence > DETECTION_CONFIDENCE_THRESHOLD) {
					newSpeedLimit = SPEED_LIMIT_SLOW;
					speedLimitSeen = true;
					// TODO: Train camera to detect actual trains
				}
				break;
			case CAM_CAT_PRIORITY_ROAD:
				if(objects[i].confidence > DETECTION_CONFIDENCE_THRESHOLD) {
					newSpeedLimit = SPEED_LIMIT_DEFAULT;
					speedLimitSeen = true;
				}
				break;
			case CAM_CAT_YIELD:
				if(objects[i].confidence > DETECTION_CONFIDENCE_THRESHOLD) {
					newSpeedLimit = SPEED_LIMIT_SLOW;
					speedLimitSeen = true;
				}
				break;
			case CAM_CAT_ROUNDABOUT: 
				if(objects[i].confidence > DETECTION_CONFIDENCE_THRESHOLD) {
					newSpeedLimit = SPEED_LIMIT_SLOW;
					speedLimitSeen = true;
				}
				break;
			case CAM_CAT_PARKING:
				if(objects[i].confidence > DETECTION_CONFIDENCE_THRESHOLD) {
					newSpeedLimit = SPEED_LIMIT_30;
					speedLimitSeen = true;
					// TODO: Enter in parking and implement car maneuvering
				}
				break;
			case CAM_CAT_STRAIGHT_FORWARD:
				if(objects[i].confidence > DETECTION_CONFIDENCE_THRESHOLD) {
					newSpeedLimit = SPEED_LIMIT_30;
					speedLimitSeen = true;
				}
				break;
			case CAM_CAT_BLACK_LINE:
				// TODO: Implement black ligne script 
				// Define placement of the car based on black line detection
				break;
			default: break;
		}
	}

	if (carSeen && !state->obstacleDetected) {
		LOG_WARNING_ASYNC("Vehicle: Obstacle detected, stopping vehicle.");
		protocol_send_stop(state->uartFd);
		state->obstacleDetected = true;
	} 
	else if (!carSeen && state->obstacleDetected) {
		LOG_INFO_ASYNC("Vehicle: Obstacle cleared, resuming navigation.");
		state->obstacleDetected = false;
		protocol_send_set_speed(state->uartFd, state->targetSpeedLimit);

		if (state->isNavigating && state->route) {
			waypoint_t *wp = &state->route[state->currentWpIndex];
			protocol_send_set_position_command(state->uartFd, (int16_t)wp->x, (int16_t)wp->y, 0);
		}
	}

	if (speedLimitSeen && newSpeedLimit != state->targetSpeedLimit) {
		LOG_INFO_ASYNC("Vehicle: Adjusting speed limit to %d due to camera detection.", newSpeedLimit);
		state->targetSpeedLimit = newSpeedLimit;
		
		if (!state->obstacleDetected && state->isNavigating) {
			protocol_send_set_speed(state->uartFd, state->targetSpeedLimit);
		}
	}
}

/**
 * @brief Traite les données de télémétrie du véhicule pour la prise de décision locale.
 * @param x Position X actuelle du véhicule.
 * @param y Position Y actuelle du véhicule.
 * @param angle Angle actuel du véhicule.
 * @param speed Vitesse actuelle du véhicule.
 */
void on_vehicle_telemetry_logic(int16_t x, int16_t y, int16_t angle, int16_t speed) {
	vehicle_state_t *state = vehicle_get_state();
	
    state->x = x;
    state->y = y;
    state->angle = angle;
    state->realSpeed = speed;

    if (!state->isNavigating || !state->route) goto publish_state;

    waypoint_t *target = &state->route[state->currentWpIndex];
    
    float dx = (float)(target->x - x);
    float dy = (float)(target->y - y);
    float distance = sqrtf(dx*dx + dy*dy);

    if (distance < WAYPOINT_REACHED_THRESHOLD_MM) {
        LOG_INFO_ASYNC("Vehicle: Reached waypoint %d (%d, %d)", state->currentWpIndex, x, y);
        state->currentWpIndex++;

        if (state->currentWpIndex < state->routeLen) {
            waypoint_t *next = &state->route[state->currentWpIndex];
            LOG_INFO_ASYNC("Vehicle: Moving to next waypoint %d (%d, %d) distance: %.2f", state->currentWpIndex, (int)next->x, (int)next->y, distance);
            protocol_send_set_position_command(state->uartFd, (int16_t)next->x, (int16_t)next->y, 0);
			protocol_send_set_speed(state->uartFd, 0);
			usleep(500000);
			protocol_send_set_speed(state->uartFd, state->targetSpeedLimit);
        } else {
            LOG_INFO_ASYNC("Vehicle: Destination reached");
            protocol_send_stop(state->uartFd);
            state->isNavigating = false;
        }
    }
	else {
		LOG_INFO_ASYNC("Vehicle: Current position (%d, %d), target waypoint (%d, %d), distance: %.2f", x, y, (int)target->x, (int)target->y, distance);
	}

	publish_state:

	static long lastPublishTime = 0;
	long currentTime = core_get_current_timestamp_ms();
	if (currentTime - lastPublishTime < 3000) {
		return;
	}
	lastPublishTime = currentTime;
	vehicle_state_message_t stateMessage = {
		.carId = state->carId,
		.timestamp = core_get_current_timestamp_ms(),
		.x = state->x,
		.y = state->y,
		.angle = (float)state->angle / 100.0f,
		.speed = state->realSpeed,
		.isNavigating = state->isNavigating,
		.obstacleDetected = state->obstacleDetected
	};

	char *jsonPayload = vehicle_state_message_serialize_json(&stateMessage);
	if (jsonPayload) {
		char topic[255];
		snprintf(topic, sizeof(topic), "vehicles/%d/state", state->carId);
		mqtt_publish(topic, jsonPayload, MQTT_QOS_EXACTLY_ONCE, false);
		free(jsonPayload);
	} else {
		LOG_ERROR_ASYNC("Vehicle: Failed to serialize vehicle state message to JSON.");	
	}
}