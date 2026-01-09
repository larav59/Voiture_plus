/**
 * @file marvelmind_wrapper.c
 * @brief Wrapper pour l'intégration de Marvelmind dans le projet.
 * @author Lukas Grando, Noé François
 * @date 2025-12-05
 */

#include "vehicle/marvelmind_wrapper.h"

static struct MarvelmindHedge *g_hedge = NULL;
static marvelmind_pos_callback_t g_user_callback = NULL;

static pthread_t g_threadId;
static sem_t g_dataSem;
static volatile bool g_threadRunning = false;
static bool g_IsInitialized = false;
void *g_context = NULL;

/**
 * @brief Callback interne pour signaler la réception de nouveaux paquets.
 * @details Cette fonction est assignée à `anyInputPacketCallback` de MarvelmindHedge.
 * Afin d'appeler notre thread de travail uniquement lorsqu'un nouveau paquet est reçu réduisant l'utilisation CPU.
 */
static void internal_sem_callback(void) {
	sem_post(&g_dataSem);
}

/**
 * @brief Thread de travail qui récupère la position.
 */
static void* marvelmind_worker_thread(void* arg) {
	UNUSED(arg);

	struct PositionValue pos;
	bool havePosition;
	
	while (g_threadRunning) {
		int s = sem_wait(&g_dataSem);
		if (s == 0 && g_threadRunning && g_hedge != NULL) {
			havePosition = getPositionFromMarvelmindHedge(g_hedge, &pos);
			if (havePosition && g_user_callback) {
				LOG_DEBUG_ASYNC("Marvelmind: Received position: x=%d, y=%d, angle=%.2f", pos.x, pos.y, pos.angle);
				g_user_callback(pos.x, pos.y, pos.angle, g_context);
			}
		}
	}
	return NULL;
}

/**
 * @brief Initialise Marvelmind et enregistre le callback de réception.
 * @param ttyDevice Chemin vers le port série.
 * @param callback La fonction à appeler à chaque nouvelle position.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int marvelmind_init(const char* ttyDevice, marvelmind_pos_callback_t callback, void* context) {
	if (g_IsInitialized) {
		return 0;
	}
	g_context = context;

	g_hedge = createMarvelmindHedge();
	if (g_hedge == NULL) {
		LOG_ERROR_ASYNC("Marvelmind: Memory allocation failed.");
		return -1;
	}

	if (sem_init(&g_dataSem, 0, 0) != 0) {
		LOG_ERROR_ASYNC("Marvelmind: Semaphore initialization failed.");
		destroyMarvelmindHedge(g_hedge);
		return -1;
	}

	g_user_callback = callback;
	g_hedge->ttyFileName = ttyDevice;
	g_hedge->anyInputPacketCallback = internal_sem_callback;

	startMarvelmindHedge(g_hedge);
	
	g_IsInitialized = true;
	LOG_DEBUG_ASYNC("Marvelmind: Initialized on %s", ttyDevice);
	return 0;
}

/**
 * @brief Démarre l'acquisition des données de Marvelmind.
 * @details Lance le thread de lecture des données. Appelle le callback à chaque nouvelle position.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int marvelmind_start_acquisition(void) {
	if (!g_IsInitialized) {
		LOG_ERROR_ASYNC("Marvelmind: Not initialized.");
		return -1;
	}
	
	if (g_threadRunning) return 0;
	g_hedge->pause = false;
	g_threadRunning = true;

	int ret = pthread_create(&g_threadId, NULL, marvelmind_worker_thread, NULL);
	if (ret != 0) {
		LOG_ERROR_ASYNC("Marvelmind: Unable to create thread.");
		g_threadRunning = false;
		return -1;
	}

	LOG_DEBUG_ASYNC("Marvelmind: Started acquisition thread.");
	return 0;
}

/**
 * @brief Arrête l'acquisition des données de Marvelmind.
 * @details Termine le thread de lecture des données.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int marvelmind_stop_acquisition(void) {
	if (!g_IsInitialized || !g_threadRunning) return 0;

	LOG_DEBUG_ASYNC("Marvelmind: Stopping thread...");
	
	g_hedge->pause = true;
	g_threadRunning = false;

	sem_post(&g_dataSem);
	pthread_join(g_threadId, NULL);
	
	LOG_DEBUG_ASYNC("Marvelmind: Thread stopped.");
	return 0;
}

/**
 * @brief Nettoie les ressources allouées pour Marvelmind.
 * @details Libère la mémoire et ferme le port série.
 */
void marvelmind_cleanup(void) {
	marvelmind_stop_acquisition();

	if (g_hedge != NULL) {
		stopMarvelmindHedge(g_hedge);
		destroyMarvelmindHedge(g_hedge);
		g_hedge = NULL;
	}
	
	if (g_IsInitialized) {
		sem_destroy(&g_dataSem);
		g_IsInitialized = false;
	}
}