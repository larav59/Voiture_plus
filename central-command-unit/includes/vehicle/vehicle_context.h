#ifndef VEHICLE_CONTEXT_H
#define VEHICLE_CONTEXT_H

#include "core/common.h" 
#include "vehicle/uart.h"
#include "core/mqtt_messages/set_waypoints_request.h"

#define SPEED_LIMIT_DEFAULT 50
#define SPEED_LIMIT_30 30

/**
 * @brief Structure représentant l'état du véhicule.
 */
typedef struct {
	int carId; //!< Identifiant du véhicule
	int uartFd; //!< Descripteur de fichier pour la connexion UART

	// Navigation
	waypoint_t *route; //!< Tableau des waypoints du trajet
	int routeLen; //!< Nombre de waypoints dans le trajet
	int currentWpIndex; //!< Index du waypoint courant
	bool isNavigating; //!< Indique si la navigation est en cours

	int16_t targetSpeedLimit; // La limitation de vitesse ciblée
	bool obstacleDetected; //!< Indique si un obstacle a été détecté

	// Télémétrie
	int16_t x; //!< Position X actuelle
	int16_t y; //!< Position Y actuelle
	int16_t angle; //!< Angle actuel
	int16_t realSpeed; //!< Vitesse réelle actuelle

} vehicle_state_t;

/**
 * @brief Obtient l'état actuel du véhicule.
 * @return Pointeur vers la structure vehicle_state_t.
 * @note Le pointeur retourné est valide tant que le contexte du véhicule est actif.
 */
vehicle_state_t* vehicle_get_state(void);

/**
 * @brief Initialise l'état du véhicule.
 * @param uart_fd Descripteur de fichier pour la connexion UART.
 * @param carId Identifiant du véhicule.
 * @return void
 * @note Doit être appelé avant toute autre opération sur le contexte du véhicule.
 */
void vehicle_init_state(int uartFd, int carId);

/**
 * @brief Nettoie les ressources allouées pour l'état du véhicule.
 * @return void
 * @note Doit être appelé lors de la fermeture du service véhicule.
 */
void vehicle_cleanup_state(void);

#endif // VEHICLE_CONTEXT_H