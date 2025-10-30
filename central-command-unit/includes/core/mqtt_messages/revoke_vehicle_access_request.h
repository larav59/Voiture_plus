/**
 * @file revoke_vehicle_access.h
 * @brief Définitions du modèle de données pour la commande REVOKE_VEHICLE_ACCESS.
 * @details
 * Adressé à : Conflict Manager Service
 * La commande est envoyé pour annuler les demande de ressource en cours d'un véhicule.
 * Le gestionnaire de conflit va également bloqué la ressource actuellement utilisé par le véhicule pour éviter les accidents.
 */

#ifndef REVOKE_VEHICLE_ACCESS_H
#define REVOKE_VEHICLE_ACCESS_H

#include "core/mqtt_messages/command_header.h"
#include "core/check.h"
#include "cJSON.h"

typedef struct {
	command_header_t header;
	int carId;
} revoke_vehicle_access_t;

/**
 * @brief Sérialise un message de révocation d'accès véhicule en JSON.
 * @param msg Pointeur vers le message de révocation d'accès véhicule à sérialiser.
 * @return Chaîne JSON représentant le message de révocation d'accès véhicule
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *revoke_vehicle_access_serialize_json(const revoke_vehicle_access_t *msg);

/**
 * @brief Désérialise un message de révocation d'accès véhicule à partir d'une chaîne JSON.
 * @param json Chaîne JSON représentant le message de révocation d'accès véhicule.
 * @param msg Pointeur vers la structure de message de révocation d'accès véhicule à remplir.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int revoke_vehicle_access_deserialize_json(const char *json, revoke_vehicle_access_t *msg);

#endif // REVOKE_VEHICLE_ACCESS_H
