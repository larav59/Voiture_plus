/**
 * @file set_railway_mode_request.h
 * @brief Définitions du modèle de données pour la commande SET_RAILWAY_MODE_REQUEST.
 * @details
 * Adressé à : Railway Service
 * La commande est envoyée pour demander au service ferroviaire de définir le mode ferroviaire pour un véhicule.
 */

#ifndef SET_RAILWAY_MODE_REQUEST_H
#define SET_RAILWAY_MODE_REQUEST_H

#include "core/mqtt_messages/command_header.h"
#include "core/check.h"
#include "cJSON.h"

typedef struct {
	command_header_t header;
	bool enabled;
} set_railway_mode_request_t;

/**
 * @brief Sérialise un message de demande de définition du mode ferroviaire en JSON.
 * @param msg Pointeur vers le message de demande de définition du mode ferroviaire à sérialiser.
 * @return Chaîne JSON représentant le message de demande de définition du mode ferroviaire
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *set_railway_mode_request_serialize(const set_railway_mode_request_t *msg);

/**
 * @brief Désérialise un message de demande de définition du mode ferroviaire à partir d'une chaîne JSON.
 * @param root Pointeur vers l'objet cJSON représentant le message de demande de définition du mode ferroviaire.
 * @param msg Pointeur vers la structure de message de demande de définition du mode ferroviaire à remplir.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 * @warning Cette fonction s'occupe uniquement de désérialiser les données. Le header doit être désérialisé séparément.
 */
int set_railway_mode_request_data_deserialize(cJSON *root, set_railway_mode_request_t *msg);

#endif // SET_RAILWAY_MODE_REQUEST_H
