/**
 * @file logger.h
 * @brief Module de journalisation pour les différents services (sync et async).
 * @author Lukas
 * @date 2025-10-18
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include "fifo.h"
#include "common.h"

/**
 * @enum log_level_t
 * @brief Niveaux de journalisation
 * Définit la criticité des messages de journalisation.
 */
typedef enum {
    LOG_LEVEL_DEBUG,   /**< Messages de debugging */
    LOG_LEVEL_INFO,    /**< Messages d'information */
    LOG_LEVEL_WARNING, /**< Messages d'avertissement */
    LOG_LEVEL_ERROR    /**< Messages d'erreur */
} log_level_t;

/**
 * @typedef log_callback_t
 * @brief Fonction de rappel pour la journalisation
 * Permet de définir un traitement personnalisé pour les messages de journalisation.
 */
typedef void (*log_callback_t)(log_level_t level, const char *message);

/** @brief Niveau global de journalisation actuel */
extern log_level_t currentLogLevel;

/** @brief Longueur maximale d'un message de journalisation */
#define LOG_MESSAGE_LENGTH 1024


// Mode synchron
// En mode synchrone, les messages sont traités immédiatement
// et les appels bloquent jusqu'à ce que le message soit traité par le callback.

/**
 * @brief Initialise le système de journalisation synchrone.
 * @param level Le niveau de journalisation à utiliser. Les messages avec un niveau inférieur seront ignorés.
 * @param callback La fonction de rappel pour traiter les messages de journalisation.
 * @note Par défaut, le niveau est LOG_LEVEL_INFO.
 * @note Si aucun callback n'est défini, les messages seront affichés sur la sortie standard.
 */
void logger_init_sync(log_level_t level, log_callback_t callback);


/**
 * @brief Journalise un message en mode synchrone.
 * @param level Niveau de criticité du message.
 * @param format Chaîne de format (comme le printf).
 * @param ... Arguments pour le format.
 * @note Si le niveau du message est inférieur au niveau configuré, il est ignoré.
 */
void logger_log_sync(log_level_t level, const char *format, ...);


// Mode asynchrone
// En mode asynchrone, les messages sont placés dans une liste FIFO
// et traités par un thread dédié, permettant aux appels de ne pas bloquer.

/**
 * @brief Initialise le système de journalisation asynchrone (thread + FIFO).
 * @param level Niveau de journalisation global.
 * @param callback Fonction de rappel pour traiter les messages.
 * @note Les messages sont stockés dans une queue et traités par un thread dédié.
 */
void logger_init_async(log_level_t level, log_callback_t callback);

/**
 * @brief Journalise un message en mode asynchrone.
 * @param level Niveau de criticité du message.
 * @param format Chaîne de format (comme le printf).
 * @param ... Arguments pour le format.
 * @note Cette fonction retourne immédiatement, le message sera traité par le thread de logging.
 */
void logger_log_async(log_level_t level, const char *format, ...);

// Macros pratiques
#define LOG_DEBUG_SYNC(format, ...)    logger_log_sync(LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO_SYNC(format, ...)     logger_log_sync(LOG_LEVEL_INFO, format, ##__VA_ARGS__)
#define LOG_WARNING_SYNC(format, ...)  logger_log_sync(LOG_LEVEL_WARNING, format, ##__VA_ARGS__)
#define LOG_ERROR_SYNC(format, ...)    logger_log_sync(LOG_LEVEL_ERROR, format, ##__VA_ARGS__)

#define LOG_DEBUG_ASYNC(format, ...)   logger_log_async(LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO_ASYNC(format, ...)    logger_log_async(LOG_LEVEL_INFO, format, ##__VA_ARGS__)
#define LOG_WARNING_ASYNC(format, ...) logger_log_async(LOG_LEVEL_WARNING, format, ##__VA_ARGS__)
#define LOG_ERROR_ASYNC(format, ...)   logger_log_async(LOG_LEVEL_ERROR, format, ##__VA_ARGS__)

#endif // LOGGER_H
