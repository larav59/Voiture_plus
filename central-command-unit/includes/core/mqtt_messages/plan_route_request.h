/**
 * @file plan_route_request.h
 * @brief Définitions du modèle de données pour les en-têtes des commandes.
 * @details
 * @author Lukas Grando
 * @date 2025-10-27
 */

#ifndef PLAN_ROUTE_REQUEST_H
#define PLAN_ROUTE_REQUEST_H

#include "core/check.h"
#include "core/graph.h"
#include "core/mqtt_messages/command_header.h"
#include <cJSON.h>

typedef struct {
	command_header_t header;
	int carId;
	int *nodeIds;
	int nodeCount;
} plan_route_request_t;

/**
 * @brief Sérialise un message de demande de planification de trajet en JSON.
 * @param msg Pointeur vers le message de demande de planification de trajet à sérialiser.
 * @return Chaîne JSON représentant le message de demande de planification de trajet
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *plan_route_request_serialize(const plan_route_request_t *msg);

/**
 * @brief Désérialise un message de demande de planification de trajet à partir d'une chaîne JSON.
 * @param root Pointeur vers l'objet cJSON représentant le message de demande de planification de trajet.
 * @param msg Pointeur vers la structure de message de demande de planification de trajet à remplir.
 * @warning Cette fonction s'occupe uniquement de désérialiser les données. Le header doit être désérialisé séparément.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int plan_route_request_data_deserialize(cJSON *json, plan_route_request_t *msg);

#endif // PLAN_ROUTE_REQUEST_H