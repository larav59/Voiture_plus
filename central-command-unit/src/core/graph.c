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
	if(originNodeId < 0 || originNodeId >= graph->numNodes ||
	   targetNodeId < 0 || targetNodeId >= graph->numNodes) {
		return false;
	}

	node_t* originNode = &graph->nodes[originNodeId];
	node_t* targetNode = &graph->nodes[targetNodeId];

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
 * @brief Récupère un pointeur vers un noeud par son ID.
 * @details Fonction utilitaire rapide (accès O(1) grâce à l'index).
 * @param graph Le graphe.
 * @param nodeId L'ID du noeud.
 * @return Pointeur vers le node_t, ou NULL si l'ID est invalide.
 */
node_t* graph_get_node(graph_t* graph, int nodeId) {
	if(nodeId < 0 || nodeId >= graph->numNodes) {
		return NULL;
	}
	return &graph->nodes[nodeId];
}

/**
 * @brief Libère la mémoire d'un chemin retourné par Dijkstra.
 */
void path_destroy(path_t* path) {
	if (!path) return;
	free(path->nodes);
}