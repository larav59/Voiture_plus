/**
 * @file vehicle_local_decision.h
 * @brief Définitions pour la prise de décision locale du véhicule.
 * @details
 * Ce fichier contient les définitions des structures et fonctions utilisées
 * pour la prise de décision locale du véhicule, y compris la gestion des obstacles et l'ajustement de la vitesse.
 * @author Lukas Grando
 * @date 2026-01-04
 */

#ifndef VEHICLE_LOCAL_DECISION_H
#define VEHICLE_LOCAL_DECISION_H

#include "core/common.h"
#include "vehicle/vehicle_context.h"
#include "vehicle/uart_proto.h"
#include "vehicle/marvelmind_wrapper.h"
#include "vehicle/socket_data_camera.h"
#include "core/mqtt_messages/vehicle_state_message.h"
#include "core/core.h"

#include <math.h>

#define WAYPOINT_REACHED_THRESHOLD_MM 80  //!< Seuil pour considérer qu'un waypoint est atteint (en mm)
#define DETECTION_CONFIDENCE_THRESHOLD 0.50f //!< Seuil de confiance pour détecter un objet

/**
 * @brief Traite les objets détectés par la caméra pour la prise de décision locale.
 * @param objects Tableau des objets détectés par la caméra.
 * @param count Nombre d'objets dans le tableau.
 */
void on_camera_objects_logic(const camera_detected_object_t* objects, int count);

/**
 * @brief Traite les données de télémétrie du véhicule pour la prise de décision locale.
 * @param x Position X actuelle du véhicule.
 * @param y Position Y actuelle du véhicule.
 * @param angle Angle actuel du véhicule.
 * @param speed Vitesse actuelle du véhicule.
 */
void on_vehicle_telemetry_logic(int16_t x, int16_t y, int16_t angle, int16_t speed);

#endif // VEHICLE_LOCAL_DECISION_H