/**
 * @file config.h
 * @brief Module pour lire les fichiers de configuration pour les services.
 * Les fichiers de configurations sont au format INI simple.
 * Chaque fichier de configuration devra inclure les section suivantes :
 * - [Network] : Configuration réseau (IP, port, etc.)
 * - [Logging] : Configuration du système de journalisation (niveau, fichier, etc.)
 * - [Service] : Paramètres spécifiques au service.
 * Les section Network et Logging sont gérées par le core.
 * La section Service est spécifique et doit être gérée via une fonction de callback.
 * @author Lukas Grando
 * @date 2025-10-19
 */
#ifndef CORE_CONFIG_H
#define CORE_CONFIG_H

#include "core/check.h"


/**
 * @brief Structure de configuration réseau
 * Contient les paramètres réseau communs à tous les services.
 * @note IP au format chaine (ex:"192.168.1.1")
 * @note Port au format uint16_t (0-65535)
 */
typedef struct {
	char brokerIp[16];
	uint16_t brokerPort;
} network_config_t;

typedef struct {
	log_level_t logLevel;
	char topic[128];
} logging_config_t;

typedef struct {
	network_config_t network;
	logging_config_t logging;
} config_common_t;

/**
 * @brief Callback pour parser la section [Service] propre à chaque service
 * @param key Nom du paramètre
 * @param value Valeur du paramètre
 * @param serviceConfig Pointeur vers la structure spécifique du service
 */
typedef void (*service_config_parser_t)(const char* key, const char* value, void* serviceConfig);

/**
 * @brief Parse un fichier INI de configuration
 * @param filename Nom du fichier INI
 * @param common Pointeur vers la structure pour stocker les sections [Network] et [Log]
 * @param serviceConfig Pointeur vers la structure spécifique du service
 * @param parser Callback pour parser la section [Service]
 * @return 0 si succès, non zéro sinon
 * @note En cas d'erreur, des messages sont loggés via le système de journalisation.
 * @note Un code retour de -1 indique une erreur fatale empêchant le démarrage du service.
 */
int parse_config_file(const char* filename, config_common_t* common, void* serviceConfig, service_config_parser_t parser);

#endif // CORE_CONFIG_H