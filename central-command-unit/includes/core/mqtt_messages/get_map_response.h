/**
 * @file get_map_response.h
 * @brief Définit le message de réponse pour la requête de carte.
 * @details
 * Ce fichier contient la définition de la structure de données
 * utilisée pour répondre aux requêtes de carte via MQTT.
 * @author Lukas Grando
 * @date 2025-11-19
 */

#ifndef MQTT_MESSAGES_GET_MAP_RESPONSE_H
#define MQTT_MESSAGES_GET_MAP_RESPONSE_H

#include "core/common.h"
#include "core/core.h"
#include "core/graph.h"
#include "core/mqtt_messages/command_response_header.h"


typedef struct {
	command_response_header_t header; /**< En-tête de la commande de réponse */
	graph_t *map; 					   /**< Pointeur vers la carte renvoyée */
} get_map_response_t;


/**
 * @brief Sérialise une réponse de demande de carte en JSON.
 * @param msg Pointeur vers le message de demande de carte à sérialiser.
 * @return Chaîne JSON représentant le message de demande de carte
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *get_map_response_serialize(const get_map_response_t *msg);

/**
 * @brief Désérialise un message de demande de carte à partir d'une chaîne JSON.
 * @param root Pointeur vers l'objet cJSON représentant le message de demande de carte.
 * @param msg Pointeur vers la structure de message de demande de carte à remplir.
 * @warning Cette fonction s'occupe uniquement de désérialiser les données. Le header doit être désérialisé séparément.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int get_map_response_data_deserialize(cJSON *json, get_map_response_t *msg);

#endif // MQTT_MESSAGES_GET_MAP_RESPONSE_H

