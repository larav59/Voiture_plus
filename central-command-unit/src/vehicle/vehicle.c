/**
 * @file vehicle.c
 * @brief Point d'entrée du "service" Vehicle
 * @details
 * Ce fichier contient la fonction main du service.
 * Ce service reste à l'écoute du topics MQTT du status des autres services
 * Lorsqu'un service ne répond plus, le broker publie le message (LWT)
 */

#include "vehicle/vehicle.h"


int g_fd = -1;

static void on_camera_objects_received(const camera_detected_object_t* objects, int count, void* context) {
	UNUSED(context);
	LOG_INFO_ASYNC("Camera: Received %d objects.", count);
	for (int i = 0; i < count; i++) {
		LOG_INFO_ASYNC(" - Object %d: Category=%s, Confidence=%.2f, Box=(x=%.2f,y=%.2f,w=%.2f,h=%.2f)",
			i,
			camera_category_to_string(objects[i].category),
			objects[i].confidence,
			objects[i].box.x,
			objects[i].box.y,
			objects[i].box.w,
			objects[i].box.h);
	}
}

static void on_position_received(int32_t x, int32_t y, double angle) {
    while (angle > 180.0) angle -= 360.0;
    while (angle < -180.0) angle += 360.0;

    int16_t angle_int = (int16_t)(angle * 100);
    protocol_send_position_telemetry(g_fd, (int16_t)x, (int16_t)y, angle_int);
}

int main(int argc, char **argv) {
	config_common_t common_config;
	vehicle_config_t vehicle_config;
	char lwtPayload[255];
	char lwtTopic[255];

	core_set_service_version(VEHICLE_VERSION);
	signal_init();

	if(core_bootstrap(argc, argv, &common_config, (void *) &vehicle_config, vehicle_service_config_parser, NULL, NULL) != 0) {
		LOG_FATAL_SYNC("Failed to bootstrap core systems. Exiting.");
		return EXIT_FAILURE;
	}
	
	// todo: changer bootstrap pour avoir un lwt_builder à la place de payload + topic separés
	// comme ça on peut faire des lwt dynamiques basé sur la config

	get_vehicle_lwt_message(vehicle_config.vehicleId, false, lwtPayload, sizeof(lwtPayload));
	get_vehicle_lwt_topic(vehicle_config.vehicleId, lwtTopic, sizeof(lwtTopic));
	
	int rc = mqtt_set_lwt(lwtTopic, lwtPayload);
	if(rc != 0) {
		LOG_FATAL_SYNC("Failed to set LWT for Vehicle service. Exiting.");
		return EXIT_FAILURE;
	}

	LOG_INFO_ASYNC("Vehicle started successfully.");

	mqtt_set_message_callback(vehicle_message_callback);

    uart_config_t uart_conf = {
		.baudrate = vehicle_config.bauderate,
		.timeoutMs = vehicle_config.timeoutMs,
		.devicePath = vehicle_config.devicePath
	};

    g_fd = uart_open(&uart_conf);
    if (g_fd < 0) {
        LOG_FATAL_SYNC("Impossible d'ouvrir l'UART pour le test.");
        core_shutdown();
		signal_cleanup();
        return EXIT_FAILURE;
    }

    if(marvelmind_init(vehicle_config.marvelmindPort, on_position_received) != 0) {
        LOG_FATAL_SYNC("Échec de l'initialisation de Marvelmind.");
        core_shutdown();
        signal_cleanup();
        return EXIT_FAILURE;
    }

    marvelmind_start_acquisition();

	camera_socket_t cam_socket;
	if (camera_server_init(&cam_socket, vehicle_config.cameraSocketBindIp, vehicle_config.cameraSocketPort, on_camera_objects_received, NULL) != 0) {
		LOG_FATAL_SYNC("Échec de l'initialisation du serveur caméra.");
		core_shutdown();
		signal_cleanup();
		return EXIT_FAILURE;
	}

	/*if (camera_server_start(&cam_socket) != 0) {
		LOG_FATAL_SYNC("Échec du démarrage du serveur caméra.");
		camera_server_cleanup(&cam_socket);
		core_shutdown();
		signal_cleanup();
		return EXIT_FAILURE;
	}*/
	
	signal_wait_for_shutdown();

	LOG_INFO_ASYNC("Shutdown signal received. Stopping Vehicle Service...");
	core_shutdown();
	signal_cleanup();	
    marvelmind_stop_acquisition();
    marvelmind_cleanup();
	camera_server_stop(&cam_socket);
	camera_server_cleanup(&cam_socket);
	return 0;
}
