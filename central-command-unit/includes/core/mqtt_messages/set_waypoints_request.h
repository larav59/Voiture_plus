/**
 * @file set_waypoints_request.h
 * @brief Définitions du modèle de données pour les requêtes de définition de waypoints.
 * @author Lukas Grando
 * @date 2025-10-27
 */

#ifndef SET_WAYPOINTS_REQUEST_H
#define SET_WAYPOINTS_REQUEST_H

#include <cJSON.h>

#include "core/mqtt_messages/command_header.h"
#include "core/graph.h"

typedef struct {
	int nodeId;
	lane_rule_t laneRule;
	node_type_t type;
	double x;
	double y;
} waypoint_t;

typedef struct {
	command_header_t header;
	int carId;
	waypoint_t *waypoints;
	int waypointCount;
} set_waypoints_request_t;

 /** 
 * @brief Sérialise un message de demande de définition de waypoints en JSON.
 * @param msg Pointeur vers le message de demande de définition de waypoints à sérialiser.
 * @return Chaîne JSON représentant le message de demande de définition de waypoints
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *set_waypoints_request_serialize(const set_waypoints_request_t *msg);

/**
 * @brief Désérialise un message de demande de définition de waypoints à partir d'une chaîne JSON.
 * @param root Pointeur vers l'objet cJSON représentant le message de demande de définition de waypoints.
 * @param msg Pointeur vers la structure de message de demande de définition de waypoints à remplir.
 * @warning Cette fonction s'occupe uniquement de désérialiser les données. Le header doit être désérialisé séparément.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int set_waypoints_request_data_deserialize(cJSON *root, set_waypoints_request_t *msg);

/**
 * @brief Libère la mémoire allouée pour une requête de définition de waypoints.
 * @param msg Pointeur vers la requête de définition de waypoints à libérer
 * @return void
 */
void set_waypoints_request_destroy(set_waypoints_request_t *msg);

/**
 * @brief Traduit un path_t (liste de nœuds) en un tableau d'instructions de waypoint.
 * @details Alloue le tableau d'instructions. L'appelant doit le libérer.
 * @param path Le chemin de nœuds retourné par Dijkstra.
 * @param waypoints Pointeur vers le tableau d'instructions qui sera alloué.
 * @param waypointCount Pointeur vers l'entier stockant la taille du tableau.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 * @internal
 */
int convert_path_to_waypoints(const path_t *path, waypoint_t **waypoints, int *waypointCount, graph_t *map);



#endif // SET_WAYPOINTS_REQUEST_H