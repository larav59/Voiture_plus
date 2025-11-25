/**
 * @file route_planner_message_callback.c
 * @brief Implémentation de la logique métier du route-planner.
 */
#include "route-planner/route_planner_message_callback.h"

// Variables globales
static graph_t* g_map = NULL;
static bool g_safeMode = false;
static bool g_railwayMode = false;

/**
 * @brief Initialise le callback du route planner avec la carte et les modes par défaut.
 * @param map Pointeur vers la carte du graphe.
 */
void route_planner_callback_init(graph_t* map) {
	g_map = map;
	g_safeMode = false;
	g_railwayMode = false;
}

/**
 * @brief Nettoie les ressources utilisées par le route planner.
 * Actuellement, cette fonction réinitialise simplement les variables globales.
 */
void route_planner_cleanup(void) {
	g_map = NULL;
	g_safeMode = false;
	g_railwayMode = false;

}


// Handlers pour les différentes commandes
static void on_set_safe_route_mode(const set_safe_route_mode_request_t* request) {
	g_safeMode = request->enabled;
	LOG_INFO_ASYNC("Safe Route Mode set to %s", g_safeMode ? "ENABLED" : "DISABLED");

	// todo: Mettre à jour les routes en conséquence ici :
	// - Changer les itinéraires en cours si nécessaire
	// - Informer les véhicules concernés
	// - Mettre à jour la carte pour supprimer les noeuds avec zones de conflit
}

static void on_set_railway_mode(const set_railway_mode_request_t* request) {
	g_railwayMode = request->enabled;
	LOG_INFO_ASYNC("Railway Mode set to %s", g_railwayMode ? "ENABLED" : "DISABLED");

	
	// todo: Mettre à jour les routes en conséquence ici :
	// - Supprimer le noeud de la carte avec le passage à niveau si le mode est activé
	// - Recalculer les itinéraires en cours si nécessaire
	// - Informer les véhicules concernés
}

static void on_plan_route_request(const plan_route_request_t* request) {
	LOG_DEBUG_ASYNC("Received PLAN_ROUTE_REQUEST for carId %d with %d nodes", request->carId, request->nodeCount);

	// Vérification de la carte
	if(!g_map) {
		// Création d'une réponse d'erreur ici
		command_response_header_t response = create_command_response_header(request->header.commandId, false, "Map not initialized");
		char *jsonResponse = command_response_header_serialize(&response);
		if(jsonResponse) {
			mqtt_publish(request->header.replyTopic, jsonResponse, MQTT_QOS_AT_MOST_ONCE, false);
			free(jsonResponse);
		}
		else LOG_ERROR_ASYNC("Failed to serialize error response for PLAN_ROUTE_REQUEST");
		return;
	}

	path_t totalPath = EMPTY_PATH;
	for(int i = 0; i < request->nodeCount - 1; i++) {
		int startNodeId = request->nodeIds[i];
		int endNodeId = request->nodeIds[i + 1];

		node_t *startNode = graph_get_node_by_id(g_map, startNodeId);
		node_t *endNode = graph_get_node_by_id(g_map, endNodeId);
		if(!startNode || !endNode) {
			LOG_ERROR_ASYNC("Invalid node IDs in plan route request: %d to %d", startNodeId, endNodeId);
			command_response_header_t response = create_command_response_header(request->header.commandId, false, "Invalid node IDs in request");
			char *jsonResponse = command_response_header_serialize(&response);
			if(jsonResponse) {
				mqtt_publish(request->header.replyTopic, jsonResponse, MQTT_QOS_AT_MOST_ONCE, false);
				free(jsonResponse);
			}
			else LOG_ERROR_ASYNC("Failed to serialize error response for PLAN_ROUTE_REQUEST with invalid node IDs");
			path_destroy(&totalPath);
			return;
		}

		path_t segment = dijkstra_find_path(g_map, startNode, endNode);
		if(segment.length == 0) {
			LOG_ERROR_ASYNC("No path found from node %d to node %d", startNodeId, endNodeId);
			command_response_header_t response = create_command_response_header(request->header.commandId, false, "No path found between specified nodes");
			char *jsonResponse = command_response_header_serialize(&response);
			if(jsonResponse) {
				mqtt_publish(request->header.replyTopic, jsonResponse, MQTT_QOS_AT_MOST_ONCE, false);
				free(jsonResponse);
			} else LOG_ERROR_ASYNC("Failed to serialize error response for PLAN_ROUTE_REQUEST with no path found");
		}

		path_append(&totalPath, &segment);
		path_destroy(&segment);
		LOG_DEBUG_ASYNC("Planned segment from node %d to node %d, segment length: %d", startNodeId, endNodeId, segment.length);
	}

	char carTopic[REPLY_TOPIC_LENGTH];
	snprintf(carTopic, sizeof(carTopic), "vehicles/%d/request", request->carId);
	set_waypoints_request_t waypointRequest = {
		.header = create_command_header(ACTION_SET_WAYPOINTS_REQUEST, carTopic),
		.carId = request->carId,
		.waypoints = NULL,
		.waypointCount = 0
	};
	if(convert_path_to_waypoints(&totalPath, &waypointRequest.waypoints, &waypointRequest.waypointCount, g_map) != 0) {
		LOG_ERROR_ASYNC("Failed to convert path to waypoints for carId %d", request->carId);
		command_response_header_t response = create_command_response_header(request->header.commandId, false, "Failed to convert path to waypoints");
		char *jsonResponse = command_response_header_serialize(&response);
		if(jsonResponse) {
			mqtt_publish(request->header.replyTopic, jsonResponse, MQTT_QOS_AT_MOST_ONCE, false);
			free(jsonResponse);
		} else LOG_ERROR_ASYNC("Failed to serialize error response for PLAN_ROUTE_REQUEST waypoint conversion failure");
		path_destroy(&totalPath);
		return;
	}

	char *jsonPayload = set_waypoints_request_serialize(&waypointRequest);
	if(!jsonPayload) {
		LOG_ERROR_ASYNC("Could not serialize set_waypoints_request message to JSON for carId %d", request->carId);
	} else {
		mqtt_publish(carTopic, jsonPayload, MQTT_QOS_EXACTLY_ONCE, false);
		free(jsonPayload);
		LOG_DEBUG_ASYNC("Planned route for carId %d with %d waypoints", request->carId, waypointRequest.waypointCount);
	}

	plan_route_response_t response = {
		.header = create_command_response_header(request->header.commandId, true, NULL),
		.nodeIds = NULL,
		.nodeCount = 0,
		.carId = request->carId
	};

    int pathNodeIds[totalPath.length];    
    for(int i = 0; i < totalPath.length; i++) {
        pathNodeIds[i] = totalPath.nodes[i]->id;
    }
    response.nodeIds = pathNodeIds;
    response.nodeCount = totalPath.length;

	char *jsonResponse = plan_route_response_serialize(&response);
	if(jsonResponse) {
		mqtt_publish(request->header.replyTopic, jsonResponse, MQTT_QOS_AT_MOST_ONCE, false);
		free(jsonResponse);
	} else {
		LOG_ERROR_ASYNC("Failed to serialize success response for PLAN_ROUTE_REQUEST for carId %d", request->carId);
	}

	set_waypoints_request_destroy(&waypointRequest);
	path_destroy(&totalPath);
}

