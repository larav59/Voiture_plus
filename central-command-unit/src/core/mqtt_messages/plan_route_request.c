/**
 * @file planify_route_request.c
 * @brief Définitions du modèle de données pour les requêtes de planification de trajet.
 * @author Lukas Grando
 * @date 2025-10-27
 */

#include "core/mqtt_messages/plan_route_request.h"

/**
 * @brief Sérialise un message de demande de planification de trajet en JSON.
 * @param msg Pointeur vers le message de demande de planification de trajet à sérialiser.
 * @return Chaîne JSON représentant le message de demande de planification de trajet
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *plan_route_request_serialize(const plan_route_request_t *msg) {
	cJSON *root = NULL;
	char *jsonString = NULL;

	if (!msg) return NULL;

	root = cJSON_CreateObject();
	if (!root) return NULL;

	if (command_header_serialize(&msg->header, root) != 0) goto cleanup;
	if (!cJSON_AddNumberToObject(root, "carId", msg->carId)) goto cleanup;

	cJSON *nodeArray = cJSON_CreateArray();
	if (!nodeArray) {
		goto cleanup;
	}
	
	if (!cJSON_AddItemToObject(root, "nodeList", nodeArray)) goto cleanup;

	for (int i = 0; i < msg->nodeCount; i++) {
		cJSON *nodeIdNum = cJSON_CreateNumber(msg->nodeIds[i]);
		if (!nodeIdNum) {
			goto cleanup;
		}
		cJSON_AddItemToArray(nodeArray, nodeIdNum);
	}

	jsonString = CJSON_PRINT(root);
	if (!jsonString) goto cleanup;

	cleanup:
		cJSON_Delete(root);
	
	return jsonString;
}

/**
 * @brief Désérialise un message de demande de planification de trajet à partir d'une chaîne JSON.
 * @param root Pointeur vers l'objet cJSON représentant le message de demande de planification de trajet.
 * @param msg Pointeur vers la structure de message de demande de planification de trajet à remplir.
 * @warning Cette fonction s'occupe uniquement de désérialiser les données. Le header doit être désérialisé séparément.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int plan_route_request_data_deserialize(cJSON *root, plan_route_request_t *msg) {

	const cJSON *carIdItem = cJSON_GetObjectItemCaseSensitive(root, "carId");
	if (!cJSON_IsNumber(carIdItem)) return -1;

	msg->carId = carIdItem->valueint;
	const cJSON *nodeArray = cJSON_GetObjectItemCaseSensitive(root, "nodeList");
	if (!cJSON_IsArray(nodeArray)) return -1;

	msg->nodeCount = cJSON_GetArraySize(nodeArray);
	msg->nodeIds = malloc(sizeof(int) * msg->nodeCount);
	if (!msg->nodeIds) return -1;

	for (int i = 0; i < msg->nodeCount; i++) {
		const cJSON *nodeNumberItem = cJSON_GetArrayItem(nodeArray, i);

		if (!cJSON_IsNumber(nodeNumberItem)) {
			free(msg->nodeIds);
			return -1;
		}
		
		msg->nodeIds[i] = nodeNumberItem->valueint;
	}
	return 0;
}