/**
 * @file set_safe_route_mode_request.h
 * @brief Définitions du modèle de données pour la commande SET_SAFE_ROUTE_MODE_REQUEST.
 * @details
 * Adressé à : Route Planner Service
 * La commande est envoyée pour demander au route planner de définir le mode de route sans zone de conflit pour un véhicule.
 */

#ifndef SET_SAFE_ROUTE_MODE_REQUEST_H
#define SET_SAFE_ROUTE_MODE_REQUEST_H

#include "core/mqtt_messages/command_header.h"
#include "core/check.h"
#include "cJSON.h"

typedef struct {
	command_header_t header;
	bool enabled;
} set_safe_route_mode_request_t;

/**
 * @brief Sérialise un message de demande de définition du mode de route sans zone de conflit en JSON.
 * @param msg Pointeur vers le message de demande de définition du mode de route sans zone de conflit à sérialiser.
 * @return Chaîne JSON représentant le message de demande de définition du mode de route sans zone de conflit
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *set_safe_route_mode_request_serialize_json(const set_safe_route_mode_request_t *msg);

/**
 * @brief Désérialise un message de demande de définition du mode de route sans zone de conflit à partir d'une chaîne JSON.
 * @param json Chaîne JSON représentant le message de demande de définition du mode de route sans zone de conflit.
 * @param msg Pointeur vers la structure de message de demande de définition du mode de route sans zone de conflit à remplir.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int set_safe_route_mode_request_deserialize_json(const char *json, set_safe_route_mode_request_t *msg);

#endif // SET_SAFE_ROUTE_MODE_REQUEST_H
