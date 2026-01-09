/**
 * @file graph.c
 * @brief Définitions des structures de données pour le graphe de la carte.
 */
#include "core/graph.h"

/**
 * @brief Crée et alloue un nouveau graphe avec un nombre fixe de noeuds.
 * @details Alloue le graphe et le tableau de noeuds. Les noeuds sont initialisés
 * avec leur ID correspondant à leur index.
 * @param numNodes Le nombre total de noeuds que ce graphe contiendra.
 * @return Pointeur vers le graphe alloué, ou NULL si échec.
 */
graph_t* graph_create(int numNodes) {
	graph_t *graph = (graph_t *)malloc(sizeof(graph_t));
	if (!graph) {
		return NULL;
	}
	graph->nodes = (node_t *)malloc(sizeof(node_t) * numNodes);
	
	if (!graph->nodes) {
		free(graph);
		return NULL;
	}

	graph->numNodes = numNodes;

	// Initialisation des noeuds
	for (int i = 0; i < numNodes; i++) {
		graph->nodes[i].id = i;
		graph->nodes[i].x = 0.0;
		graph->nodes[i].y = 0.0;
		graph->nodes[i].type = NODE_TYPE_WAYPOINT;
		graph->nodes[i].edges = NULL;
		graph->nodes[i].index = i;
	}

	return graph;
}

/**
 * @brief Détruit le graphe et libère toute la mémoire associée.
 * @details Libère le graphe, le tableau de noeuds, et toutes les arêtes.
 * @param graph Le graphe à détruire.
 */
void graph_destroy(graph_t* graph) {
	if (!graph) return;

	// Libération des arêtes
	for (int i = 0; i < graph->numNodes; i++) {
		edge_t* edge = graph->nodes[i].edges;
		while (edge) {
			edge_t* nextEdge = edge->nextEdge;
			free(edge);
			edge = nextEdge;
		}
	}
	
	// Libération des noeuds et du graphe
	free(graph->nodes);
	free(graph);
}

/**
 * @brief Initialise un noeud dans le graphe.
 * @param graph Le graphe.
 * @param nodeId L'ID (et index) du noeud à initialiser.
 * @param x Coordonnée X.
 * @param y Coordonnée Y.
 * @param type Type de noeud.
 */
void graph_init_node(graph_t* graph, int nodeId, double x, double y, node_type_t type) {
	if(nodeId < 0 || nodeId >= graph->numNodes) {
		return;
	}
	graph->nodes[nodeId].x = x;
	graph->nodes[nodeId].y = y;
	graph->nodes[nodeId].type = type;
}

/**
 * @brief Ajoute une arête (arc) orientée au graphe.
 * @details Crée une arête allant de 'originNodeId' vers 'targetNodeId'.
 * @param graph Le graphe.
 * @param originNodeId ID du noeud d'origine.
 * @param targetNodeId ID du noeud de destination.
 * @param weight Coût (poids) de l'arête.
 * @param rule Règle de conduite sur l'arête.
 * @return true si succès, false en cas d'erreur (ex: ID de noeud invalide).
 */
bool graph_add_edge(graph_t* graph, int originNodeId, int targetNodeId, double weight, lane_rule_t rule) {
	node_t* originNode = graph_get_node_by_id(graph, originNodeId);
	node_t* targetNode = graph_get_node_by_id(graph, targetNodeId);

	if (!originNode || !targetNode) {
		return false;
	}

	edge_t* newEdge = (edge_t*)malloc(sizeof(edge_t));
	if (!newEdge) {
		return false;
	}
	newEdge->targetNode = targetNode;
	newEdge->weight = weight;
	newEdge->drivingRule = rule;
	newEdge->nextEdge = originNode->edges;
	originNode->edges = newEdge;

	return true;
}

/**
 * @brief Récupère un pointeur vers un noeud par son index.
 * @param graph Le graphe.
 * @param index L'index du noeud.
 * @return Pointeur vers le node_t, ou NULL si l'index est invalide.
 */
node_t* graph_get_node(graph_t* graph, int index) {
	if(index < 0 || index >= graph->numNodes) {
		return NULL;
	}
	return &graph->nodes[index];
}

/**
 * @brief Récupère un pointeur vers un noeud par son ID.
 * @details Parcourt le tableau des noeuds pour trouver celui avec l'ID donné.
 * @param graph Le graphe.
 * @param nodeId L'ID du noeud à rechercher.
 * @return Pointeur vers le node_t, ou NULL si l'ID n'existe pas.
 */
node_t *graph_get_node_by_id(graph_t *graph, int nodeId) {
	for (int i = 0; i < graph->numNodes; i++) {
		if (graph->nodes[i].id == nodeId) {
			return &graph->nodes[i];
		}
	}
	return NULL;
}

/**
 * @brief Libère la mémoire d'un chemin
 */
void path_destroy(path_t* path) {
	if (!path) return;
	free(path->nodes);
}

/**
 * @brief Concatène un chemin (src) à la fin d'un chemin (dest).
 * @details Modifie dest en place en réallouant son tableau de nœuds.
 * Si le dernier nœud de dest est le même que le premier de src,
 * le doublon est automatiquement supprimé.
 * @param dest Le chemin à étendre (sera modifié).
 * @param src Le chemin à ajouter à la fin.
 * @note L'appelant est toujours responsable de détruire src (via path_destroy)
 */
void path_append(path_t* dest, const path_t* src) {
	if(!dest || !src || src->length == 0) {
		return;
	}

	int startIndex = 0;
	int nodesToAdd = src->length;

	// Gestion du doublon potentiel
	if(dest->length > 0 && dest->nodes[dest->length - 1] == src->nodes[0]) {
		startIndex = 1;
		nodesToAdd -= 1;
	}

	if(nodesToAdd <= 0) {
		return; 
	}

	int newLength = dest->length + nodesToAdd;
	node_t** newNodes = (node_t**)realloc(dest->nodes, sizeof(node_t*) * newLength);
	if(!newNodes) {
		return;
	}
	dest->nodes = newNodes;
	memcpy(dest->nodes + dest->length, src->nodes + startIndex,sizeof(node_t*) * nodesToAdd);
	dest->length = newLength;
}

/**
 * @brief Trouve l'arête (orientée) entre deux nœuds.
 * @param graph Le graphe.
 * @param originNodeId ID du nœud d'origine.
 * @param targetNodeId ID du nœud de destination.
 * @return Pointeur vers l'edge_t, ou NULL si non trouvée.
 */
edge_t* graph_get_edge(graph_t* graph, int originNodeId, int targetNodeId) {
    node_t* originNode = graph_get_node(graph, originNodeId);
    if (!originNode) {
        return NULL;
    }

    edge_t* edge = originNode->edges;
    while (edge) {
        if (edge->targetNode->id == targetNodeId) {
            return edge; // Trouvé !
        }
        edge = edge->nextEdge;
    }

    return NULL; // Pas d'arête directe
}