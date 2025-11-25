/**
 * @file get_map_request.h
 * @brief Définitions du modèle de données pour la requête de récupération de la carte.
 * @author Lukas Grando
 * @date 2025-10-27
 */

#ifndef GET_MAP_REQUEST_H
#define GET_MAP_REQUEST_H

#include "core/check.h"
#include "core/graph.h"
#include "core/mqtt_messages/command_header.h"
#include <cJSON.h>

typedef struct {
	command_header_t header;
} get_map_request_t;

/**
 * @brief Sérialise un message de demande de récupération de carte en JSON.
 * @param msg Pointeur vers le message de demande de récupération de carte à sérialiser.
 * @return Chaîne JSON représentant le message de demande de récupération de carte
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *get_map_request_serialize_json(const get_map_request_t *msg);

/**
 * @brief Désérialise un message de demande de récupération de carte à partir d'une chaîne JSON.
 * @param json Chaîne JSON représentant le message de demande de récupération de carte.
 * @param msg Pointeur vers la structure de message de demande de récupération de carte à remplir.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int get_map_request_deserialize_json(const char *json, get_map_request_t *msg);

#endif // GET_MAP_REQUEST_H