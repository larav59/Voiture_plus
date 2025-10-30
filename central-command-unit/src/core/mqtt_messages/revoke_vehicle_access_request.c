/**
 * @file revoke_vehicle_access.h
 * @brief Définitions du modèle de données pour la commande REVOKE_VEHICLE_ACCESS.
 * @details
 * Adressé à : Conflict Manager Service
 * La commande est envoyé pour annuler les demande de ressource en cours d'un véhicule.
 * Le gestionnaire de conflit va également bloqué la ressource actuellement utilisé par le véhicule pour éviter les accidents.
 */

#include "core/mqtt_messages/revoke_vehicle_access_request.h"

/**
 * @brief Sérialise un message de révocation d'accès véhicule en JSON.
 * @param msg Pointeur vers le message de révocation d'accès véhicule à sérialiser.
 * @return Chaîne JSON représentant le message de révocation d'accès véhicule
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *revoke_vehicle_access_serialize_json(const revoke_vehicle_access_t *msg) {
	cJSON *root = cJSON_CreateObject();
	if(!root) return NULL;
	
	if(command_header_serialize(&msg->header, root) != 0) goto error_cleanup;
	if(!cJSON_AddNumberToObject(root, "carId", msg->carId)) goto error_cleanup;
	
	char *json = CJSON_PRINT(root);
	
	cJSON_Delete(root);
	return json;

	error_cleanup:
		cJSON_Delete(root);
		return NULL;
}

/**
 * @brief Désérialise un message de révocation d'accès véhicule à partir d'une chaîne JSON.
 * @param json Chaîne JSON représentant le message de révocation d'accès véhicule.
 * @param msg Pointeur vers la structure de message de révocation d'accès véhicule à remplir.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int revoke_vehicle_access_deserialize_json(const char *json, revoke_vehicle_access_t *msg) {
	if (!json || !msg) return -1;

	cJSON *root = cJSON_Parse(json);
	if (!root) return -1;

	if (command_header_deserialize(root, &msg->header) != 0) goto error_cleanup;

	const cJSON *carIdItem = cJSON_GetObjectItemCaseSensitive(root, "carId");

	if (!cJSON_IsNumber(carIdItem)) goto error_cleanup;

	msg->carId = carIdItem->valueint;
	cJSON_Delete(root);
	return 0;

	error_cleanup:
		cJSON_Delete(root);
		return -1;
}