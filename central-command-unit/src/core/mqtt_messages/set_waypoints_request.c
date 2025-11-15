/**
 * @file set_waypoints_request.c
 * @brief Définitions du modèle de données pour les requêtes de définition de waypoints.
 * @author Lukas Grando
 * @date 2025-10-27
 */
#include "core/mqtt_messages/set_waypoints_request.h"
#include "core/action_codes.h"

/**
 * @brief Sérialise un message de demande de définition de waypoints en JSON.
 * @param msg Pointeur vers le message de demande de définition de waypoints à sérialiser.
 * @return Chaîne JSON représentant le message de demande de définition de waypoints
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *set_waypoints_request_serialize(const set_waypoints_request_t *msg) {
	if (!msg) return NULL;

	cJSON *root = NULL;
	char *jsonString = NULL;

	root = cJSON_CreateObject();
	if (!root) return NULL;

	if (command_header_serialize(&msg->header, root) != 0) goto cleanup;
	if (!cJSON_AddNumberToObject(root, "carId", msg->carId)) goto cleanup;

	cJSON *waypointArray = cJSON_CreateArray();
	if (!waypointArray) goto cleanup;

	if (!cJSON_AddItemToObject(root, "waypoints", waypointArray)) goto cleanup;

	for(int i = 0; i < msg->waypointCount; i++) {
		cJSON *waypointObj = cJSON_CreateObject();
		if (!waypointObj) goto cleanup;

		if (!cJSON_AddNumberToObject(waypointObj, "nodeId", msg->waypoints[i].nodeId)) {
			cJSON_Delete(waypointObj); 
			goto cleanup;
		}
		if (!cJSON_AddNumberToObject(waypointObj, "laneRule", msg->waypoints[i].laneRule)) {
			cJSON_Delete(waypointObj);
			goto cleanup;
		}
		if (!cJSON_AddNumberToObject(waypointObj, "type", msg->waypoints[i].type)) {
			cJSON_Delete(waypointObj);
			goto cleanup;
		}
		if (!cJSON_AddNumberToObject(waypointObj, "x", msg->waypoints[i].x)) {
			cJSON_Delete(waypointObj);
			goto cleanup;
		}
		if (!cJSON_AddNumberToObject(waypointObj, "y", msg->waypoints[i].y)) {
			cJSON_Delete(waypointObj);
			goto cleanup;
		}

		if (!cJSON_AddItemToArray(waypointArray, waypointObj)) {
			cJSON_Delete(waypointObj);
			goto cleanup;
		}
	}

	jsonString = CJSON_PRINT(root);
	if (!jsonString) goto cleanup; 

	cJSON_Delete(root);
	return jsonString;

	cleanup:
		if (root) cJSON_Delete(root);
		return NULL;
}

/**
 * @brief Désérialise un message de demande de définition de waypoints à partir d'une chaîne JSON.
 * @param root Pointeur vers l'objet cJSON représentant le message de demande de définition de waypoints.
 * @param msg Pointeur vers la structure de message de demande de définition de waypoints à remplir.
 * @warning Cette fonction s'occupe uniquement de désérialiser les données. Le header doit être désérialisé séparément.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int set_waypoints_request_data_deserialize(cJSON *root, set_waypoints_request_t *msg) {
	if (!root || !msg) {
		return -1;
	}

	const cJSON *carIdItem = cJSON_GetObjectItemCaseSensitive(root, "carId");
	if (!cJSON_IsNumber(carIdItem)) return -1;

	msg->carId = carIdItem->valueint;

	const cJSON *waypointArray = cJSON_GetObjectItemCaseSensitive(root, "waypoints");
	if (!cJSON_IsArray(waypointArray)) return -1;

	msg->waypointCount = cJSON_GetArraySize(waypointArray);
	if (msg->waypointCount == 0) {
		msg->waypoints = NULL;
		return 0;
	}

	msg->waypoints = (waypoint_t*)malloc(sizeof(waypoint_t) * msg->waypointCount);
	if (!msg->waypoints) return -1;

	for (int i = 0; i < msg->waypointCount; i++) {
		cJSON *waypointObj = cJSON_GetArrayItem(waypointArray, i);
		if (!cJSON_IsObject(waypointObj)) goto error_cleanup;

		const cJSON *nodeIdItem = cJSON_GetObjectItemCaseSensitive(waypointObj, "nodeId");
		const cJSON *laneRuleItem = cJSON_GetObjectItemCaseSensitive(waypointObj, "laneRule");
		const cJSON *typeItem = cJSON_GetObjectItemCaseSensitive(waypointObj, "type");
		const cJSON *xItem = cJSON_GetObjectItemCaseSensitive(waypointObj, "x");
		const cJSON *yItem = cJSON_GetObjectItemCaseSensitive(waypointObj, "y");

		if (!cJSON_IsNumber(nodeIdItem) || !cJSON_IsNumber(laneRuleItem) || !cJSON_IsNumber(typeItem)  || !cJSON_IsNumber(xItem) || !cJSON_IsNumber(yItem)) {
			goto error_cleanup;
		}

		msg->waypoints[i].nodeId = nodeIdItem->valueint;
		msg->waypoints[i].laneRule = (lane_rule_t)laneRuleItem->valueint;
		msg->waypoints[i].type = (node_type_t)typeItem->valueint;
		msg->waypoints[i].x = xItem->valuedouble;
		msg->waypoints[i].y = yItem->valuedouble;
	}

	return 0;

	error_cleanup:
		free(msg->waypoints);
		msg->waypoints = NULL;
		msg->waypointCount = 0;
		return -1;
}

/**
 * @brief Traduit un path_t (liste de nœuds) en un tableau d'instructions de waypoint.
 * @details Alloue le tableau d'instructions. L'appelant doit le libérer.
 * @param path Le chemin de nœuds retourné par Dijkstra.
 * @param waypoints Pointeur vers le tableau d'instructions qui sera alloué.
 * @param waypointCount Pointeur vers l'entier stockant la taille du tableau.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 * @internal
 */
