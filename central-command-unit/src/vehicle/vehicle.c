/**
 * @file vehicle.c
 * @brief Point d'entrée du "service" Vehicle
 * @details
 * Ce fichier contient la fonction main du service.
 * Ce service reste à l'écoute du topics MQTT du status des autres services
 * Lorsqu'un service ne répond plus, le broker publie le message (LWT)
 */

#include "vehicle/vehicle.h"

int main(int argc, char **argv) {
	config_common_t common_config;
	vehicle_config_t vehicle_config;

	core_set_service_version(VEHICLE_VERSION);
	signal_init();

	if(core_bootstrap(argc, argv, &common_config, (void *) &vehicle_config, NULL, NULL, NULL) != 0) {
		LOG_FATAL_SYNC("Failed to bootstrap core systems. Exiting.");
		return EXIT_FAILURE;
	}

	LOG_INFO_ASYNC("Vehicle started successfully.");

	signal_wait_for_shutdown();

	LOG_INFO_ASYNC("Shutdown signal received. Stopping Route Planner Service...");
	core_shutdown();
	signal_cleanup();	
	return 0;
}
