/**
 * @file conflict.c
 * @brief Définitions pour la gestion des verrous de zones de conflit
 * @details
 * Ce fichier contient la définition des structures et fonctions utilisées pour gérer les zones de conflit et dépassements.
 * Elle utilisera un système de table de hachage pour suivre les verrous actifs. Et proposera des fonctions pour :
 * - Demander un verrou sur une zone
 * - Libérer un verrou
 * - Vérifier l'état d'un verrou
 */
#include "conflict-manager/conflict.h"

typedef struct {
	char key[64]; ///< Clé unique du verrou (origin-target-rule)
	int ownerId; ///< ID du véhicule propriétaire du verrou
	uint64_t decayTimeMs; ///< Timestamp d'expiration du verrou
	priority_queue_t *waitingQueue; ///< File d'attente des véhicules en attente
	UT_hash_handle hh; ///< Handle pour la table de hachage uthash
} conflict_lock_entry_t;

static conflict_lock_entry_t *lockHead = NULL;
static sem_t lockSem;
static int64_t g_defaultDecayTimeMs = 10000; // 10 secondes par défaut

static uint64_t get_current_time_ms() {
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (uint64_t)(ts.tv_sec) * 1000 + (ts.tv_nsec / 1000000);
}

static void generate_conflict_key(int p1, int p2, char *buffer, size_t size) {
	int min = (p1 < p2) ? p1 : p2;
	int max = (p1 < p2) ? p2 : p1;
	snprintf(buffer, size, "CONFLICT_%d_%d", min, max);
}

static bool promote_next_vehicle(conflict_lock_entry_t *entry, int *promotedId) {
	if (!entry->waitingQueue || pq_is_empty(entry->waitingQueue)) {
		return false;
	}

	void *rawId = pq_pop(entry->waitingQueue);
	int nextId = (int) rawId;

	entry->ownerId = nextId;
	entry->decayTimeMs = get_current_time_ms() + g_defaultDecayTimeMs;

	if (promotedId) *promotedId = nextId;
	
	return true;
}

/**
 * @brief Initialise le gestionnaire de conflits.
 */
void conflict_init(int64_t defaultDecayTimeMs) {
	lockHead = NULL;
	g_defaultDecayTimeMs = defaultDecayTimeMs;
	sem_init(&lockSem, 0, 1);
}

/**
 * @brief Tente de verrouiller un segment de route.
 * @param origin ID du noeud de départ.
 * @param target ID du noeud d'arrivée.
 * @param rule Règle de conduite (DRIVE_RIGHT, DRIVE_LEFT...).
 * @param vehiculeId ID du véhicule demandant le verrou.
 * @param[out] promotedVehicleId ID du véhicule promu au verrou ou -1 si aucun.
 * @return conflict_lock_status_t indiquant le résultat de la tentative de verrouillage.
 */
