/**
 * @file graph.h
 * @brief Définitions des structures de données pour le graphe de la carte.
 * @date 2025-11-04
 */
#ifndef GRAPH_H
#define GRAPH_H

#include "core/common.h"
#include "core/check.h"

/**
 * @brief Type de noeud sur la carte.
 * @details Permet au véhicule de savoir comment se comporter à ce noeud.
 */
typedef enum {
    NODE_TYPE_INTERSECTION, //!< Noeud d'intersection classique
    NODE_TYPE_WAYPOINT, //!< Noeud de passage simple sur une route par exemple
    NODE_TYPE_ROUNDABOUT, //!< Noeud à l'entrée de rond-point
} node_type_t;

/**
 * @brief Indique le sens de circulation sur un arc.
 */
typedef enum {
    LANE_RULE_DRIVE_RIGHT, //!< Le véhicule doit circuler à droite de la position du noeud
    LANE_RULE_DRIVE_LEFT, //!< Le véhicule doit circuler à gauche de la position du noeud
    LANE_RULE_ONE_WAY //!< Le véhicule doit circuler à la position du noeud (sens unique)
} lane_rule_t;

typedef struct _Node node_t;

/**
 * @brief Un arc (un tronçon de route) reliant deux noeuds.
 * @details Ici on utilise une représentation du graphe par une liste d'adjacence.
 * Chaque noeud possède une liste chaînée d'arcs partant de ce noeud
 */
typedef struct _Edge {
    node_t* targetNode; //!< Le noeud vers lequel cet arc pointe
    double weight; //!< Le "coût" de cet arc
    struct _Edge* nextEdge; //!< Pointeur vers le prochain arc partant du MÊME noeud (liste chaînée)
    lane_rule_t drivingRule; //!< Règle de conduite (gauche/droit/central)

} edge_t;

/**
 * @brief Un noeud dans le graphe de la carte.
 * @details Représente un point d'intérêt sur la carte (intersection, rond-point, etc.)
 * Ces noeuds sont reliés entre eux par des arcs (edge_t) formant le graphe complet.
 */
struct _Node {
    int id; //!< Identifiant unique du noeud
    double x, y; //!< Coordonnées du noeud sur la carte
    node_type_t type; //!< Type de noeud (intersection, waypoint, rond-point, etc.)
    edge_t* edges; //!< Liste chaînée des arcs partant de ce noeud
};

/**
 * @brief Le graphe complet, représentant la carte.
 * @details Contient un tableau de tous les noeuds du graphe.
 */
typedef struct {
    node_t* nodes; //!< Tableau des noeuds du graphe
    int numNodes; //!< Nombre total de noeuds dans le tableau
} graph_t;


// TODO: à déplacer dans dijkstra.h ??

/**
 * @brief Structure pour retourner le chemin trouvé.
 */
typedef struct {
    node_t** nodes; // Tableau de pointeurs vers les noeuds, dans l'ordre
    int length;     // Nombre de noeuds dans le chemin
} path_t;


/**
 * @brief Crée et alloue un nouveau graphe avec un nombre fixe de noeuds.
 * @details Alloue le graphe et le tableau de noeuds. Les noeuds sont initialisés
 * avec leur ID correspondant à leur index.
 * @param numNodes Le nombre total de noeuds que ce graphe contiendra.
 * @return Pointeur vers le graphe alloué, ou NULL si échec.
 */
graph_t* graph_create(int numNodes);

/**
 * @brief Détruit le graphe et libère toute la mémoire associée.
 * @details Libère le graphe, le tableau de noeuds, et toutes les arêtes.
 * @param graph Le graphe à détruire.
 */
void graph_destroy(graph_t* graph);

/**
 * @brief Initialise un noeud dans le graphe.
 * @param graph Le graphe.
 * @param nodeId L'ID du noeud à initialiser.
 * @param x Coordonnée X.
 * @param y Coordonnée Y.
 * @param type Type de noeud.
 */
void graph_init_node(graph_t* graph, int nodeId, double x, double y, node_type_t type);

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
bool graph_add_edge(graph_t* graph, int originNodeId, int targetNodeId, double weight, lane_rule_t rule);

/**
 * @brief Récupère un pointeur vers un noeud par son ID.
 * @details Fonction utilitaire rapide (accès O(1) grâce à l'index).
 * @param graph Le graphe.
 * @param nodeId L'ID du noeud.
 * @return Pointeur vers le node_t, ou NULL si l'ID est invalide.
 */
node_t* graph_get_node(graph_t* graph, int nodeId);

/**
 * @brief Libère la mémoire d'un chemin retourné par Dijkstra.
 * @details (Tu avais déjà ce prototype, voici son implémentation).
 */
void path_destroy(path_t* path);



#endif // GRAPH_H