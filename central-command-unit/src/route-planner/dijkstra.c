/**
 * @file dijkstra.h
 * @brief Définitions de l'algorithme de Dijkstra pour le calcul des plus
 * courts chemins dans un graphe pondéré.
 * @date 2025-11-05
 */

 #include "route-planner/dijkstra.h"


/**
 * @brief Reconstruit le chemin (une fois la destination atteinte).
 * @details Remonte la chaîne des 'parent' depuis la fin.
 * @internal
 */
static path_t reconstruct_path(node_t* endNode, dijkstra_node_t* data) {
    path_t path = { .nodes = NULL, .length = 0 };
    node_t* current = endNode;
    int count = 0;

    while (current) {
        count++;
        current = data[current->id].previous;
    }

	// Aucun chemin trouvé
    if (count == 0) {
        return path;
    }

    // 2. Allouer le tableau de pointeurs de noeuds
    path.nodes = (node_t**)malloc(sizeof(node_t*) * count);
	if (path.nodes == NULL) {
		return ERROR_PATH;
	}

    path.length = count;
    current = endNode;
    for (int i = count - 1; i >= 0; i--) {
        path.nodes[i] = current;
        current = data[current->id].previous;
    }
    
    return path;
}

 /**
 * @brief Calcule le plus court chemin entre deux noeud dans un graphe pondéré orienté
 * 
 * @param graph Le graphe pondéré orienté
 * @param start Le noeud de départ
 * @param end Le noeud d'arrivée
 * @return La liste des noeuds représentant le plus court chemin, ou NULL si aucun chemin
 * @retval ERROR_PATH En cas d'erreur (noeuds invalides, etc.)
 * @warning Le chemin retourné doit être libéré avec path_destroy()
 * @see path_destroy()
 */
path_t dijkstra_find_path(graph_t *graph, node_t *start, node_t *end) {
	if(graph == NULL || start == NULL || end == NULL) {
		return ERROR_PATH;
	}

	dijkstra_node_t *data = (dijkstra_node_t *) calloc(graph->numNodes, sizeof(dijkstra_node_t));
	if(data == NULL) {
		return ERROR_PATH;
	}

	priority_queue_t *pq = pq_create(graph->numNodes);
	if(pq == NULL) {
		free(data);
		return ERROR_PATH;
	}

	for(int i = 0; i < graph->numNodes; i++) {
		data[i].gCost = DIJKSTRA_INFINITY;
	}
	data[start->index].gCost = 0.0;
	pq_push(pq, start, 0);

	while(!pq_is_empty(pq)) {
		node_t *current = (node_t *) pq_pop(pq);

		if(data[current->index].visited) {
			continue;
		}
		data[current->index].visited = true;

		if(current == end) {
			path_t path = reconstruct_path(end, data);
			pq_destroy(pq);
			free(data);
			return path;
		}

		edge_t *edge = current->edges;
		while(edge != NULL) {
			node_t *neighbor = edge->targetNode;

			if(!data[neighbor->index].visited) {
				double newCost = data[current->index].gCost + edge->weight;

				if(newCost < data[neighbor->index].gCost) {
					data[neighbor->index].gCost = newCost;
					data[neighbor->index].previous = current;
					pq_push(pq, neighbor, (int)newCost);
				}
			}
			edge = edge->nextEdge;
		}
	}

	pq_destroy(pq);
	free(data);
	return (path_t) { .nodes = NULL, .length = 0 };
}