conflict_lock_status_t conflict_lock_lane(int origin, int target, lane_rule_t rule, conflict_priority_t priority, int vehicleId, int *promotedVehicleId) {
	if(rule != LANE_RULE_ONE_WAY) return CONFLICT_ERROR;

	char key[MAX_KEY_LEN];
	generate_conflict_key(origin, target, key, sizeof(key));

	uint64_t currentTime = get_current_time_ms();
	uint64_t decayTime = currentTime + g_defaultDecayTimeMs;

	sem_wait(&lockSem);
	conflict_lock_entry_t *entry = NULL;
	HASH_FIND_STR(lockHead, key, entry);

	// Cas d'une nouvelle ressource encore inconnue
	if(!entry) {
		entry = (conflict_lock_entry_t *)malloc(sizeof(conflict_lock_entry_t));
		if(!entry) {
			sem_post(&lockSem);
			return CONFLICT_ERROR;
		}
		strncpy(entry->key, key, MAX_KEY_LEN);
		entry->ownerId = vehicleId;
		entry->decayTimeMs = decayTime;
		entry->waitingQueue = pq_create(DEFAULT_QUEUE_CAPACITY);

		HASH_ADD_STR(lockHead, key, entry);
		sem_post(&lockSem);
		LOG_DEBUG_ASYNC("Lock granted immediately for vehicle %d on lane %s", vehicleId, key);
		return CONFLICT_GRANTED;
	}

	// Cas ou la ressource existe déjà

	if(entry->ownerId == vehicleId) {
		sem_post(&lockSem);
		return CONFLICT_ERROR;
	}

	if(entry->ownerId != -1 && currentTime > entry->decayTimeMs) {
		LOG_WARNING_ASYNC("Conflict: Vehicle %d lock on s%s expired, releasing.", entry->ownerId, key);
		bool promoted = promote_next_vehicle(entry, promotedVehicleId);
		if(!promoted) {
			entry->ownerId = -1;
			*promotedVehicleId = -1;
		} else {
			LOG_DEBUG_ASYNC("Lock on %s transferred to vehicle %d due to expiration.", key, *promotedVehicleId);
		}
	}

	// On ajoute le demandeur à la file
	// Comme les id de véhicule sont des int, on les caste en pointeur c'est plus rapide et pratique)
	pq_push(entry->waitingQueue, (void *) vehicleId, priority);
	LOG_INFO_ASYNC("Vehicle %d added to waiting queue for %s", vehicleId, key);
	sem_post(&lockSem);
	return CONFLICT_WAITING;
}

/**
 * @brief Libère un segment de route.
 * * @param origin ID du noeud de départ.
 * @param target ID du noeud d'arrivée.
 * @param rule Règle de conduite utilisée.
 * @param vehicle_id ID du véhicule (seul le propriétaire peut déverrouiller).
 * * @return true si déverrouillé avec succès.
 * @return false si erreur (mauvais propriétaire ou pas de verrou).
 */
conflict_lock_status_t conflict_unlock_lane(int origin, int target, lane_rule_t rule, int vehicleId, int *promotedVehicleId) {
	if(rule != LANE_RULE_ONE_WAY) return CONFLICT_ERROR;

	char key[MAX_KEY_LEN];
	generate_conflict_key(origin, target, key, sizeof(key));

	sem_wait(&lockSem);
	conflict_lock_entry_t *entry = NULL;
	HASH_FIND_STR(lockHead, key, entry);

	// Tentative de libérer une ressource inconnue
	if(!entry) {
		sem_post(&lockSem);
		return CONFLICT_ERROR;
	}

	// Tentative de libérer une ressource non possédée
	if(entry->ownerId != vehicleId) {
		LOG_WARNING_ASYNC("Illegal unlock attempt by vehicle %d on %s owned by vehicle %d", vehicleId, key, entry->ownerId);
		sem_post(&lockSem);
	}

	bool promoted = promote_next_vehicle(entry, promotedVehicleId);
	if(!promoted) {
		pq_destroy(entry->waitingQueue);
		HASH_DEL(lockHead, entry);
		free(entry);
		LOG_DEBUG_ASYNC("Lock on %s fully released, no waiting vehicles.", key);
	} else {
		LOG_DEBUG_ASYNC("Lock on %s transferred to vehicle %d.", key, *promotedVehicleId);
	}
	sem_post(&lockSem);
	return CONFLICT_GRANTED;
}

/**
 * @brief Fonction de maintenance (Garbage Collector).
 * @details Parcourt la table de hachage pour supprimer les verrous expirés.
 * À appeler régulièrement (ex: toutes les secondes).
 */
void conflict_cleanup(void) {

}

/**
 * @brief (Debug) Affiche l'état actuel des verrous dans les logs.
 */
void conflict_dump_state(void) {

}

/**
 * @brief Détruit le gestionnaire et libère la mémoire.
 */
void conflict_destroy(void) {

}
