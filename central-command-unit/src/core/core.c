/**
 * @file core.c
 * @brief Bootstrap central pour les services MQTT
 * Fournit les fonctions et structures de base pour initialiser et gérer les services MQTT.
 * @author Lukas Grando
 * @date 2025-10-19
 */
 #include "core/core.h"
 #include "core/logger_callbacks.h"

 static char registeredServiceVersion[50] = "??? Service v?.?.?";


 static void print_usage(const char* program_name) {
	printf(COLOR_YELLOW "Usage: %s [options]\n" COLOR_RESET, program_name);
	printf(COLOR_YELLOW "Options:\n" COLOR_RESET);
	printf(COLOR_YELLOW "  -c, --config <file>    Specify the configuration file path (default: ./config.ini)\n" COLOR_RESET);
	printf(COLOR_YELLOW "  -h, --help             Show this help message and exit\n" COLOR_RESET);
	printf(COLOR_YELLOW "  -v, --version          Show the version information and exit\n" COLOR_RESET);
 }

 static void parse_command_line(int argc, char** argv, char** config_path) {
	*config_path = DEFAULT_CONFIG_PATH;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) {
			if (i + 1 < argc) {
				*config_path = argv[i + 1];
				i++;
			} else {
				LOG_FATAL_SYNC("Error: Missing argument for %s\n", argv[i]);
				print_usage(argv[0]);
				exit(EXIT_FAILURE);
			}
		} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			print_usage(argv[0]);
			exit(EXIT_SUCCESS);
		} else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
			printf(COLOR_GREEN "%s", registeredServiceVersion);
			printf( " using libcore v%d.%d.%d\n" COLOR_RESET, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
			exit(EXIT_SUCCESS);
		} else {
			fprintf(stderr, COLOR_RED "Error: Unknown option %s\n" COLOR_RESET, argv[i]);
			print_usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}
}

/**
 * @brief Initialise tous les sous systèmes du core
 * @details
 * - Parse la ligne de commande pour trouver le fichier de configuration
 * - Lit le fichier de configuration
 * - Initialise le client MQTT et se connecte.
 * - Initialise le logger avec un callback qui log sur la console et sur le topic MQTT dédié.
 * @param argc Nombre d'arguments
 * @param argv Tableau des arguments
 * @param commonConfig pointeur vers la structure commune de configuration
 * @param serviceConfig Pointeur vers la structure de configuration spécifique au service
 * @param serviceParser callback pour parser la section spécifique de la configuration du service
 * @return 0 en cas de succès et -1 en cas d'erreur fatale
 */
int core_bootstrap(
    int argc, 
    char** argv, 
    config_common_t* commonConfig, 
    void* serviceConfig, 
    service_config_parser_t serviceParser,
	char *lwtPayload,
	char *lwtTopic
) {
    char *configPath = NULL;
	// Failback logger
	logger_init(FAILBACK_LOGGER_LEVEL, console_log_callback);
    parse_command_line(argc, argv, &configPath);

    LOG_INFO_SYNC("CORE: Starting core bootstrap...");

    int result = parse_config_file(configPath, commonConfig, serviceConfig, serviceParser);
    if(result != 0) {
        LOG_FATAL_SYNC("CORE: Failed to open configuration file '%s'.", configPath);
        return -1;
    }
    LOG_INFO_SYNC("CORE: Configuration file '%s' loaded successfully.", configPath);

	result = mqtt_connect(
		commonConfig->network.brokerIp, 
		commonConfig->network.brokerPort, 
		commonConfig->network.clientId,
		lwtTopic,
		lwtPayload
	);

	if(result != 0) {
		LOG_FATAL_SYNC("CORE: Failed to initialize MQTT client.");
		return -1;
	}

	if(mqtt_wait_for_connection(commonConfig->network.timeoutSec) != 0) {
		LOG_FATAL_SYNC("CORE: MQTT connection timed out.");
		return -1;
	}

	mqtt_log_callback_init(commonConfig->logging.topic, commonConfig->network.clientId);
	logger_destroy();
	logger_init(commonConfig->logging.logLevel, mqtt_log_callback);
	LOG_INFO_SYNC("CORE: MQTT logger initialized successfully.");

	request_manager_init();

    return 0; // Succès
}

/**
 * @brief Enregistre la chaîne de version du service.
 * @details Doit être appelée par main() avant core_bootstrap().
 * @param serviceVersion Chaîne de version (ex: "Heartbeat v1.0.0").
 * @note La version du service doit tenir dans moins de 50 caractères. Sinon, elle sera tronquée.
 */
void core_set_service_version(const char* serviceVersion) {
	strncpy(registeredServiceVersion, serviceVersion, sizeof(registeredServiceVersion) - 1);
	registeredServiceVersion[sizeof(registeredServiceVersion) - 1] = '\0';
}


/**
 * @brief Arrête tous les sous systèmes du core
 * @details
 * - Déconnecte le client MQTT
 * - Libère les ressources allouées
 * - Arrête le système de logging
 * @note Cette fonction doit être appelée avant de quitter le programme.
 */
void core_shutdown(void) {
	LOG_INFO_SYNC("CORE: Shutting down core...");
	logger_destroy();
	mqtt_disconnect();
}