/**
 * @file logger.h
 * @brief Module de journalisation pour les différents services (sync et async).
 * @author Lukas
 * @date 2025-10-18
 */
#include "core/logger.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct {
	char *msg;
	log_level_t level;
} log_node_t;

// Variable globale pour le niveau de journalisation
log_level_t currentLogLevel = LOG_LEVEL_INFO;

// Mode Synchrone
static log_callback_t logCallbackSync = NULL;

// Mode asynchrone
static sem_t logSem; // Sémaphore pour signaler la présence de nouveaux messages
static fifo_t logFifo; // File d'attente FIFO pour les messages de log
static pthread_t loggerThread; // Thread de journalisation
static log_callback_t logCallbackAsync = NULL; // Callback pour le mode asynchrone
static int loggerThreadRunning = 0; // Indicateur d'exécution du thread

/**
 * @brief Initialise le système de journalisation synchrone.
 * @param level Le niveau de journalisation à utiliser. Les messages avec un niveau inférieur seront ignorés.
 * @param callback La fonction de rappel pour traiter les messages de journalisation.
 * @note Par défaut, le niveau est LOG_LEVEL_INFO.
 * @note Si aucun callback n'est défini, les messages seront affichés sur la sortie standard.
 */
void logger_init_sync(log_level_t level, log_callback_t callback) {
	currentLogLevel = level;
	logCallbackSync = callback;
}


/**
 * @brief Journalise un message en mode synchrone.
 * @param level Niveau de criticité du message.
 * @param format Chaîne de format (comme le printf).
 * @param ... Arguments pour le format.
 * @note Si le niveau du message est inférieur au niveau configuré, il est ignoré.
 */
void logger_log_sync(log_level_t level, const char *format, ...) {
	if(level < currentLogLevel) {
		return;
	}
	char message[LOG_MESSAGE_LENGTH];
	va_list args;
	va_start(args, format);
	vsnprintf(message, LOG_MESSAGE_LENGTH, format, args);
	va_end(args);

	if(logCallbackSync) {
		logCallbackSync(level, message);
	} else {
		fprintf(stderr, "[%d] %s\n", level, message);
	}
}


// Mode asynchrone

/**
 * @private
 * @brief Fonction du thread de journalisation asynchrone.
 * @param arg Argument non utilisé.
 * @return NULL
 */
static void *logger_thread_function(void *arg) {
	UNUSED(arg);
	while(loggerThreadRunning) {
		sem_wait(&logSem);
		log_node_t *node = fifo_pop(&logFifo);
		if(node) {
			if(logCallbackAsync) {
				logCallbackAsync(node->level, node->msg);
			} else {
				fprintf(stderr, "[%d] %s\n", node->level, node->msg);
			}
			free(node->msg);
			free(node);
		}
	}
	return NULL;
}

/**
 * @brief Initialise le système de journalisation asynchrone (thread + FIFO).
 * @param level Niveau de journalisation global.
 * @param callback Fonction de rappel pour traiter les messages.
 * @note Les messages sont stockés dans une queue et traités par un thread dédié.
 */
void logger_init_async(log_level_t level, log_callback_t callback) {
	currentLogLevel = level;
	logCallbackAsync = callback;
	sem_init(&logSem, 0, 0);
	fifo_init(&logFifo);
	loggerThreadRunning = 1;

	pthread_create(&loggerThread, NULL, logger_thread_function, NULL);
	pthread_detach(loggerThread);
}

/**
 * @brief Journalise un message en mode asynchrone.
 * @param level Niveau de criticité du message.
 * @param format Chaîne de format (comme le printf).
 * @param ... Arguments pour le format.
 * @note Cette fonction retourne immédiatement, le message sera traité par le thread de logging.
 */
void logger_log_async(log_level_t level, const char *format, ...) {
	if(level < currentLogLevel) {
		return;
	}
	char *message = malloc(LOG_MESSAGE_LENGTH);
	CHECK_ALLOC(message);

	va_list args;
	va_start(args, format);
	vsnprintf(message, LOG_MESSAGE_LENGTH, format, args);
	va_end(args);

	log_node_t *node = malloc(sizeof(log_node_t));
	CHECK_ALLOC(node);
	node->msg = message;
	node->level = level;

	fifo_push(&logFifo, node);
	sem_post(&logSem);
}

// Macros pratiques
#define LOG_DEBUG_SYNC(format, ...)    logger_log_sync(LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO_SYNC(format, ...)     logger_log_sync(LOG_LEVEL_INFO, format, ##__VA_ARGS__)
#define LOG_WARNING_SYNC(format, ...)  logger_log_sync(LOG_LEVEL_WARNING, format, ##__VA_ARGS__)
#define LOG_ERROR_SYNC(format, ...)    logger_log_sync(LOG_LEVEL_ERROR, format, ##__VA_ARGS__)

#define LOG_DEBUG_ASYNC(format, ...)   logger_log_async(LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO_ASYNC(format, ...)    logger_log_async(LOG_LEVEL_INFO, format, ##__VA_ARGS__)
#define LOG_WARNING_ASYNC(format, ...) logger_log_async(LOG_LEVEL_WARNING, format, ##__VA_ARGS__)
#define LOG_ERROR_ASYNC(format, ...)   logger_log_async(LOG_LEVEL_ERROR, format, ##__VA_ARGS__)


