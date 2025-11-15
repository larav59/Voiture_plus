/**
 * @file heartbeat.h
 * @brief fichier d'en-tête pour le service heartbeat
 * @details
 * Définit les structures et fonctions spécifiques au service heartbeat. ainsi que la version du service.
 */

#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "core/core.h"
#include "heartbeat/heartbeat_message_callback.h"

#define HEARTBEAT_SERVICE_VERSION "Heartbeat v1.0.0"
#define HEARTBEAT_REPLY_TOPIC "services/heartbeat/response"
#endif // HEARTBEAT_H