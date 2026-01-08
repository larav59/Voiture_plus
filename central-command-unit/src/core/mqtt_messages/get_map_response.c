/**
 * @file get_map_response.c
 * @brief Définit le message de réponse pour la requête de carte.
 * @details
 * Ce fichier contient la définition de la structure de données
 * utilisée pour répondre aux requêtes de carte via MQTT.
 * @author Lukas Grando
 * @date 2025-11-19
 */
#include "core/mqtt_messages/get_map_response.h"

/**
 * @brief Sérialise une réponse de demande de carte en JSON.
 * @param msg Pointeur vers le message de demande de carte à sérialiser.
 * @return Chaîne JSON représentant le message de demande de carte
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *get_map_response_serialize(const get_map_response_t *msg) {
	if (!msg) return NULL;

	cJSON *root = cJSON_CreateObject();
	if (!root) return NULL;

	if (command_response_header_to_json(&msg->header, root) != 0) goto error;

	if (!msg->map) goto finish;
	
	cJSON *nodesArray = cJSON_CreateArray();
	if (!nodesArray) goto error;

	cJSON_AddItemToObject(root, "nodes", nodesArray);

	for (int i = 0; i < msg->map->numNodes; i++) {
		node_t *node = &msg->map->nodes[i];
		
		cJSON *nodeObj = cJSON_CreateObject();
		if (!nodeObj) goto error;
		cJSON_AddItemToArray(nodesArray, nodeObj);

		cJSON_AddNumberToObject(nodeObj, "id", node->id);
		cJSON_AddNumberToObject(nodeObj, "type", (int)node->type);
		cJSON_AddNumberToObject(nodeObj, "x", node->x);
		cJSON_AddNumberToObject(nodeObj, "y", node->y);


		cJSON *edgesArray = cJSON_CreateArray();
		if (!edgesArray) goto error;
		cJSON_AddItemToObject(nodeObj, "edges", edgesArray);

		edge_t *edge = node->edges;
		while (edge) {
			cJSON *edgeObj = cJSON_CreateObject();
			if (!edgeObj) goto error;
			cJSON_AddItemToArray(edgesArray, edgeObj);

			cJSON_AddNumberToObject(edgeObj, "target", edge->targetNode->id);
			cJSON_AddNumberToObject(edgeObj, "weight", (double)edge->weight);
			cJSON_AddNumberToObject(edgeObj, "rule", (int)edge->drivingRule);

			edge = edge->nextEdge;
		}
	}

	finish:;
		char *jsonString = CJSON_PRINT(root);
		cJSON_Delete(root);
		return jsonString;

	error:
		cJSON_Delete(root);
		return NULL;
}

/**
 * @brief Désérialise un message de demande de carte à partir d'une chaîne JSON.
 * @param root Pointeur vers l'objet cJSON représentant le message de demande de carte.
 * @param msg Pointeur vers la structure de message de demande de carte à remplir.
 * @warning Cette fonction s'occupe uniquement de désérialiser les données. Le header doit être désérialisé séparément.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int get_map_response_data_deserialize(cJSON *root, get_map_response_t *msg) {
	if (!root || !msg) return -1;
	if (!root) return -1;

	cJSON *nodesArray = cJSON_GetObjectItem(root, "nodes");
	
	if (!nodesArray) return 0;

	if (!cJSON_IsArray(nodesArray)) return -1;

	int numNodes = cJSON_GetArraySize(nodesArray);
	
	msg->map = graph_create(numNodes);
	if (!msg->map) return -1;


	cJSON *nodeJson = NULL;
	int index = 0;
	cJSON_ArrayForEach(nodeJson, nodesArray) {
		cJSON *idItem = cJSON_GetObjectItem(nodeJson, "id");
		cJSON *xItem = cJSON_GetObjectItem(nodeJson, "x");
		cJSON *yItem = cJSON_GetObjectItem(nodeJson, "y");
		cJSON *typeItem = cJSON_GetObjectItem(nodeJson, "type");

		if (cJSON_IsNumber(idItem) && cJSON_IsNumber(xItem) && cJSON_IsNumber(yItem)) {
			graph_init_node(msg->map, idItem->valueint, xItem->valuedouble, yItem->valuedouble, (node_type_t)typeItem->valueint);
		}
		msg->map->nodes[index].id = idItem->valueint;
		index++;
		LOG_DEBUG_ASYNC("Deserialized node ID %d at index %d", idItem->valueint, index - 1);
		// Remplissage des arêtes
		cJSON *edgesArray = cJSON_GetObjectItem(nodeJson, "edges");
		cJSON *edgeJson = NULL;
		cJSON_ArrayForEach(edgeJson, edgesArray) {
			cJSON *targetItem = cJSON_GetObjectItem(edgeJson, "target");
			cJSON *weightItem = cJSON_GetObjectItem(edgeJson, "weight");
			cJSON *ruleItem = cJSON_GetObjectItem(edgeJson, "rule");

			if (cJSON_IsNumber(targetItem) && cJSON_IsNumber(weightItem)) {
				graph_add_edge(msg->map, idItem->valueint,targetItem->valueint,(int)weightItem->valuedouble,(lane_rule_t)ruleItem->valueint);
			}
		}
	}
	
	return 0;
}


