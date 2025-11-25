/**
 * @file command_response_header.h
 * @brief Définitions du modèle de données pour les en-têtes des réponses aux commandes.
 * @details
 * Ce fichier définit la structure commune `command_response_header_t` utilisée
 * comme premier membre dans les structures de réponses spécifiques.
 * Il fournit également des fonctions utilitaires pour sérialiser/désérialiser
 * cette partie de l'en-tête depuis/vers un objet cJSON.
 * @author Lukas Grando
 * @date 2025-10-28
 */

#ifndef COMMAND_RESPONSE_HEADER_H
#define COMMAND_RESPONSE_HEADER_H

#include "core/check.h"

// TODO déplacer dans un fichier commun core/variables.h ou on a tous les #defines globaux
#define COMMAND_ID_LENGTH 64
#define MAX_ERROR_MSG_LEN 128

/**
 * @brief Structure commune pour l'en-tête des messages de réponse.
 * @details Doit être le premier membre des structures de réponses spécifiques.
 */
typedef struct {
    char commandId[COMMAND_ID_LENGTH];      /**< ID de la commande d'origine à laquelle on répond */
    bool success;                           /**< Statut de succès (true = OK, false = Erreur) */
    char errorMessage[MAX_ERROR_MSG_LEN];   /**< Message d'erreur (pertinent si success == false) */
    struct timespec timestamp;              /**< Timestamp de création de la réponse */
} command_response_header_t;

/**
 * @brief Crée et initialise une structure d'en-tête de réponse.
 * @details Récupère le timestamp actuel.
 * @param originalCommandId L'ID de la commande à laquelle cette réponse correspond.
 * @param success Statut de succès de l'opération.
 * @param errorMessage Message d'erreur (peut être NULL si success == true).
 * @return Une structure command_response_header_t initialisée.
 */
command_response_header_t create_command_response_header(const char *originalCommandId, bool success, const char *errorMessage);

/**
 * @brief Remplit un objet cJSON avec les champs de l'en-tête de réponse.
 * @details Ajoute les clés "command_id", "success", "error_message" (si échec),
 * "timestamp_s", "timestamp_ns" à l'objet JSON racine.
 * @param header Pointeur vers la structure d'en-tête de réponse.
 * @param root Pointeur vers l'objet cJSON racine.
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int command_response_header_to_json(const command_response_header_t *header, cJSON *root);

/**
 * @brief Sérialise une structure d'en-tête de réponse en chaîne JSON.
 * @param header Pointeur vers la structure d'en-tête de réponse à sérialiser.
 * @return Chaîne JSON représentant l'en-tête de réponse, ou NULL en cas d'erreur.
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *command_response_header_serialize(const command_response_header_t *header);

/**
 * @brief Remplit une structure d'en-tête de réponse à partir d'un objet cJSON.
 * @details Lit les clés "command_id", "success", "error_message" (si échec),
 * "timestamp_s", "timestamp_ns".
 * @param root Pointeur vers l'objet cJSON racine.
 * @param header Pointeur vers la structure d'en-tête de réponse à remplir.
 * @return 0 en cas de succès, -1 si un champ est manquant/mauvais type.
 */
int command_response_header_deserialize(const cJSON *root, command_response_header_t *header);

#endif // COMMAND_RESPONSE_HEADER_H