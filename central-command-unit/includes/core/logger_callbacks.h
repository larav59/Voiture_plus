/**
 * @file logger_callbacks.h
 * @brief Déclarations des callbacks de journalisation personnalisés. Propres au projet.
 */

#ifndef LOGGER_CALLBACKS_H
#define LOGGER_CALLBACKS_H

#include "core/logger.h"
#include "core/mqtt.h"
#include "core/telemetry_message.h"

/**
 * @brief Callback de log qui écrit sur la console avec couleurs.
 * @param level Niveau de log.
 * @param message Message à logger.
 */
void console_log_callback(log_level_t level, const char *message);

/**
 * @brief Callback de log qui écrit sur la console et publie sur MQTT.
 * @param level Niveau de log.
 * @param message Message à logger.
 * @warning l'appel de mqtt_log_callback_init doit être fait avant d'utiliser ce callback.
 */
void mqtt_log_callback(log_level_t level, const char *message);


/**
 * @brief Initialise les informations nécessaires pour le callback de log MQTT.
 * @param topic Topic MQTT pour les logs.
 * @param clientId ID client MQTT.
 */
void mqtt_log_callback_init(const char* topic, const char* clientId);


#endif // LOGGER_CALLBACKS_H