int convert_path_to_waypoints(const path_t *path, waypoint_t **waypoints, int *waypointCount, graph_t *map) {
	int nodeCount = path->length - 1;
    if (nodeCount <= 0) {
        *waypoints = NULL;
        *waypointCount = 0;
        return 0; 
    }

    waypoint_t *newWaypoints = (waypoint_t*)malloc(sizeof(waypoint_t) * nodeCount);
    if (!newWaypoints) {
        *waypoints = NULL;
        *waypointCount = 0;
        return -1;
    }

    for (int i = 0; i < nodeCount; i++) {
        node_t* currentNode = path->nodes[i];
        node_t* nextNode = path->nodes[i + 1];

        edge_t* foundEdge = graph_get_edge(map, currentNode->id, nextNode->id);
        if (!foundEdge) {
            LOG_ERROR_SYNC("Path/Graph mismatch: No edge from node %d to %d.", currentNode->id, nextNode->id);
            free(newWaypoints);
            return -1;
        }

        newWaypoints[i].laneRule = foundEdge->drivingRule;
        newWaypoints[i].x = nextNode->x;
        newWaypoints[i].y = nextNode->y;
        newWaypoints[i].type = nextNode->type;
        newWaypoints[i].nodeId = nextNode->id;
    }

    *waypoints = newWaypoints;
    *waypointCount = nodeCount;
    return 0; // Succès
}

/**
 * @brief Libère la mémoire allouée pour une requête de définition de waypoints.
 * @param msg Pointeur vers la requête de définition de waypoints à libérer
 * @return void
 */
void set_waypoints_request_destroy(set_waypoints_request_t *msg) {
	if (!msg) return;

	if (msg->waypoints) {
		free(msg->waypoints);
		msg->waypoints = NULL;
	}
	msg->waypointCount = 0;
}