void on_get_map_response(const cJSON *root, const command_response_header_t *header, void *context) {
	UNUSED(context);

	get_map_response_t mapResponse = {
		.header = *header,
		.map = NULL
	};

	if(get_map_response_data_deserialize((cJSON *)root, &mapResponse) != 0) {
		LOG_ERROR_ASYNC("Failed to deserialize get map response.");
		return;
	}

	if(g_map) graph_destroy(g_map);
	
	g_map = mapResponse.map;
	LOG_INFO_ASYNC("Map received with %d nodes.", g_map->numNodes);
}

void route_planner_message_callback(const char* topic, const char* payload) {
	LOG_DEBUG_SYNC("Received message on topic: %s", topic);

	// Gestion des réponses à nos requêtes
	if (strcmp(topic, "services/route-planner/response") == 0) {
		request_manager_process_response(payload);
		return;
	}

	// Gestion des commandes reçues
	if(strcmp(topic, "services/route-planner/request") == 0) {
		command_header_t header = {0};
		cJSON *root = cJSON_Parse(payload);
		if (!root) {
			LOG_ERROR_ASYNC("The payload is not valid JSON.");
			return;
		}
		if(command_header_deserialize(root, &header) != 0) {
			LOG_ERROR_ASYNC("Failed to deserialize command header.");
			cJSON_Delete(root);
			return;
		}
		LOG_DEBUG_ASYNC("Processing command: %s, action: %s, replyTopic: %s", header.commandId, header.action, header.replyTopic);

		if(strcmp(header.action, ACTION_SET_SAFE_ROUTE_MODE) == 0) {
			set_safe_route_mode_request_t request = { .header = header };
			if(set_safe_route_mode_request_data_deserialize(root, &request) != 0) {
				LOG_ERROR_ASYNC("Failed to deserialize set safe route mode request.");
				return;
			}
			else on_set_safe_route_mode(&request);
			
		} else if(strcmp(header.action, ACTION_SET_RAILWAY_MODE) == 0) {
			set_railway_mode_request_t request = { .header = header };
			if(set_railway_mode_request_data_deserialize(root, &request) != 0) {
				LOG_ERROR_ASYNC("Failed to deserialize set railway mode request.");
				return;
			}
			else on_set_railway_mode(&request);

		} else if(strcmp(header.action, ACTION_PLAN_ROUTE_REQUEST) == 0) {
			plan_route_request_t request = {0};
			request.header = header;
			
			if(plan_route_request_data_deserialize(root, &request) != 0) {
				LOG_ERROR_ASYNC("Failed to deserialize plan route request.");
				return;
			}
			else on_plan_route_request(&request);
		}

		cJSON_Delete(root);
		return;
	}
}