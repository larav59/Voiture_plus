/**
 * @file marvelmind_wrapper.c
 * @brief Wrapper pour l'intégration de Marvelmind dans le projet.
 * @author Lukas Grando, Noé François
 * @date 2025-12-05
 */

#include "vehicle/marvelmind_wrapper.h"

static struct MarvelmindHedge *g_hedge = NULL;
static marvelmind_pos_callback_t g_user_callback = NULL;

/**
 * @brief Type de fonction de callback pour la réception des positions Marvelmind.
 * @param x Position X en millimètres.
 * @param y Position Y en millimètres.
 * @param angle Angle en degrés.
 */
typedef void (*marvelmind_pos_callback_t)(int32_t x, int32_t y, double angle);

static void marvelmind_internal_callback(struct PositionValue position) {
    if (g_user_callback) {
        g_user_callback(position.x, position.y, position.angle);
    }
}

/**
 * @brief Initialise Marvelmind et enregistre le callback de réception.
 * @param ttyDevice Chemin vers le port série.
 * @param callback La fonction à appeler à chaque nouvelle position.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int marvelmind_init(const char* ttyDevice, marvelmind_pos_callback_t callback) {
	if (g_hedge != NULL) {
		LOG_ERROR_ASYNC("Marvelmind: Déjà initialisé.");
		return -1;
	}

	g_hedge = createMarvelmindHedge();
	if (g_hedge == NULL) {
		LOG_ERROR_ASYNC("Marvelmind: Échec de la création de MarvelmindHedge.");
		return -1;
	}

	g_user_callback = callback;
	g_hedge->ttyFileName = ttyDevice;
	g_hedge->receiveDataCallback = marvelmind_internal_callback;
	g_hedge->pause = true;

	startMarvelmindHedge(g_hedge);

	LOG_INFO_ASYNC("Marvelmind initialisé avec succès sur %s.", ttyDevice);
	return 0;
}

/**
 * @brief Démarre l'acquisition des données de Marvelmind.
 * @details Lance le thread de lecture des données. Appelle le callback à chaque nouvelle position.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int marvelmind_start_acquisition(void) {
	if (g_hedge == NULL) {
		LOG_ERROR_ASYNC("Marvelmind: Non initialisé.");
		return -1;
	}

	g_hedge->pause = false;
	LOG_INFO_ASYNC("Acquisition Marvelmind démarrée.");
	return 0;
}

/**
 * @brief Arrête l'acquisition des données de Marvelmind.
 * @details Termine le thread de lecture des données.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int marvelmind_stop_acquisition(void) {
	if (g_hedge == NULL) {
		LOG_ERROR_ASYNC("Marvelmind: Non initialisé.");
		return -1;
	}

	g_hedge->pause = true;
	LOG_INFO_ASYNC("Acquisition Marvelmind arrêtée.");
	return 0;
}

/**
 * @brief Nettoie les ressources allouées pour Marvelmind.
 * @details Libère la mémoire et ferme le port série.
 */
void marvelmind_cleanup(void) {
	if (g_hedge != NULL) {
		destroyMarvelmindHedge(g_hedge);
		g_hedge = NULL;
	}
}