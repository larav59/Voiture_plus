/**
 * @file cancel_vehicle_route_request.c
 * @brief Définitions du modèle de données pour la commande CANCEL_VEHICLE_ROUTE_REQUEST.
 * @details
 * Adressé à : Route Planner Service
 * La commande est envoyé qu'on veut que le route planner annule le trajet en cours d'un véhicule.
 */

#include "core/mqtt_messages/cancel_vehicle_route_request.h"

/**
 * @brief Sérialise un message de demande d'annulation de trajet véhicule en JSON.
 * @param msg Pointeur vers le message de demande d'annulation de trajet véhicule à sérialiser.
 * @return Chaîne JSON représentant le message de demande d'annulation de trajet véhicule
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *cancel_vehicle_route_request_serialize_json(const cancel_vehicle_route_request_t *msg) {
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
 * @brief Désérialise un message CANCEL_VEHICLE_ROUTE_REQUEST à partir d'une chaîne JSON.
 * @param json Chaîne JSON représentant le message CANCEL_VEHICLE_ROUTE_REQUEST.
 * @param msg Pointeur vers la structure de message CANCEL_VEHICLE_ROUTE_REQUEST à remplir.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int cancel_vehicle_route_request_deserialize_json(const char *json, cancel_vehicle_route_request_t *msg) {
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