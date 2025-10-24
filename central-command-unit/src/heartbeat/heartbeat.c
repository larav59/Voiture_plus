/**
 * @file heartbeat.c
 * @brief Point d'entrée du service Heartbeat
 * @details
 * Ce fichier contient la fonction main du service.
 * Ce service reste à l'écoute du topics MQTT du status des autres services
 * Lorsqu'un service ne répond plus, le broker publie le message (LWT)
 */

#include "core/core.h"
// #include "heartbeat/heartbeat.h"




int main(int argc, char **argv) {
	config_common_t common_config;
	core_bootstrap(argc, argv, &common_config);
}
