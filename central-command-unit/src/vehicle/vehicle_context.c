#include "vehicle/vehicle_context.h"

static vehicle_state_t g_vehicleState = {0};

/**
 * @brief Initialise l'état du véhicule.
 * @param uart_fd Descripteur de fichier pour la connexion UART.
 * @return void
 * @note Doit être appelé avant toute autre opération sur le contexte du véhicule.
 */
void vehicle_init_state(int uartFd, int carId) {
    g_vehicleState.uartFd = uartFd;
    g_vehicleState.carId = carId;
    
    // Navigation
    g_vehicleState.route = NULL;
    g_vehicleState.routeLen = 0;
    g_vehicleState.currentWpIndex = 0;
    g_vehicleState.isNavigating = false;
    
    // Logique
    g_vehicleState.targetSpeedLimit = SPEED_LIMIT_DEFAULT;
    g_vehicleState.obstacleDetected = false;

    // Télémétrie (todo: semaphore pour accès thread-safe)
    g_vehicleState.x = 0;
    g_vehicleState.y = 0;
    g_vehicleState.angle = 0;
    g_vehicleState.realSpeed = 0;
}

/**
 * @brief Obtient l'état actuel du véhicule.
 * @return Pointeur vers la structure vehicle_state_t.
 * @note Le pointeur retourné est valide tant que le contexte du véhicule est actif.
 */
vehicle_state_t* vehicle_get_state(void) {
    return &g_vehicleState;
}

/**
 * @brief Nettoie les ressources allouées pour l'état du véhicule.
 * @return void
 * @note Doit être appelé lors de la fermeture du service véhicule.
 */
void vehicle_cleanup_state(void) {
    if (g_vehicleState.route) {
        free(g_vehicleState.route);
        g_vehicleState.route = NULL;
    }
}