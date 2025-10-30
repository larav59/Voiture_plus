/**
 * @file set_safe_route_mode_request.c
 * @brief Définitions du modèle de données pour la commande SET_SAFE_ROUTE_MODE_REQUEST.
 * @details
 * Adressé à : Route Planner Service
 * La commande est envoyée pour demander au route planner de définir le mode de route sans zone de conflit pour un véhicule.
 */
 
#include "core/mqtt_messages/set_safe_route_mode_request.h"

/**
 * @brief Sérialise un message de demande de définition du mode de route sans zone de conflit en JSON.
 * @param msg Pointeur vers le message de demande de définition du mode de route sans zone de conflit à sérialiser.
 * @return Chaîne JSON représentant le message de demande de définition du mode de route sans zone de conflit
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *set_safe_route_mode_request_serialize_json(const set_safe_route_mode_request_t *msg) {
	cJSON *root = cJSON_CreateObject();
	if(!root) return NULL;
	
	if(command_header_serialize(&msg->header, root) != 0) goto error_cleanup;
	if(!cJSON_AddBoolToObject(root, "enabled", msg->enabled)) goto error_cleanup;

	char *json = CJSON_PRINT(root);
	
	cJSON_Delete(root);
	return json;

	error_cleanup:
		cJSON_Delete(root);
		return NULL;
}

/**
 * @brief Désérialise un message de demande de définition du mode de route sans zone de conflit à partir d'une chaîne JSON.
 * @param json Chaîne JSON représentant le message de demande de définition du mode de route sans zone de conflit.
 * @param msg Pointeur vers la structure de message de demande de définition du mode de route sans zone de conflit à remplir.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int set_safe_route_mode_request_deserialize_json(const char *json, set_safe_route_mode_request_t *msg) {
	if (!json || !msg) return -1;

	cJSON *root = cJSON_Parse(json);
	if (!root) return -1;

	if (command_header_deserialize(root, &msg->header) != 0) goto error_cleanup;

	const cJSON *enabledItem = cJSON_GetObjectItemCaseSensitive(root, "enabled");
	if (!cJSON_IsBool(enabledItem)) goto error_cleanup;
	msg->enabled = cJSON_IsTrue(enabledItem);

	cJSON_Delete(root);
	return 0;

	error_cleanup:
		cJSON_Delete(root);
		return -1;
}