/**
 * @file vehicle_config.c
 * @brief Configuration des véhicules.
 * @details
 * Ce fichier gère la configuration des véhicules, y compris les paramètres de communication UART.
 * Inspiré du fonctionnement des sockets POSIX.
 * @author Lukas Grando
 * @date 2025-11-26
 */

 #include "vehicle/vehicle_config.h"

/*
[Service]
; L'identifiant unique du véhicule dans le système
vehicle_id = 1
uart_baudrate = 115200
uart_device_path = /dev/ttyS0
uart_timeout_ms = 1000
*/
/***
 * @brief Parse la section spécifique de la configuration du service vehicle.
 * @param key Nom du paramètre
 * @param value Valeur du paramètre
 * @param serviceConfig Pointeur vers la structure spécifique du service
 */
void vehicle_service_config_parser(const char *key, const char *value, void *serviceConfig) {
    vehicle_config_t *config = (vehicle_config_t *) serviceConfig;
	
    // Conversion pour vehicle_id (int)
    if (strcmp(key, "vehicle_id") == 0) {
        config->vehicleId = atoi(value);
    }
    // Conversion pour uart_baudrate (int)
    else if (strcmp(key, "uart_baudrate") == 0) {
        config->bauderate = atoi(value);
    }
    // Copie de chaîne pour uart_device_path
    else if (strcmp(key, "uart_device_path") == 0) {
		strncpy(config->devicePath, value, sizeof(config->devicePath) - 1);
		config->devicePath[sizeof(config->devicePath) - 1] = '\0';
    }
    // Conversion pour uart_timeout_ms (int)
    else if (strcmp(key, "uart_timeout_ms") == 0) {
        config->timeoutMs = atoi(value);
    }
    else if (strcmp(key, "uart_marvelmind_port") == 0) {
        strncpy(config->marvelmindPort, value, sizeof(config->marvelmindPort) - 1);
        config->marvelmindPort[sizeof(config->marvelmindPort) - 1] = '\0';
    }
    else if (strcmp(key, "camera_socket_port") == 0) {
        config->cameraSocketPort = atoi(value);
    }
    else if (strcmp(key, "camera_socket_bind_ip") == 0) {
        strncpy(config->cameraSocketBindIp, value, sizeof(config->cameraSocketBindIp) - 1);
        config->cameraSocketBindIp[sizeof(config->cameraSocketBindIp) - 1] = '\0';
    }
    else {
        LOG_WARNING_ASYNC("Unknown key in [Service]: %s", key);
    }
}