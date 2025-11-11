/**
 * @file test_dijkstra.c
 * @brief Tests unitaires pour l'algorithme de Dijkstra.
 * @details Teste la recherche de chemin sur différents types de graphes.
 */

#include "tests/runner.h"
#include "core/graph.h"
#include "route-planner/dijkstra.h"

/**
 * @brief Crée un graphe de test complexe pour le choix optimal.
 * @details
 * Graphe (4 nœuds: 0, 1, 2, 3):
 *
 * (10)
 * (0)------> (1)
 * | \        |
 * (2)|  \ (8)   | (10)
 * |    \     |
 * v     \    v
 * (3)------> (2)
 * (3)
 *
 * Chemins de 0 à 2:
 * - 0 -> 1 -> 2: coût 10 + 10 = 20
 * - 0 -> 2:      coût 8
 * - 0 -> 3 -> 2: coût 2 + 3 = 5 (Encore plus optimal)
 *
 * Dijkstra doit trouver 0 -> 3 -> 2.
 */
static graph_t* create_optimal_test_graph() {
    graph_t* g = graph_create(4); // 4 nœuds
    
    // Initialiser les nœuds (x, y, type n'importent pas pour Dijkstra)
    graph_init_node(g, 0, 0, 0, NODE_TYPE_INTERSECTION);
    graph_init_node(g, 1, 0, 0, NODE_TYPE_INTERSECTION);
    graph_init_node(g, 2, 0, 0, NODE_TYPE_INTERSECTION);
    graph_init_node(g, 3, 0, 0, NODE_TYPE_INTERSECTION);

    // Ajouter les arêtes (int weight)
    graph_add_edge(g, 0, 1, 10, LANE_RULE_DRIVE_RIGHT);
    graph_add_edge(g, 1, 2, 10, LANE_RULE_DRIVE_RIGHT);
    graph_add_edge(g, 0, 2, 8, LANE_RULE_DRIVE_RIGHT); // Raccourci 1
    graph_add_edge(g, 0, 3, 2, LANE_RULE_DRIVE_RIGHT); // Raccourci 2 (début)
    graph_add_edge(g, 3, 2, 3, LANE_RULE_DRIVE_RIGHT); // Raccourci 2 (fin)

    return g;
}


// Test 1: Un chemin simple et linéaire (A->B->C)
TEST_REGISTER(test_dijkstra_simple_path, "Test Dijkstra : chemin linéaire simple (A->B->C)") {
    graph_t* g = graph_create(3); // 0=A, 1=B, 2=C
    graph_init_node(g, 0, 0, 0, NODE_TYPE_WAYPOINT);
    graph_init_node(g, 1, 1, 0, NODE_TYPE_WAYPOINT);
    graph_init_node(g, 2, 2, 0, NODE_TYPE_WAYPOINT);
    graph_add_edge(g, 0, 1, 1, LANE_RULE_DRIVE_RIGHT); // A->B (coût 1)
    graph_add_edge(g, 1, 2, 1, LANE_RULE_DRIVE_RIGHT); // B->C (coût 1)

    node_t* start = graph_get_node(g, 0);
    node_t* end = graph_get_node(g, 2);
    
    path_t path = dijkstra_find_path(g, start, end);

    TEST_ASSERT(path.length == 3, "Le chemin doit contenir 3 nœuds (A, B, C)");
    if (path.length == 3) {
        TEST_ASSERT(path.nodes[0]->id == 0, "Le chemin doit commencer par le nœud 0");
        TEST_ASSERT(path.nodes[1]->id == 1, "Le chemin doit passer par le nœud 1");
        TEST_ASSERT(path.nodes[2]->id == 2, "Le chemin doit finir par le nœud 2");
    }

    path_destroy(&path);
    graph_destroy(g);
}

// Test 2: Le choix du chemin optimal (pas le plus direct)
TEST_REGISTER(test_dijkstra_optimal_choice, "Test Dijkstra : choix du chemin optimal (pas le plus direct)") {
    // Utilise le graphe de test (0->2 coûte 8, 0->3->2 coûte 5)
    graph_t* g = create_optimal_test_graph();
    
    node_t* start = graph_get_node(g, 0);
    node_t* end = graph_get_node(g, 2);
    
    path_t path = dijkstra_find_path(g, start, end);

    TEST_ASSERT(path.length == 3, "Le chemin optimal doit contenir 3 nœuds (0, 3, 2)");
    if (path.length == 3) {
        TEST_ASSERT(path.nodes[0]->id == 0, "Chemin optimal : début [0]");
        TEST_ASSERT(path.nodes[1]->id == 3, "Chemin optimal : milieu [3]");
        TEST_ASSERT(path.nodes[2]->id == 2, "Chemin optimal : fin [2]");
    }

    path_destroy(&path);
    graph_destroy(g);
}

// Test 3: Un chemin impossible (graphe non connecté)
TEST_REGISTER(test_dijkstra_no_path_found, "Test Dijkstra : aucun chemin trouvé (graphe non connecté)") {
    graph_t* g = graph_create(2); // 0=A, 1=B
    graph_init_node(g, 0, 0, 0, NODE_TYPE_WAYPOINT);
    graph_init_node(g, 1, 1, 0, NODE_TYPE_WAYPOINT);
    // Aucune arête de 0 à 1

    node_t* start = graph_get_node(g, 0);
    node_t* end = graph_get_node(g, 1);
    
    path_t path = dijkstra_find_path(g, start, end);

    TEST_ASSERT(path.length == 0, "La longueur du chemin doit être 0");
    TEST_ASSERT(path.nodes == NULL, "Le tableau de nœuds doit être NULL");

    path_destroy(&path);
    graph_destroy(g);
}

// Test 4: Le graphe est bien orienté (sens unique)
TEST_REGISTER(test_dijkstra_oriented_graph, "Test Dijkstra : le graphe est bien orienté (A->B != B->A)") {
    graph_t* g = graph_create(2); // 0=A, 1=B
    graph_init_node(g, 0, 0, 0, NODE_TYPE_WAYPOINT);
    graph_init_node(g, 1, 1, 0, NODE_TYPE_WAYPOINT);
    graph_add_edge(g, 0, 1, 1, LANE_RULE_DRIVE_RIGHT); // A->B seulement

    // Tenter de trouver B -> A
    node_t* start = graph_get_node(g, 1);
    node_t* end = graph_get_node(g, 0);
    
    path_t path = dijkstra_find_path(g, start, end);

    TEST_ASSERT(path.length == 0, "Aucun chemin ne doit être trouvé de B vers A");

    path_destroy(&path);
    graph_destroy(g);
}

// Test 5: Le point de départ est le même que le point d'arrivée
TEST_REGISTER(test_dijkstra_start_equals_end, "Test Dijkstra : nœud de départ == nœud d'arrivée") {
    graph_t* g = create_optimal_test_graph(); // Utiliser un graphe non trivial
    
    node_t* start = graph_get_node(g, 1);
    
    path_t path = dijkstra_find_path(g, start, start); // Chemin de 1 à 1

    TEST_ASSERT(path.length == 1, "Le chemin doit avoir une longueur de 1");
    if (path.length == 1) {
        TEST_ASSERT(path.nodes[0]->id == 1, "Le chemin doit contenir uniquement le nœud de départ");
    }

    path_destroy(&path);
    graph_destroy(g);
}