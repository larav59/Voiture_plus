/**
 * @file socket_protocol.h
 * @brief 
 * @details Définit les structures de données échangées entre le processus Node.js et le service C.
 * @author Lukas Grando
 */

#ifndef SOCKET_PROTO_H
#define SOCKET_PROTO_H

#include "core/logger.h"

#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <errno.h>

#define PROTO_MAX_SOCKET_PATH_LEN 256
#define PROTO_MAX_STR_LEN 256

typedef struct {
	char socket_path[PROTO_MAX_SOCKET_PATH_LEN]; ///< Chemin du socket Unix pour la communication IPC
	int max_clients; ///< Nombre max de connexions en attente
	int recv_timeout_ms; ///< Timeout de réception en millisecondes
} proto_config_t;

typedef enum {
	PROTO_CMD_UNKNOWN = 0, ///< Commande inconnué utilisée pour les erreurs.
	PROTO_CMD_SIM_ON, ///< Activer le mode simulation pour une valeur IEC spécifique.
	PROTO_CMD_SIM_OFF, ///< Désactiver le mode simulation pour une valeur IEC spécifique.
	PROTO_CMD_STATUS, ///< Obtenir le statut actuel du serveur IEC.
} proto_command_type_t;

typedef struct {
	proto_command_type_t type;
	char iec_address[PROTO_MAX_STR_LEN];
	char value[PROTO_MAX_STR_LEN];
} proto_msg_t;

/**
 * @brief Callback appelé quand un message JSON valide est reçu.
 * @param msg La commande reçue et déparsée.
 * @param json_response Pointeur vers l'objet cJSON de réponse (à remplir par l'utilisateur).
 */
typedef void (*proto_handler_t)(const proto_msg_t *msg, void *json_response);

/**
 * @brief Initialise et démarre le serveur IPC.
 * @param config Pointeur vers ta structure de configuration.
 * @param handler Fonction de rappel pour traiter les messages.
 * @return 0 si succès, -1 si erreur.
 */
int proto_server_start(const proto_config_t *config, proto_handler_t handler);

/**
 * @brief Arrête le serveur et nettoie le socket.
 */
void proto_server_stop(void);

#endif