/**
 * @file dijkstra.h
 * @brief Définitions de l'algorithme de Dijkstra pour le calcul des plus
 * courts chemins dans un graphe pondéré.
 * @date 2025-11-05
 */

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "core/common.h"
#include "core/priority_queue.h"
#include "core/graph.h"
#include "core/check.h"


#define DIJKSTRA_INFINITY (double) INFINITY

#define ERROR_PATH (path_t) { .nodes = NULL, .length = -1 }

typedef struct {
	double gCost; // Cout du départ jusqu'au noeud actuel
	node_t *previous; // Noeud précédent dans le chemin
	bool visited; // Indique si le noeud a été visité
} dijkstra_node_t;

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
path_t dijkstra_find_path(graph_t *graph, node_t *start, node_t *end);


#endif // DIJKSTRA_H