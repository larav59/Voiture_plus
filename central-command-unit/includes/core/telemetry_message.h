/**
 * @file telemetry_message.h
 * @brief Définitions du modèle de données pour les messages de télémétrie.
 * @details
 * Ce fichier contient les définitions des structures de données utilisées pour
 * Il inclut également la fonction de sérialisation en JSON des messages de télémétrie.
 * Et une fonction de nettoyage de la mémoire allouée pour les messages de télémétrie.
 */

#ifndef TELEMETRY_MESSAGE_H
#define TELEMETRY_MESSAGE_H

#include "core/logger.h"
#include "cJSON.h"

/**
 * @brief Structure représentant un message de télémétrie.
 */
typedef struct
{
	char *origin; /**< ID du client émetteur */
	long timestamp; /**< Timestamp du message */
	log_level_t level; /**< Niveau de log */
	char *message; /**< Contenu du message */
} telemetry_message_t;

/**
 * @brief Sérialise un message de télémétrie en JSON.
 * @param msg Pointeur vers le message de télémétrie à sérialiser.
 * @return Chaîne JSON représentant le message de télémétrie
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *telemetry_message_serialize_json(const telemetry_message_t *msg);

/**
 * @brief Désérialise un message de télémétrie à partir d'une chaîne JSON.
 * @param json Chaîne JSON représentant le message de télémétrie.
 * @param msg Pointeur vers la structure de message de télémétrie à remplir.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int telemetry_message_deserialize_json(const char *json, telemetry_message_t *msg);

#endif // TELEMETRY_MESSAGE_H