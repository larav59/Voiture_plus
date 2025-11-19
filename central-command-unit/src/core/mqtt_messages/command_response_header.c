/**
 * @file command_response_header.c
 * @brief Définitions du modèle de données pour les en-têtes des réponses aux commandes.
 * @details
 * Ce fichier définit la structure commune `command_response_header_t` utilisée
 * comme premier membre dans les structures de réponses spécifiques.
 * Il fournit également des fonctions utilitaires pour sérialiser/désérialiser
 * cette partie de l'en-tête depuis/vers un objet cJSON.
 * @author Lukas Grando
 * @date 2025-10-28
 */
#include "core/mqtt_messages/command_response_header.h"


/**
 * @brief Crée et initialise une structure d'en-tête de réponse.
 * @details Récupère le timestamp actuel.
 * @param originalCommandId L'ID de la commande à laquelle cette réponse correspond.
 * @param success Statut de succès de l'opération.
 * @param errorMessage Message d'erreur (peut être NULL si success == true).
 * @return Une structure command_response_header_t initialisée.
 */
command_response_header_t create_command_response_header(const char *originalCommandId, bool success, const char *errorMessage) {
	command_response_header_t header = {0};
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
int command_response_header_to_json(const command_response_header_t *header, cJSON *root) {
	if(!cJSON_AddStringToObject(root, "commandId", header->commandId)) 
	if(!cJSON_AddBoolToObject(root, "success", header->success))
	if(!cJSON_AddNumberToObject(root, "timestampSec", (double) header->timestamp.tv_sec)) 
	if(!cJSON_AddNumberToObject(root, "timestampNsec", (double) header->timestamp.tv_nsec)) return -1;	

	if (!header->success) {
		if (!cJSON_AddStringToObject(root, "errorMessage", header->errorMessage)) return -1;
	}

	return 0;
}

/**
 * @brief Sérialise une structure d'en-tête de réponse en chaîne JSON.
 * @param header Pointeur vers la structure d'en-tête de réponse à sérialiser.
 * @return Chaîne JSON représentant l'en-tête de réponse, ou NULL en cas d'erreur.
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *command_response_header_serialize(const command_response_header_t *header) {
	cJSON *root = cJSON_CreateObject();
	if(!root) return NULL;

	if(command_response_header_to_json(header, root) != 0) {
		cJSON_Delete(root);
		return NULL;
	}
	char *json = CJSON_PRINT(root);
	cJSON_Delete(root);
	return json;
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
		if (!cJSON_IsString(errorItem) || (errorItem->valuestring == NULL)) {
			strncpy(header->errorMessage, "Unknown error", MAX_ERROR_MSG_LEN - 1);
		}

		strncpy(header->errorMessage, errorItem->valuestring, MAX_ERROR_MSG_LEN - 1);
		header->errorMessage[MAX_ERROR_MSG_LEN - 1] = '\0';
	} else {
		header->errorMessage[0] = '\0';
	}

	const cJSON *timestampSItem = cJSON_GetObjectItemCaseSensitive(root, "timestampSec");
	const cJSON *timestampNsItem = cJSON_GetObjectItemCaseSensitive(root, "timestampNsec");
	if (!cJSON_IsNumber(timestampSItem) || !cJSON_IsNumber(timestampNsItem)) return -1;

	header->timestamp.tv_sec = (time_t) timestampSItem->valuedouble;
	header->timestamp.tv_nsec = (long) timestampNsItem->valuedouble;

	return 0;
}