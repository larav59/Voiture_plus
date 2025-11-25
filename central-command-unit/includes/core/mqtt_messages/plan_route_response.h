/**
 * @file plan_route_response.h
 * @brief Définitions du modèle de données pour les réponses de planification de trajet.
 * @author Lukas Grando
 * @date 2025-11-25
 */

#ifndef PLAN_ROUTE_RESPONSE_H
#define PLAN_ROUTE_RESPONSE_H

#include "core/check.h"
#include "core/mqtt_messages/command_response_header.h"
#include "core/graph.h"

typedef struct {
	command_response_header_t header; /**< En-tête de la commande de réponse */
	int *nodeIds; /**< Liste des IDs de nœuds représentant la route planifiée */
	int nodeCount; /**< Nombre de nœuds dans la route planifiée */
	int carId;  /**< ID du véhicule pour lequel la route a été planifiée */
} plan_route_response_t;

/**
 * @brief Sérialise une réponse de planification de trajet en JSON.
 * @param msg Pointeur vers la structure à sérialiser.
 * @return Chaîne JSON allouée (à libérer par l'appelant), ou NULL en cas d'erreur.
 */
char *plan_route_response_serialize(const plan_route_response_t *msg);

/**
 * @brief Désérialise les données spécifiques (la liste des nœuds) d'une réponse.
 * @details Alloue la mémoire pour msg->nodeIds.
 * @param root L'objet cJSON racine (déjà parsé).
 * @param msg Pointeur vers la structure à remplir.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int plan_route_response_data_deserialize(const cJSON *root, plan_route_response_t *msg);


#endif // PLAN_ROUTE_RESPONSE_H