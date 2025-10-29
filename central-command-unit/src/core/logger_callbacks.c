
#include "core/logger_callbacks.h"

static char gLogTopic[128];
static char gClientId[64];

/**
 * @brief Callback de log qui écrit sur la console avec couleurs.
 * @param level Niveau de log.
 * @param message Message à logger.
 */
void console_log_callback(log_level_t level, const char *message) {
	// Ton code pour printf avec les couleurs et les \n
	switch (level) {
		case LOG_LEVEL_DEBUG:   printf("[DEBUG] %s\n", message); break;
		case LOG_LEVEL_INFO:    printf(COLOR_BLUE "[INFO] %s" COLOR_RESET "\n", message); break;
		case LOG_LEVEL_WARNING: fprintf(stderr, COLOR_YELLOW "[WARNING] %s" COLOR_RESET "\n", message); break;
		case LOG_LEVEL_ERROR:   fprintf(stderr, COLOR_RED "[ERROR] %s" COLOR_RESET "\n", message); break;
		case LOG_LEVEL_FATAL:   fprintf(stderr, COLOR_RED "[FATAL] %s" COLOR_RESET "\n", message); break;
	}
}


/**
 * @brief Callback de log qui écrit sur la console et publie sur MQTT.
 * @param level Niveau de log.
 * @param message Message à logger.
 * @warning l'appel de mqtt_log_callback_init doit être fait avant d'utiliser ce callback.
 */
void mqtt_log_callback(log_level_t level, const char *message) {
	console_log_callback(level, message);
	
	// 2. Log sur MQTT
	if (gLogTopic[0] != '\0' && gClientId[0] != '\0' && mqtt_is_connected()) {
		telemetry_message_t telemetryMsg = {
			.timestamp = time(NULL),
			.level = level,
			.origin = gClientId,
			.message = (char *) message
		};
		char *json = telemetry_message_serialize_json(&telemetryMsg);
		if (json) {
			mqtt_publish(gLogTopic, json, MQTT_QOS_AT_LEAST_ONCE, false);
			free(json);
		}
	}
}

/**
 * @brief Initialise les informations nécessaires pour le callback de log MQTT.
 * @param topic Topic MQTT pour les logs.
 * @param clientId ID client MQTT.
 */
void mqtt_log_callback_init(const char* topic, const char* clientId) {
	// Copie des informations nécessaires
	if (topic) {
		strncpy(gLogTopic, topic, sizeof(gLogTopic) - 1);
		gLogTopic[sizeof(gLogTopic) - 1] = '\0';
	} else {
		gLogTopic[0] = '\0';
	}

	if (clientId) {
		strncpy(gClientId, clientId, sizeof(gClientId) - 1);
		gClientId[sizeof(gClientId) - 1] = '\0';
	} else {
		gClientId[0] = '\0';
	}
}
