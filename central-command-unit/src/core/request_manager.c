/**
 * @file request_manager.c
 * @brief Gère le suivi des requêtes MQTT en attente de réponse.
 * @details Utilise une hashmap pour corréler les réponses
 * @note L'implémentation est thread-safe
 * @author Lukas Grando
 * @date 2025-11-11
 */
#include "core/request_manager.h"

typedef struct {
    char requestId[COMMAND_ID_LENGTH]; // Clé de la HashMap
    response_callback_t callback;      // Pointeur vers la fonction de rappel
    void* context;                     // Pointeur de contexte (sac à dos)
    UT_hash_handle hh;                 // Structure interne pour uthash
} hash_entry_t;

static sem_t g_rmAccessSem; // Semaphore pour l'accès thread-safe
static hash_entry_t *g_requestMap = NULL; // Hashmap des requêtes en attente

/**
 * @brief Initialise le gestionnaire de requêtes. Hashmap + semaphore.
 * @note Doit être appelé une fois au démarrage du service.
 */
void request_manager_init(void) {
	int res = sem_init(&g_rmAccessSem, 0, 1);
	g_requestMap = NULL;
	CHECK_SEM(res);

}

/**
 * @brief Nettoie le gestionnaire de requêtes. Libère la Hashmap et le semaphore.
 */
void request_manager_destroy(void) {
	hash_entry_t *current_entry, *tmp;
	sem_wait(&g_rmAccessSem);
	HASH_ITER(hh, g_requestMap, current_entry, tmp) {
		HASH_DEL(g_requestMap, current_entry);
		free(current_entry);
	}
	sem_post(&g_rmAccessSem);

	int res = sem_destroy(&g_rmAccessSem);
	CHECK_SEM(res);
}

/**
 * @brief Enregistre une nouvelle requête en attente, son callback et son contexte.
 * @param requestId L'ID de la requête qui sera envoyée.
 * @param callback La fonction à appeler lorsque la réponse arrivera.
 * @param context le contexte à passer au callback lors de l'appel pouvant être utilisé pour stocker des données spécifiques.
 * @return 0 en cas de succès, -1 si l'ID existe déjà ou en cas d'erreur.
 */
int request_manager_register(const char* requestId, response_callback_t callback, void* context) {
	if(requestId == NULL) {
		LOG_ERROR_ASYNC("Invalid parameters to request_manager_register");
		return -1;
	}
	if(callback == NULL) {
		// On peut laisser pour faciliter le dev mais on loggue un warning
		// Les requêtes sans gestion de réponse n'ont pas besoin d'être enregistrées
		LOG_WARNING_ASYNC("Registering request with NULL callback: %s", requestId);
	}

	hash_entry_t *newEntry = (hash_entry_t*)malloc(sizeof(hash_entry_t));
	if(newEntry == NULL) {
		LOG_ERROR_ASYNC("Memory allocation failed in request_manager_register");
		return -1;
	}

	strncpy(newEntry->requestId, requestId, COMMAND_ID_LENGTH);
	newEntry->callback = callback;
	newEntry->context = context;
	sem_wait(&g_rmAccessSem);

	hash_entry_t *found = NULL;
	HASH_FIND_STR(g_requestMap, requestId, found);
	
	if(found != NULL) {
		sem_post(&g_rmAccessSem);
		free(newEntry);
		LOG_ERROR_ASYNC("Request ID already registered: %s", requestId);
		return -1;
	}

	HASH_ADD_STR(g_requestMap, requestId, newEntry);
	sem_post(&g_rmAccessSem);
	LOG_DEBUG_ASYNC("Registered request ID: %s", requestId);
	return 0;

}

/**
 * @brief Traite un payload de réponse entrant.
 * @details Extrait l'id de la commande, et cherche le callback
 * correspondant dans la hashmap. Si trouvé, appelle le callback avec le payload et le contexte.
 * @param payload Le payload JSON brut reçu.
 * @return 0 si la réponse a été trouvée et traitée, -1 si elle était inattendue ou invalide.
 */
int request_manager_process_response(const char* payload) {
	if(!payload) {
		LOG_ERROR_ASYNC("Invalid payload in request_manager_process_response");
		return -1;
	}
	// command_response_header_deserialize
	cJSON *json = cJSON_Parse(payload);
	if(!json) {
		LOG_ERROR_ASYNC("Failed to parse JSON payload in request_manager_process_response");
		return -1;
	}

	command_response_header_t responseHeader;
	if(command_response_header_deserialize(json, &responseHeader) != 0) {
		LOG_ERROR_ASYNC("Invalid response header in payload");
		cJSON_Delete(json);
		return -1;
	}

	sem_wait(&g_rmAccessSem);
	hash_entry_t *found = NULL;
	HASH_FIND_STR(g_requestMap, responseHeader.commandId, found);
	if(!found) {
		sem_post(&g_rmAccessSem);
		cJSON_Delete(json);
		return -1;
	}

	// Retirer de la hashmap avant d'appeler le callback pour éviter les récursions
	HASH_DEL(g_requestMap, found);
	sem_post(&g_rmAccessSem);
	// Appeler le callback
	if(found->callback) found->callback(json, &responseHeader, found->context);
	
	free(found);
	cJSON_Delete(json);
	LOG_DEBUG_ASYNC("Processed response for request ID: %s", responseHeader.commandId);
	return 0;
}
