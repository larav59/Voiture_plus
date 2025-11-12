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
#define MAX_ERROR_MSG_LEN 1024

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
command_response_header_t create_command_response_header(const char *originalCommandId, bool success, const char *errorMessage) {
	command_response_header_t header;
	struct timespec ts;

	strncpy(header.commandId, originalCommandId, COMMAND_ID_LENGTH - 1);
	header.commandId[COMMAND_ID_LENGTH - 1] = '\0';

	header.success = success;

	if (!success && errorMessage) {
		strncpy(header.errorMessage, errorMessage, MAX_ERROR_MSG_LEN - 1);
		header.errorMessage[MAX_ERROR_MSG_LEN - 1] = '\0';
	} else {
		header.errorMessage[0] = '\0';
	}

	clock_gettime(CLOCK_REALTIME, &ts);
	header.timestamp = ts;
	return header;
}

/**
 * @brief Remplit un objet cJSON avec les champs de l'en-tête de réponse.
 * @details Ajoute les clés "command_id", "success", "error_message" (si échec),
 * "timestamp_s", "timestamp_ns" à l'objet JSON racine.
 * @param header Pointeur vers la structure d'en-tête de réponse.
 * @param root Pointeur vers l'objet cJSON racine.
 * @return 0 en cas de succès, -1 en cas d'échec.
 */
int command_response_header_serialize(const command_response_header_t *header, cJSON *root) {
	if(!cJSON_AddStringToObject(root, "commandId", header->commandId)) 
	if(!cJSON_AddBoolToObject(root, "success", header->success))
	if(!cJSON_AddNumberToObject(root, "timestamp_s", (double) header->timestamp.tv_sec)) 
	if(!cJSON_AddNumberToObject(root, "timestamp_ns", (double) header->timestamp.tv_nsec)) return -1;	

	if (!header->success) {
		if (!cJSON_AddStringToObject(root, "errorMessage", header->errorMessage)) return -1;
	}

	return 0;
}

/**
 * @brief Remplit une structure d'en-tête de réponse à partir d'un objet cJSON.
 * @details Lit les clés "command_id", "success", "error_message" (si échec),
 * "timestamp_s", "timestamp_ns".
 * @param root Pointeur vers l'objet cJSON racine.
 * @param header Pointeur vers la structure d'en-tête de réponse à remplir.
 * @return 0 en cas de succès, -1 si un champ est manquant/mauvais type.
 */
int command_response_header_deserialize(const cJSON *root, command_response_header_t *header) {
	const cJSON *commandItem = cJSON_GetObjectItemCaseSensitive(root, "commandId");
	if (!cJSON_IsString(commandItem) || (commandItem->valuestring == NULL)) return -1;

	strncpy(header->commandId, commandItem->valuestring, COMMAND_ID_LENGTH - 1);
	header->commandId[COMMAND_ID_LENGTH - 1] = '\0';

	const cJSON *successItem = cJSON_GetObjectItemCaseSensitive(root, "success");
	if (!cJSON_IsBool(successItem)) return -1;

	header->success = cJSON_IsTrue(successItem);

	if (!header->success) {
		const cJSON *errorItem = cJSON_GetObjectItemCaseSensitive(root, "errorMessage");
		if (!cJSON_IsString(errorItem) || (errorItem->valuestring == NULL)) return -1;

		strncpy(header->errorMessage, errorItem->valuestring, MAX_ERROR_MSG_LEN - 1);
		header->errorMessage[MAX_ERROR_MSG_LEN - 1] = '\0';
	} else {
		header->errorMessage[0] = '\0';
	}

	const cJSON *timestampSItem = cJSON_GetObjectItemCaseSensitive(root, "timestamp_s");
	const cJSON *timestampNsItem = cJSON_GetObjectItemCaseSensitive(root, "timestamp_ns");
	if (!cJSON_IsNumber(timestampSItem) || !cJSON_IsNumber(timestampNsItem)) return -1;

	header->timestamp.tv_sec = (time_t) timestampSItem->valuedouble;
	header->timestamp.tv_nsec = (long) timestampNsItem->valuedouble;

	return 0;
}

#endif // COMMAND_RESPONSE_HEADER_H