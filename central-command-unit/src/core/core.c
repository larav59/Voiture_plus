/**
 * @file core.h
 * @brief Bootstrap central pour les services MQTT
 * Fournit les fonctions et structures de base pour initialiser et gérer les services MQTT.
 * @author Lukas Grando
 * @date 2025-10-19
 */

 #include "core/core.h"

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
			printf(COLOR_GREEN "%s LibCore v%d.%d.%d\n" COLOR_RESET, argv[0], VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
			exit(EXIT_SUCCESS);
		} else {
			fprintf(stderr, COLOR_RED "Error: Unknown option %s\n" COLOR_RESET, argv[i]);
			print_usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}
}


static void failback_logger_callback(log_level_t level, const char *message) {
	switch (level)
	{
		case LOG_LEVEL_DEBUG:
			printf("[DEBUG] %s", message);
			break;
		case LOG_LEVEL_INFO:
			printf(COLOR_BLUE "[INFO] %s" COLOR_RESET, message);
			break;
		case LOG_LEVEL_WARNING:
			printf(COLOR_YELLOW "[WARNING] %s" COLOR_RESET, message);
			break;
		case LOG_LEVEL_ERROR:
			fprintf(stderr, COLOR_RED "[ERROR] %s" COLOR_RESET, message);
			break;
		case LOG_LEVEL_FATAL:
			fprintf(stderr, COLOR_RED "[FATAL] %s" COLOR_RESET, message);
			break;
	}
	return;
}

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
 * @param commonConfig pointeur vers la structure commune de configuration
 * @param serviceConfig Pointeur vers la structure de configuration spécifique au service
 * @param serviceParser callback pour parser la section spécifique de la configuration du service
 * @return 0 en cas de succès et -1 en cas d'erreur fatale
 */
int core_bootstrap(int argc, char** argv, config_common_t* commonConfig, void* serviceConfig, service_config_parser_t serviceParser) {
	char *configPath = NULL;
	parse_command_line(argc, argv, &configPath);
	logger_init(FAILBACK_LOGGER_LEVEL, failback_logger_callback);
	LOG_INFO_SYNC("CORE: Starting core bootstrap...");

	int result = parse_config_file(configPath, commonConfig, serviceConfig, serviceParser);

	if(result != 0) {
		LOG_FATAL_SYNC("CORE: Failed to open configuration file '%s'.", configPath);
		return EXIT_FAILURE;
	}

	LOG_INFO_SYNC("CORE: Configuration file '%s' loaded successfully.", configPath);

	// Initialisation du client MQTT

	// Initialisation du logger final
	logger_destroy();
	logger_init(commonConfig->logging.logLevel, NULL);
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

	// Libération des ressources
	logger_destroy();

	// TODO: Déconnexion du client MQTT
	mqtt_disconnect();

}