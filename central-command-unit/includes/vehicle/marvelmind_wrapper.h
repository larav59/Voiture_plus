/**
 * @file marvelmind_wrapper.h
 * @brief Wrapper pour l'intégration de Marvelmind dans le projet.
 * @author Lukas Grando, Noé François
 * @date 2025-12-05
 */

#ifndef MARVELMIND_WRAPPER_H
#define MARVELMIND_WRAPPER_H

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <marvelmind.h>

#include "core/logger.h"

/**
 * @brief Type de fonction de callback pour la réception des positions Marvelmind.
 * @param x Position X en millimètres.
 * @param y Position Y en millimètres.
 * @param angle Angle en degrés.
 * @param context Contexte utilisateur (peut être NULL).
 */
typedef void (*marvelmind_pos_callback_t)(int32_t x, int32_t y, double angle, void* context);


/**
 * @brief Initialise Marvelmind et enregistre le callback de réception.
 * @param ttyDevice Chemin vers le port série.
 * 
 * @param callback La fonction à appeler à chaque nouvelle position.
 */
int marvelmind_init(const char* ttyDevice, marvelmind_pos_callback_t callback, void* context);

/**
 * @brief Démarre l'acquisition des données de Marvelmind.
 * @details Lance le thread de lecture des données. Appelle le callback à chaque nouvelle position.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int marvelmind_start_acquisition(void);

/**
 * @brief Arrête l'acquisition des données de Marvelmind.
 * @details Termine le thread de lecture des données.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int marvelmind_stop_acquisition(void);

/**
 * @brief Nettoie les ressources allouées pour Marvelmind.
 * @details Libère la mémoire et ferme le port série.
 */
void marvelmind_cleanup(void);

#endif // MARVELMIND_WRAPPER_H