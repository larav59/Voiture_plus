/**
 * @file test_graph.c
 * @brief Tests unitaires pour les fonctions de base du graphe (création, nœuds, arêtes).
 */

#include "tests/runner.h"
#include "core/graph.h"

TEST_REGISTER(test_graph_api_basic, "Test de l'API de base du graphe (création, nœuds, arêtes)") {
    // 1. Tester la création
    graph_t* g = graph_create(5);
    TEST_ASSERT(g != NULL, "La création du graphe ne doit pas échouer");
    TEST_ASSERT(g->numNodes == 5, "Le graphe doit avoir 5 nœuds");
    TEST_ASSERT(g->nodes != NULL, "Le tableau de nœuds doit être alloué");

    // 2. Tester l'initialisation d'un nœud
    graph_init_node(g, 2, 12.5, 7.5, NODE_TYPE_ROUNDABOUT);
    node_t* n2 = graph_get_node(g, 2);
    
    TEST_ASSERT(n2 != NULL, "Le nœud 2 doit être récupérable");
    TEST_ASSERT(n2->id == 2, "L'ID du nœud 2 doit être 2 (auto-assigné par create)");
    TEST_ASSERT(n2->x == 12.5, "La coordonnée X doit être 12.5");
    TEST_ASSERT(n2->type == NODE_TYPE_ROUNDABOUT, "Le type du nœud doit être ROUNDABOUT");
    TEST_ASSERT(n2->edges == NULL, "Le nœud 2 ne doit avoir aucune arête sortante pour l'instant");

    // 3. Tester l'ajout d'une arête
    bool added = graph_add_edge(g, 2, 4, 1.5, LANE_RULE_DRIVE_RIGHT);
    TEST_ASSERT(added == true, "L'ajout de l'arête doit réussir");
    TEST_ASSERT(n2->edges != NULL, "Le nœud 2 doit maintenant avoir une arête sortante");
    TEST_ASSERT(n2->edges->targetNode == graph_get_node(g, 4), "L'arête doit pointer vers le nœud 4");
    TEST_ASSERT(n2->edges->weight == 1.5, "Le poids de l'arête doit être 1.5");
    TEST_ASSERT(n2->edges->drivingRule == LANE_RULE_DRIVE_RIGHT, "La règle de conduite est incorrecte");
    TEST_ASSERT(n2->edges->nextEdge == NULL, "Il ne doit y avoir qu'une seule arête");

    // 4. Tester l'ajout d'une deuxième arête au MÊME nœud (liste chaînée)
    bool added2 = graph_add_edge(g, 2, 1, 5.0, LANE_RULE_DRIVE_LEFT);
    TEST_ASSERT(added2 == true, "L'ajout de la deuxième arête doit réussir");
    TEST_ASSERT(n2->edges->targetNode == graph_get_node(g, 1), "La nouvelle arête (vers 1) doit être en tête de liste");
    TEST_ASSERT(n2->edges->nextEdge != NULL, "La première arête doit être la suivante dans la liste");
    TEST_ASSERT(n2->edges->nextEdge->targetNode == graph_get_node(g, 4), "La deuxième arête (vers 4) doit être en seconde position");

    // 5. Tester les cas d'erreur
    node_t* n_invalid = graph_get_node(g, 99);
    TEST_ASSERT(n_invalid == NULL, "Récupérer un nœud hors limites doit retourner NULL");
    
    bool added_invalid_target = graph_add_edge(g, 2, 99, 1.0, LANE_RULE_DRIVE_RIGHT);
    TEST_ASSERT(added_invalid_target == false, "Ajouter une arête vers un nœud cible inexistant doit échouer");
    
    bool added_invalid_origin = graph_add_edge(g, 99, 1, 1.0, LANE_RULE_DRIVE_RIGHT);
    TEST_ASSERT(added_invalid_origin == false, "Ajouter une arête depuis un nœud origine inexistant doit échouer");

    // 6. Tester la destruction
    graph_destroy(g);
    // Note : On ne peut pas ASSERT la destruction, mais valgrind confirmera qu'il n'y a pas de fuite.
}