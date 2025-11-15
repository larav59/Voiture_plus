/**
 * @file route_planner.c
 * @brief Point d'entrée du service Route Planner
 * @details
 * Ce fichier contient la fonction main du service.
 * Ce service reste à l'écoute du topics MQTT du status des autres services
 * Lorsqu'un service ne répond plus, le broker publie le message (LWT)
 */

#include "route-planner/route-planner.h"

int main(int argc, char **argv) {
	config_common_t common_config;

	core_set_service_version(ROUTE_PLANNER_SERVICE_VERSION);
	signal_init();

	if(core_bootstrap(argc, argv, &common_config, NULL, NULL, LWT_MESSAGE_OFFLINE, LWT_TOPIC) != 0) {
		LOG_FATAL_SYNC("Failed to bootstrap core systems. Exiting.");
		return EXIT_FAILURE;
	}
	LOG_INFO_ASYNC("Route Planner Service started successfully.");

	mqtt_subscribe(ROUTE_PLANNER_REQUEST_TOPIC, MQTT_QOS_EXACTLY_ONCE);
	mqtt_subscribe(ROUTE_PLANNER_REPLY_TOPIC, MQTT_QOS_EXACTLY_ONCE);

	mqtt_set_message_callback(route_planner_message_callback);
	mqtt_publish(LWT_TOPIC, LWT_MESSAGE_ONLINE, MQTT_QOS_EXACTLY_ONCE, true);

	get_map_request_t mapRequest = {
		.header = create_command_header(ACTION_GET_MAP_REQUEST, ROUTE_PLANNER_REPLY_TOPIC)
	};

	char *jsonPayload = get_map_request_serialize_json(&mapRequest);

	if(!jsonPayload) {
		LOG_ERROR_ASYNC("Could not serialize get_map_request message to JSON.");
	} else {
		mqtt_publish("services/api/request", jsonPayload, MQTT_QOS_EXACTLY_ONCE, false);
		free(jsonPayload);
	}

	signal_wait_for_shutdown();

	LOG_INFO_ASYNC("Shutdown signal received. Stopping Route Planner Service...");
	core_shutdown();
	signal_cleanup();
	
	return 0;
}
