/**
 * @file vehicle_state_message.c
 * @brief Définitions du modèle de données pour les messages de télémétrie.
 * @details
 * Ce fichier contient les définitions des structures de données utilisées pour l'état du véhicule.
 */
 #include "core/mqtt_messages/vehicle_state_message.h"

/**
 * @brief Sérialise un message de l'état véhicule en JSON.
 * @param msg Pointeur vers le message de l'état véhicule à sérialiser.
 * @return Chaîne JSON représentant le message de l'état véhicule
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *vehicle_state_message_serialize_json(const vehicle_state_message_t *msg) {
	cJSON *root = cJSON_CreateObject();
	if (!root) return NULL;

	if (!cJSON_AddNumberToObject(root, "carId", msg->carId)) goto error_cleanup;
	if (!cJSON_AddNumberToObject(root, "timestamp", (double)msg->timestamp)) goto error_cleanup;
	
	if (!cJSON_AddNumberToObject(root, "x", msg->x)) goto error_cleanup;
	if (!cJSON_AddNumberToObject(root, "y", msg->y)) goto error_cleanup;
	if (!cJSON_AddNumberToObject(root, "angle", msg->angle)) goto error_cleanup;
	if (!cJSON_AddNumberToObject(root, "speed", msg->speed)) goto error_cleanup;
	
	if (!cJSON_AddBoolToObject(root, "isNavigating", msg->isNavigating)) goto error_cleanup;
	if (!cJSON_AddBoolToObject(root, "obstacleDetected", msg->obstacleDetected)) goto error_cleanup;

	char *json = CJSON_PRINT(root);
	cJSON_Delete(root);

	return json;

	error_cleanup:
		cJSON_Delete(root);
		return NULL;
}