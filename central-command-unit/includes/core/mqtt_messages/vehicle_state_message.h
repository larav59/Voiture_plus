/**
 * @file vehicle_state_message.h
 * @brief Définitions du modèle de données pour les messages de télémétrie.
 * @details
 * Ce fichier contient les définitions des structures de données utilisées pour l'état du véhicule.
 */

#ifndef VEHICLE_STATE_MESSAGE_H
#define VEHICLE_STATE_MESSAGE_H
#include "core/logger.h"
#include "cJSON.h"

/**
 * @brief Structure représentant un message de l'état du véhicule.
 */
typedef struct {
	int carId; /**< ID du véhicule */
	long timestamp; /**< Timestamp du message */
	int16_t x; /**< Position X du véhicule */
	int16_t y; /**< Position Y du véhicule */
	float angle; /**< Angle du véhicule */
	int16_t speed; /**< Vitesse du véhicule */
	bool isNavigating; /**< Indique si le véhicule est en navigation */
	bool obstacleDetected; /**< Indique si un obstacle est détecté */
} vehicle_state_message_t;


/**
 * @brief Sérialise un message de l'état véhicule en JSON.
 * @param msg Pointeur vers le message de l'état véhicule à sérialiser.
 * @return Chaîne JSON représentant le message de l'état véhicule
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *vehicle_state_message_serialize_json(const vehicle_state_message_t *msg);

#endif // VEHICLE_STATE_MESSAGE_H