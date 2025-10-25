#include "tests/runner.h"
#include "core/config.h"
#include "core/logger.h"

// Dummy callback pour éviter affichage + erreur
static void log_callback(log_level_t level, const char *msg) {
	UNUSED(level);
	UNUSED(msg);
    //printf("[TEST-CONFIG] %s\n", msg);
}

static int serviceParserCalled = 0;

typedef struct {
    int max_connections;
    bool feature_enabled;
    char mode[32];
} test_service_config_t;

static void test_service_parser(const char* key, const char* value, void* serviceConfig) {
	serviceParserCalled++;
	test_service_config_t* config = (test_service_config_t*)serviceConfig;

	if (strcmp(key, "max_connections") == 0) {
		config->max_connections = atoi(value);
	} else if (strcmp(key, "feature_enabled") == 0) {
		config->feature_enabled = (strcmp(value, "true") == 0);
	} else if (strcmp(key, "mode") == 0) {
		strncpy(config->mode, value, sizeof(config->mode) - 1);
		config->mode[sizeof(config->mode) - 1] = '\0';
	}
}

static void create_test_file(const char* filename, const char* content) {
    FILE* fp = fopen(filename, "w");
    if (fp) {
        fputs(content, fp);
        fclose(fp);
    }
}

TEST_REGISTER(test_config_success, "Test d'un fichier de configuration valide et complet") {
	logger_init(LOG_LEVEL_DEBUG, log_callback);
    const char* filename = "test_ok.ini";
    const char* content =
        "[Network]\n"
        "mqtt_broker_ip = 192.168.1.100\n"
        "mqtt_broker_port = 5672\n"
        "mqtt_client_id = test_client\n"
        "mqtt_timeout_sec = 10\n"
        "\n"
        "[Logging]\n"
        "log_level = DEBUG\n"
        "log_topic = services/test/logs\n"
        "\n"
        "[Service]\n"
        "max_connections = 100\n"
        "feature_enabled = true\n"
        "mode = standalone\n";

    create_test_file(filename, content);

    config_common_t common_cfg;
    test_service_config_t service_cfg = {0};
    serviceParserCalled = 0;

    int result = parse_config_file(filename, &common_cfg, &service_cfg, test_service_parser);

    TEST_ASSERT(result == 0, "Le parsing doit réussir");
    
    // Vérification de la section [Network]
    TEST_ASSERT(strcmp(common_cfg.network.brokerIp, "192.168.1.100") == 0, "mqtt_broker_ip incorrect");
    TEST_ASSERT(common_cfg.network.brokerPort == 5672, "mqtt_broker_port incorrect");

    // Vérification de la section [Logging]
    TEST_ASSERT(common_cfg.logging.logLevel == LOG_LEVEL_DEBUG, "log_level incorrect");
    TEST_ASSERT(strcmp(common_cfg.logging.topic, "services/test/logs") == 0, "topic incorrect");

    // Vérification de la section [Service] via le callback
    TEST_ASSERT(serviceParserCalled == 3, "Le parser de service doit être appelé 3 fois");
    TEST_ASSERT(service_cfg.max_connections == 100, "max_connections incorrect");
    TEST_ASSERT(service_cfg.feature_enabled == true, "feature_enabled incorrect");
    TEST_ASSERT(strcmp(service_cfg.mode, "standalone") == 0, "mode incorrect");

    remove(filename);
	logger_destroy();
}

TEST_REGISTER(test_config_file_not_found, "Test d'un fichier de configuration non trouvé") {
	logger_init(LOG_LEVEL_DEBUG, log_callback);
    config_common_t common_cfg;
    int result = parse_config_file("non_existent_file.ini", &common_cfg, NULL, NULL);
    TEST_ASSERT(result != 0, "Le parsing doit échouer pour un fichier non trouvé");
	logger_destroy();
}

TEST_REGISTER(test_config_missing_key, "Test d'un fichier avec une clé obligatoire manquante") {
	logger_init(LOG_LEVEL_DEBUG, log_callback);
    const char* filename = "test_missing.ini";
    const char* content =
        "[Network]\n"
        "mqtt_broker_ip = 192.168.1.100\n"
        /* mqtt_broker_port est manquant */
        "mqtt_client_id = test_client\n"
        "mqtt_timeout_sec = 10\n"
        "\n"
        "[Logging]\n"
        "log_level = INFO\n"
        "log_topic = services/test/logs\n";
    create_test_file(filename, content);

    config_common_t common_cfg;
    int result = parse_config_file(filename, &common_cfg, NULL, NULL);
    TEST_ASSERT(result != 0, "Le parsing doit échouer si une clé est manquante");

    remove(filename);
	logger_destroy();
}

TEST_REGISTER(test_config_invalid_value, "Test avec une valeur invalide pour log_level") {
	logger_init(LOG_LEVEL_DEBUG, log_callback);
    const char* filename = "test_invalid.ini";
    const char* content =
        "[Network]\n"
        "mqtt_broker_ip = 192.168.1.100\n"
        "mqtt_broker_port = 5672\n"
        "mqtt_client_id = test_client\n"
        "mqtt_timeout_sec = 10\n"
        "[Logging]\n"
        "log_level = GIBBERISH\n" // Valeur invalide
        "log_topic = services/test/logs\n";
    
    create_test_file(filename, content);

    config_common_t common_cfg;
    int result = parse_config_file(filename, &common_cfg, NULL, NULL);
    TEST_ASSERT(result != 0, "Le parsing doit échouer pour une valeur de log_level invalide");

    remove(filename);
	logger_destroy();
}

TEST_REGISTER(test_config_syntax_error, "Test d'un fichier avec une erreur de syntaxe INI") {
	logger_init(LOG_LEVEL_DEBUG, log_callback);
    const char* filename = "test_syntax.ini";
    const char* content =
        "[Network]\n"
        "this is not a valid line\n";
    
    create_test_file(filename, content);

    config_common_t common_cfg;
    int result = parse_config_file(filename, &common_cfg, NULL, NULL);
    TEST_ASSERT(result != 0, "Le parsing doit échouer en cas d'erreur de syntaxe");

    remove(filename);
	logger_destroy();
}