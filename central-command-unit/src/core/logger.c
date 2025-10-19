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

// Etat du logger
log_level_t currentLogLevel = LOG_LEVEL_INFO;
static log_callback_t logCallback = NULL;
bool loggerInitialized = false; // Utile pour les macros de vérification

// Gestion du mode asynchrone
static sem_t logSem; // Sémaphore pour signaler la présence de nouveaux messages
static fifo_t logFifo; // File d'attente FIFO pour les messages de log
static pthread_t loggerThread; // Thread de journalisation
static int loggerThreadRunning = 0; // Indicateur d'exécution du thread


/**
 * @private
 * @brief Fonction du thread de journalisation asynchrone.
 * @param arg Argument non utilisé.
 * @return NULL
 */
static void *logger_thread_function(void *arg) {
	UNUSED(arg);
	while(1) {
		sem_wait(&logSem);
		if(!loggerThreadRunning) break;
		log_node_t *node = fifo_pop(&logFifo);
		if(node) {
			if(logCallback) {
				logCallback(node->level, node->msg);
			} else {
				fprintf(stderr, "[%d] %s\n", node->level, node->msg);
			}
			free(node->msg);
			free(node);
		}
	}

	// Nettoyage des messages restants
	while(!fifo_is_empty(&logFifo)) {
		log_node_t *node = fifo_pop(&logFifo);
		if(node) {
			if(logCallback) {
				logCallback(node->level, node->msg);
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
 * @brief  Initialise le système de log global (callback + thread async prêt)
 * @param level Niveau de journalisation global.
 * @param callback Fonction de rappel pour traiter les messages.
 * @note Les messages sont stockés dans une queue et traités par un thread dédié.
 */
void logger_init(log_level_t level, log_callback_t callback) {
	currentLogLevel = level;
	logCallback = callback;
	loggerInitialized = true;

	fifo_init(&logFifo);
	CHECK_SEM_RAW(sem_init(&logSem, 0, 0));

	loggerThreadRunning = 1;
	CHECK_PTHREAD_RAW(pthread_create(&loggerThread, NULL, logger_thread_function, NULL));
}

/**
 * @brief Journalise un message en mode synchrone.
 * @param level Niveau de criticité du message.
 * @param format Chaîne de format (comme le printf).
 * @param ... Arguments pour le format.
 * @note Si le niveau du message est inférieur au niveau configuré, il est ignoré.
 */
void logger_log_sync(log_level_t level, const char *format, ...) {
	if(!loggerInitialized || level < currentLogLevel) {
		return;
	}

	char message[LOG_MESSAGE_LENGTH];
	va_list args;
	va_start(args, format);
	vsnprintf(message, LOG_MESSAGE_LENGTH, format, args);
	va_end(args);

	if(logCallback) {
		logCallback(level, message);
	} else {
		fprintf(stderr, "[%d] %s\n", level, message);
	}
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
	CHECK_ALLOC_RAW(message);

	va_list args;
	va_start(args, format);
	vsnprintf(message, LOG_MESSAGE_LENGTH, format, args);
	va_end(args);

	log_node_t *node = malloc(sizeof(log_node_t));
	CHECK_ALLOC_RAW(node);
	node->msg = message;
	node->level = level;

	fifo_push(&logFifo, node);
	sem_post(&logSem);
}

/**
 * @brief Libère les ressources du système de journalisation.
 * @note Cette fonction doit être appelée pour nettoyer le thread et les sémaphores.
 */
void logger_destroy(void) {
	if(!loggerInitialized) {
		return;
	}

	loggerThreadRunning = 0;
	sem_post(&logSem);
	pthread_join(loggerThread, NULL);

	CHECK_SEM_RAW(sem_destroy(&logSem));
	loggerInitialized = false;
}