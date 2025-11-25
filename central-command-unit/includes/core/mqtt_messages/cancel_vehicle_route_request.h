/**
 * @file cancel_vehicle_route_request.h
 * @brief Définitions du modèle de données pour la commande CANCEL_VEHICLE_ROUTE_REQUEST.
 * @details
 * Adressé à : Route Planner Service
 * La commande est envoyée lorsqu'on veut que le route planner annule le trajet en cours d'un véhicule
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
 * @brief Sérialise un message de demande d'annulation de trajet véhicule en JSON.
 * @param msg Pointeur vers le message de demande d'annulation de trajet véhicule à sérialiser.
 * @return Chaîne JSON représentant le message de demande d'annulation de trajet véhicule
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *cancel_vehicle_route_request_serialize(const cancel_vehicle_route_request_t *msg);

/**
 * @brief Désérialise un message de demande d'annulation de trajet véhicule à partir d'une chaîne JSON.
 * @param root Pointeur vers l'objet cJSON représentant le message de demande d'annulation de trajet véhicule.
 * @param msg Pointeur vers la structure de message de demande d'annulation de trajet véhicule à remplir.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 * @warning Cette fonction s'occupe uniquement de désérialiser les données. Le header doit être désérialisé séparément.
 */
int cancel_vehicle_route_request_data_deserialize(cJSON *root, cancel_vehicle_route_request_t *msg);

#endif // CANCEL_VEHICLE_ROUTE_REQUEST_H
