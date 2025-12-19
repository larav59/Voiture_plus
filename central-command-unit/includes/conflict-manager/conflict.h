/**
 * @file conflict.h
 * @brief Définitions pour la gestion des verrous de zones de conflit
 * @details
 * Ce fichier contient la définition des structures et fonctions utilisées pour gérer les zones de conflit et dépassements.
 * Elle utilisera un système de table de hachage pour suivre les verrous actifs. Et proposera des fonctions pour :
 * - Demander un verrou sur une zone
 * - Libérer un verrou
 * - Vérifier l'état d'un verrou
 */

#ifndef CONFLICT_H
#define CONFLICT_H

#include "core/check.h"
#include "core/priority_queue.h"
#include "core/common.h"
#include "core/graph.h"

#include <uthash.h>
#include <pthread.h>

#define MAX_KEY_LEN 64
#define DEFAULT_QUEUE_CAPACITY 8

typedef enum {
	CONFLICT_PRIORITY_HIGH = 0,     ///< Priorité haute
	CONFLICT_PRIORITY_LOW = 1,      ///< Priorité basse
} conflict_priority_t;

typedef enum {
	CONFLICT_GRANTED = 0, ///< Le verrou a été accordé
	CONFLICT_DENIED, ///< Le verrou est déjà pris par un autre véhicule
	CONFLICT_WAITING, ///< Le verrou est en attente (file d'attente)

	CONFLICT_ERROR = 100, ///< Une erreur est survenue lors de la tentative de verrouillage
} conflict_lock_status_t;

/**
 * @brief Initialise le gestionnaire de conflits.
 */
void conflict_init(int64_t defaultDecayTimeMs);

/**
 * @brief Tente de verrouiller un segment de route.
 * @param origin ID du noeud de départ.
 * @param target ID du noeud d'arrivée.
 * @param rule Règle de conduite (DRIVE_RIGHT, DRIVE_LEFT...).
 * @param vehiculeId ID du véhicule demandant le verrou.
 * @param[out] promotedVehicleId ID du véhicule promu au verrou ou -1 si aucun.
 * @return conflict_lock_status_t indiquant le résultat de la tentative de verrouillage.
 */
conflict_lock_status_t conflict_lock_lane(int origin, int target, lane_rule_t rule, conflict_priority_t priority, int vehicleId, int *promotedVehicleId);

/**
 * @brief Libère un segment de route.
 * * @param origin ID du noeud de départ.
 * @param target ID du noeud d'arrivée.
 * @param rule Règle de conduite utilisée.
 * @param vehicleId ID du véhicule (seul le propriétaire peut déverrouiller).
 * @param[out] promotedVehicleId ID du véhicule promu au verrou ou -1 si aucun.
 * @return true si déverrouillé avec succès.
 * @return false si erreur (mauvais propriétaire ou pas de verrou).
 */
conflict_lock_status_t conflict_unlock_lane(int origin, int target, lane_rule_t rule, int vehicleId, int *promotedVehicleId);

/**
 * @brief Fonction de maintenance (Garbage Collector).
 * @details Parcourt la table de hachage pour supprimer les verrous expirés.
 * À appeler régulièrement (ex: toutes les secondes).
 */
void conflict_cleanup(void);

/**
 * @brief (Debug) Affiche l'état actuel des verrous dans les logs.
 */
void conflict_dump_state(void);

/**
 * @brief Détruit le gestionnaire et libère la mémoire.
 */
void conflict_destroy(void);

#endif // CONFLICT_H