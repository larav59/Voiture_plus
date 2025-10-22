/**
 * @file core.h
 * @brief Bootstrap central pour les services MQTT
 * Fournit les fonctions et structures de base pour initialiser et gérer les services MQTT.
 * @author Lukas Grando
 * @date 2025-10-19
 */

#ifndef CCU_CORE_H
#define CCU_CORE_H

#include "core/check.h"
#include "core/config.h"
#include "core/mqtt.h"


#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 0

#ifdef DEBUG
	#define FAILBACK_LOGGER_LEVEL LOG_LEVEL_DEBUG
#else
	#define FAILBACK_LOGGER_LEVEL LOG_LEVEL_INFO
#endif

#define DEFAULT_CONFIG_PATH "./config.ini"


/**
 * @brief Initialise tous les sous systèmes du core
 * @details
 * - Parse la ligne de commande pour trouver le fichier de configuration
 * - Lit le fichier de configuration
 * - Initialise le client MQTT et se connecte.
 * - Initialise le logger avec un callback qui log sur la console et sur le topic MQTT dédiéabort
 * 
 * @param argc Nombre d'arguments
 * @param argv Tableau des arguments
 * @param common_config pointeur vers la structure commune de configuration
 * @param service_config Pointeur vers la structure de configuration spécifique au service
 * @param service_parser callback pour parser la section spécifique de la configuration du service
 * @return 0 en cas de succès et -1 en cas d'erreur fatale
 */
int core_bootstrap(int argc, char** argv, config_common_t* common_config, void* service_config, service_config_parser_t service_parser);


/**
 * @brief Arrête tous les sous systèmes du core
 * @details
 * - Déconnecte le client MQTT
 * - Libère les ressources allouées
 * - Arrête le système de logging
 * @note Cette fonction doit être appelée avant de quitter le programme.
 */
void core_shutdown(void);

#endif // CCU_CORE_H