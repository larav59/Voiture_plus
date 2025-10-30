/**
 * @file core/signal.c
 * @brief Gestion des signaux pour les services.
 * @details Ce fichier contient les définition et les fonctions pour gérer l'arrêt propre des services via des signaux système.
 * @author Lukas Grando
 * @date 2025-10-30
 */

#include "core/signal.h"
#include <signal.h>

static sem_t shutdownSem;

static void handle_signal(int signum) {
	UNUSED(signum);
	// ici on ne passe pas par send_shutdown pour éviter une récursion potentielle
	CHECK_SEM_RAW(sem_post(&shutdownSem)); 
}

/**
 * @brief Initialise la gestion des signaux pour l'arrêt propre des services.
 * @details Cette fonction met en place les gestionnaires de signaux nécessaires pour
 * permettre un arrêt propre des services en réponse à des signaux système.
 * @note Elle utilise une sémaphore interne pour signaler l'arrêt.
 * @return void
 */
void signal_init(void) {
	CHECK_SEM_RAW(sem_init(&shutdownSem, 0, 0));

	struct sigaction sa;
	sa.sa_handler = handle_signal;
	sigfillset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	CHECK_CRITICAL_RAW(sigaction(SIGINT, &sa, NULL) == 0, "Failed to set SIGINT handler");
	CHECK_CRITICAL_RAW(sigaction(SIGTERM, &sa, NULL) == 0, "Failed to set SIGTERM handler");
}

/**
 * @brief Attend un signal d'arrêt.
 * @details Cette fonction bloque l'exécution jusqu'à ce qu'un signal d'arrêt soit reçu
 * via la sémaphore interne.
 * @return void
 */
void signal_wait_for_shutdown(void) {
	CHECK_SEM_RAW(sem_wait(&shutdownSem));
}

/**
 * @brief envoie un signal d'arrêt.
 * @details Cette fonction libère la sémaphore interne pour signaler aux autres parties
 * du programme qu'un arrêt est demandé.
 * @return void
 */
void signal_send_shutdown(void) {
	CHECK_SEM_RAW(sem_post(&shutdownSem));
}

/**
 * @brief Nettoie les ressources liées à la gestion des signaux.
 * @details Cette fonction libère les ressources allouées pour la gestion des signaux.
 * @return void
 */
void signal_cleanup(void) {
	CHECK_SEM_RAW(sem_destroy(&shutdownSem));
}
