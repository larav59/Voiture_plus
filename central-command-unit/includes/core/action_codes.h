/**
 * @file action_codes.h
 * @brief Définitions des codes d'action pour les commandes MQTT.
 * @details
 * Ce fichier contient les définitions des codes d'action utilisés dans les messages MQTT
 * Cela évite les magiques strings dispersées dans le code.
 * @author Lukas Grando
 * @date 2025-10-30
 */

#ifndef ACTION_CODES_H
#define ACTION_CODES_H

#define ACTION_CANCEL_VEHICLE_ROUTE   "CANCEL_VEHICLE_ROUTE_REQUEST"
#define ACTION_REVOKE_VEHICLE_ACCESS  "REVOKE_VEHICLE_ACCESS"
#define ACTION_SET_SAFE_ROUTE_MODE    "SET_SAFE_ROUTE_MODE"
#define ACTION_SET_RAILWAY_MODE       "SET_RAILWAY_MODE"
#define ACTION_GET_MAP_REQUEST 	      "GET_MAP_REQUEST"
#define ACTION_PLAN_ROUTE_REQUEST     "PLAN_ROUTE_REQUEST"
#define ACTION_SET_WAYPOINTS_REQUEST  "SET_WAYPOINTS_REQUEST"
#define ACTION_START_ROUTE 		 	  "START_ROUTE"

#endif // ACTION_CODES_H