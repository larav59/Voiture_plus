/**
 * @file plan_route_response.c
 * @brief Définitions du modèle de données pour les réponses de planification de trajet.
 * @author Lukas Grando
 * @date 2025-11-25
 */

#include "core/mqtt_messages/plan_route_response.h"

/**
 * @brief Sérialise une réponse de planification de trajet en JSON.
 * @param msg Pointeur vers la structure à sérialiser.
 * @return Chaîne JSON allouée (à libérer par l'appelant), ou NULL en cas d'erreur.
 */
char *plan_route_response_serialize(const plan_route_response_t *msg) {
    if (!msg) return NULL;

    cJSON *root = cJSON_CreateObject();
    if (!root) return NULL;

    if (command_response_header_to_json(&msg->header, root) != 0) goto error;

    cJSON_AddNumberToObject(root, "carId", msg->carId);

    if (msg->header.success && msg->nodeCount > 0 && msg->nodeIds) {
        cJSON *nodesArray = cJSON_CreateIntArray(msg->nodeIds, msg->nodeCount);
        if (!nodesArray) goto error;
        
        cJSON_AddItemToObject(root, "nodeIds", nodesArray);
    }

    char *jsonString = CJSON_PRINT(root);
    cJSON_Delete(root);
    return jsonString;

	error:
		cJSON_Delete(root);
		return NULL;
}

/**
 * @brief Désérialise les données spécifiques (la liste des nœuds) d'une réponse.
 * @details Alloue la mémoire pour msg->nodeIds.
 * @param root L'objet cJSON racine (déjà parsé).
 * @param msg Pointeur vers la structure à remplir.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int plan_route_response_data_deserialize(const cJSON *root, plan_route_response_t *msg) {
    if (!root || !msg) return -1;

    const cJSON *nodesArray = cJSON_GetObjectItemCaseSensitive(root, "nodeIds");
	const cJSON *carIdItem = cJSON_GetObjectItemCaseSensitive(root, "carId");

	if (msg->header.success) {
		if (cJSON_IsNumber(carIdItem)) {
			msg->carId = carIdItem->valueint;
		} else {
			return -1;
		}
	} else {
		// For error responses, carId may be absent; do not set msg->carId
	}
    
    if (!cJSON_IsArray(nodesArray)) {
        msg->nodeCount = 0;
        msg->nodeIds = NULL;
        return 0; 
    }
    msg->nodeCount = cJSON_GetArraySize(nodesArray);
    
    if (msg->nodeCount > 0) {
        msg->nodeIds = (int *)malloc(sizeof(int) * msg->nodeCount);
        if (!msg->nodeIds) return -1;

        int i = 0;
        cJSON *item = NULL;
        cJSON_ArrayForEach(item, nodesArray) {
            if (cJSON_IsNumber(item)) {
                msg->nodeIds[i] = item->valueint;
            } else {
                // Erreur de type dans le tableau
                free(msg->nodeIds);
                msg->nodeIds = NULL;
                msg->nodeCount = 0;
                return -1;
            }
            i++;
        }
    } else msg->nodeIds = NULL;

    return 0;
}