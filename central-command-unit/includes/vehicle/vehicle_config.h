/**
 * @file vehicle_config.h
 * @brief Définitions de la configuration spécifique au véhicule.
 * @author Lukas Grando
 * @date 2025-11-25
 */
#ifndef VEHICLE_CONFIG_H
#define VEHICLE_CONFIG_H

#include "core/config.h"
#include <ini.h>

typedef struct {
	int vehicleId; /**< L'identifiant unique du véhicule dans le système */
	int bauderate;   /**< La vitesse de communication UART (en bauds) */
	char devicePath[256]; /**< Le chemin du périphérique UART (ex: /dev/ttyS0) */
	int timeoutMs;  /**< Le délai d'attente pour les opérations UART (en millisecondes) */
} vehicle_config_t;

// typedef void (*service_config_parser_t)(const char* key, const char* value, void* serviceConfig);

/***
 * @brief Parse la section spécifique de la configuration du service vehicle.
 * @param key Nom du paramètre
 * @param value Valeur du paramètre
 * @param serviceConfig Pointeur vers la structure spécifique du service
 */
void vehicle_service_config_parser(const char *key, const char *value, void *serviceConfig);

#endif // VEHICLE_CONFIG_H