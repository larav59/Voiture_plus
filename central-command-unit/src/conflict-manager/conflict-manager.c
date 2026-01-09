/**
 * @file heartbeat.c
 * @brief Point d'entrée du service Heartbeat
 * @details
 * Ce fichier contient la fonction main du service.
 * Ce service reste à l'écoute du topics MQTT du status des autres services
 * Lorsqu'un service ne répond plus, le broker publie le message (LWT)
 */

#include "conflict-manager/conflict-manager.h"

int main(int argc, char **argv) {
	config_common_t common_config;

	core_set_service_version(CONFLICT_MANAGER_SERVICE_VERSION);
	signal_init();

	if(core_bootstrap(argc, argv, &common_config, NULL, NULL, NULL, NULL) != 0) {
		LOG_FATAL_SYNC("Failed to bootstrap core systems. Exiting.");
		return EXIT_FAILURE;
	}
	LOG_INFO_ASYNC("Conflict Manager Service started successfully.");

	mqtt_subscribe("services/conflict-manager/request", MQTT_QOS_AT_LEAST_ONCE);
	mqtt_subscribe("services/conflict-manager/response", MQTT_QOS_AT_LEAST_ONCE);

	
	signal_wait_for_shutdown();

	LOG_INFO_ASYNC("Shutdown signal received. Stopping Heartbeat Service...");
	core_shutdown();
	signal_cleanup();
	
	return 0;
}
