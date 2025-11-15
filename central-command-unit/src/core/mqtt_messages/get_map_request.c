/**
 * @file get_map_request.c
 * @brief Définitions du modèle de données pour la requête de récupération de la carte.
 * @author Lukas Grando
 * @date 2025-10-27
 */
#include "core/mqtt_messages/get_map_request.h"

/**
 * @brief Sérialise un message de demande de récupération de carte en JSON.
 * @param msg Pointeur vers le message de demande de récupération de carte à sérialiser.
 * @return Chaîne JSON représentant le message de demande de récupération de carte
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *get_map_request_serialize_json(const get_map_request_t *msg) {
	cJSON *root = cJSON_CreateObject();
	if(!root) return NULL;
	if(command_header_serialize(&msg->header, root) != 0) goto error_cleanup;

	char *json = CJSON_PRINT(root);
	cJSON_Delete(root);
	LOG_DEBUG_ASYNC("Serialized get_map_request JSON: %s", json);
	return json;

	error_cleanup:
		cJSON_Delete(root);
		return NULL;
}

/**
 * @brief Désérialise un message de demande de récupération de carte à partir d'une chaîne JSON.
 * @param json Chaîne JSON représentant le message de demande de récupération de carte.
 * @param msg Pointeur vers la structure de message de demande de récupération de carte à remplir.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int get_map_request_deserialize_json(const char *json, get_map_request_t *msg) {
	if( !json || !msg) return -1;
	cJSON *root = cJSON_Parse(json);
	if(!root) return -1;
	if(command_header_deserialize(root, &msg->header) != 0) goto error_cleanup;
	
	cJSON_Delete(root);
	return 0;

	error_cleanup:
		cJSON_Delete(root);
		return -1;
}