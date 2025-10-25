
#include "core/config.h"
#include "ini.h"

/**
 * @brief Structure interne pour suivre les champs obligatoires trouvés.
 * @internal
 */
typedef struct {
    bool brokerIp;
    bool brokerPort;
    bool clientId;
    bool logLevel;
    bool topic;
    bool timeoutSec;
} fields_tracker_t;

/**
 * @brief Structure interne pour passer toutes les données nécessaires au handler inih.
 * @internal
 */
typedef struct {
    config_common_t* common_config;
    void* service_config;
    service_config_parser_t service_parser;
    fields_tracker_t tracker;
} config_payload_t;

/**
 * @brief Convertit une chaîne de caractères en enum log_level_t.
 * @param str La chaîne à convertir (ex: "DEBUG"). La comparaison est insensible à la casse.
 * @param level Pointeur pour stocker le résultat.
 * @return 0 en cas de succès, -1 si la chaîne est invalide.
 */
static int log_level_from_string(const char* str, log_level_t* level) {
    if (strcasecmp(str, "DEBUG") == 0) *level = LOG_LEVEL_DEBUG;
    else if (strcasecmp(str, "INFO") == 0) *level = LOG_LEVEL_INFO;
    else if (strcasecmp(str, "WARN") == 0) *level = LOG_LEVEL_WARNING;
    else if (strcasecmp(str, "ERROR") == 0) *level = LOG_LEVEL_ERROR;
    else if (strcasecmp(str, "FATAL") == 0) *level = LOG_LEVEL_FATAL;
    else return -1; // Valeur non reconnue
    return 0;
}


static int config_handler(void* user, const char* section, const char* name, const char* value) {
    config_payload_t* payload = (config_payload_t*)user;
    config_common_t* config = payload->common_config;

    #define MATCH(s, n) strcasecmp(section, s) == 0 && strcasecmp(name, n) == 0
    
    if (MATCH("Network", "mqtt_broker_ip")) {
        strncpy(config->network.brokerIp, value, sizeof(config->network.brokerIp) - 1);
        config->network.brokerIp[sizeof(config->network.brokerIp) - 1] = '\0';
        payload->tracker.brokerIp = true;
    } else if (MATCH("Network", "mqtt_broker_port")) {
        config->network.brokerPort = (uint16_t)strtoul(value, NULL, 10);
        payload->tracker.brokerPort = true;
    } else if (MATCH("Network", "mqtt_client_id")) {
        strncpy(config->network.clientId, value, sizeof(config->network.clientId) - 1);
        config->network.clientId[sizeof(config->network.clientId) - 1] = '\0';
        payload->tracker.clientId = true;
    } else if(MATCH("Network", "mqtt_timeout_sec")) {
        config->network.timeoutSec = (uint32_t)strtoul(value, NULL, 10);
        payload->tracker.timeoutSec = true;
    } else if (MATCH("Logging", "log_Level")) {
        if (log_level_from_string(value, &config->logging.logLevel) == 0) {
            payload->tracker.logLevel = true;
        } else {
            LOG_ERROR_ASYNC("CONFIG: Invalid logLevel '%s' in configuration.", value);
            config->logging.logLevel = LOG_LEVEL_INFO; // Valeur par défaut en cas d'erreur
        }
    } else if (MATCH("Logging", "log_topic")) {
        strncpy(config->logging.topic, value, sizeof(config->logging.topic) - 1);
        config->logging.topic[sizeof(config->logging.topic) - 1] = '\0';
        payload->tracker.topic = true;
    } else if (strcasecmp(section, "Service") == 0) {
        if (payload->service_parser && payload->service_config) {
            payload->service_parser(name, value, payload->service_config);
        }
    } else {
        LOG_WARNING_ASYNC("CONFIG: Unrecognized section or key '[%s] %s'", section, name);
    }
    return 1; // Succès, on continue le parsing
}

/**
 * @brief Parse un fichier INI de configuration
 */
int parse_config_file(const char* filename, config_common_t* common, void* serviceConfig, service_config_parser_t parser) {
    if (!filename || !common) {
        LOG_ERROR_SYNC("CONFIG: Invalid arguments to parse_config_file.");
        return -1;
    }

    config_payload_t payload = {
        .common_config = common,
        .service_config = serviceConfig,
        .service_parser = parser,
        .tracker = {0}
    };

    memset(common, 0, sizeof(config_common_t));
    CHECK_ALLOC(common);

    // Appel du parser de la bibliothèque inih
    int result = ini_parse(filename, config_handler, &payload);

    if (result < 0) {
        LOG_ERROR_SYNC("CONFIG: Can't load '%s'", filename);
        return -1;
    }
    if (result > 0) {
        LOG_ERROR_SYNC("CONFIG: Parse error in '%s' at line %d", filename, result);
        return -1;
    }

    fields_tracker_t* t = &payload.tracker;
    if (!t->brokerIp || !t->brokerPort || !t->clientId || !t->logLevel || !t->topic || !t->timeoutSec) {
        LOG_ERROR_SYNC("CONFIG: One or more required fields are missing in '%s'.", filename);
        return -1;
    }

    return 0; // Succès
}