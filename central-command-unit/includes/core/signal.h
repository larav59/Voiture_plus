/**
 * @file includes/core/signal.h
 * @brief Gestion des signaux pour les services.
 * @details Ce fichier contient les définitions et les fonctions pour gérer l'arrêt propre des services via des signaux système.
 * @author Lukas Grando
 * @date 2025-10-30
 */
#ifndef CORE_SIGNAL_H
#define CORE_SIGNAL_H

#include "core/common.h"
#include "core/check.h"


/**
 * @brief Initialise la gestion des signaux pour l'arrêt propre des services.
 * @details Cette fonction met en place les gestionnaires de signaux nécessaires pour
 * permettre un arrêt propre des services en réponse à des signaux système.
 * @note Elle utilise une sémaphore interne pour signaler l'arrêt.
 * @return void
 */
void signal_init(void);

/**
 * @brief Attend un signal d'arrêt.
 * @details Cette fonction bloque l'exécution jusqu'à ce qu'un signal d'arrêt soit reçu
 * via la sémaphore interne.
 * @return void
 */
void signal_wait_for_shutdown(void);

/**
 * @brief envoie un signal d'arrêt.
 * @details Cette fonction libère la sémaphore interne pour signaler aux autres parties
 * du programme qu'un arrêt est demandé.
 * @return void
 */
void signal_send_shutdown(void);

/**
 * @brief Nettoie les ressources liées à la gestion des signaux.
 * @details Cette fonction libère les ressources allouées pour la gestion des signaux.
 * @return void
 */
void signal_cleanup(void);






#endif // CORE_SIGNAL_H