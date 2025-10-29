/**
 * @file cancel_vehicle_route_request.h
 * @brief Définitions du modèle de données pour la commande CANCEL_VEHICLE_ROUTE_REQUEST.
 * @details
 * Adressé à : Route Planner Service
 * La commande est envoyé qu'on veut que le route planner annule le trajet en cours d'un véhicule
 */

#ifndef CANCEL_VEHICLE_ROUTE_REQUEST_H
#define CANCEL_VEHICLE_ROUTE_REQUEST_H

#include "core/mqtt_messages/command_header.h"
#include "core/check.h"
#include "cJSON.h"

typedef struct {
	command_header_t header;
	int carId;
} cancel_vehicle_route_request_t;

/**
 * @brief Sérialise un message de télémétrie en JSON.
 * @param msg Pointeur vers le message de télémétrie à sérialiser.
 * @return Chaîne JSON représentant le message de télémétrie
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *cancel_vehicle_route_request_serialize_json(const cancel_vehicle_route_request_t *msg);

/**
 * @brief Désérialise un message de télémétrie à partir d'une chaîne JSON.
 * @param json Chaîne JSON représentant le message de télémétrie.
 * @param msg Pointeur vers la structure de message de télémétrie à remplir.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int cancel_vehicle_route_request_deserialize_json(const char *json, cancel_vehicle_route_request_t *msg);

#endif // CANCEL_VEHICLE_ROUTE_REQUEST_H
