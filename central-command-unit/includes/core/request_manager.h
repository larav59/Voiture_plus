/**
 * @file request_manager.h
 * @brief Gère le suivi des requêtes MQTT en attente de réponse.
 * @details Utilise une hashmap pour corréler les réponses
 * @note L'implémentation est thread-safe
 * @author Lukas Grando
 * @date 2025-11-11
 */
#ifndef CORE_REQUEST_MANAGER_H
#define CORE_REQUEST_MANAGER_H

#include "core/common.h"
#include "core/check.h"
#include "core/mqtt_messages/command_header.h"
#include "core/mqtt_messages/command_response_header.h"
#include "core/logger.h"
#include <uthash.h>

/**
 * @brief Pointeur de fonction pour un callback de réponse.
 * @param payload Le payload JSON brut de la réponse.
 * @param context Le pointeur "contexte" fourni lors de l'enregistrement.
 */
typedef void (*response_callback_t)(const cJSON* root, const command_response_header_t* header, void* context);

/**
 * @brief Initialise le gestionnaire de requêtes. Hashmap + semaphore.
 * @note Doit être appelé une fois au démarrage du service.
 */
void request_manager_init(void);

/**
 * @brief Nettoie le gestionnaire de requêtes. Libère la Hashmap et le semaphore.
 */
void request_manager_destroy(void);

/**
 * @brief Enregistre une nouvelle requête en attente, son callback et son contexte.
 * @param requestId L'ID de la requête qui sera envoyée.
 * @param callback La fonction à appeler lorsque la réponse arrivera.
 * @param context le contexte à passer au callback lors de l'appel pouvant être utilisé pour stocker des données spécifiques.
 * @return 0 en cas de succès, -1 si l'ID existe déjà ou en cas d'erreur.
 */
int request_manager_register(const char* requestId, response_callback_t callback, void* context);

/**
 * @brief Traite un payload de réponse entrant.
 * @details Extrait l'id de la commande, et cherche le callback
 * correspondant dans la hashmap. Si trouvé, appelle le callback avec le payload et le contexte.
 * @param payload Le payload JSON brut reçu.
 * @return 0 si la réponse a été trouvée et traitée, -1 si elle était inattendue ou invalide.
 */
int request_manager_process_response(const char* payload);

#endif // REQUEST_MANAGER_H