/**
 * @file telemetry_message.c
 * @brief Définitions du modèle de données pour les messages de télémétrie.
 * @details
 * Ce fichier contient les définitions des structures de données utilisées pour
 * Il inclut également la fonction de sérialisation en JSON des messages de télémétrie.
 * Et une fonction de nettoyage de la mémoire allouée pour les messages de télémétrie.
 * @author Lukas
 * @date 2025-10-24
 */

#include "core/telemetry_message.h"

#ifdef DEBUG
	// En mode debug, on formate le JSON pour qu'il soit lisible
	#define CJSON_PRINT(x) cJSON_Print(x)
#else
	// En mode release, on compacte pour optimiser la taille
	#define CJSON_PRINT(x) cJSON_PrintUnformatted(x)
#endif

/**
 * @brief Sérialise un message de télémétrie en JSON.
 * @param msg Pointeur vers le message de télémétrie à sérialiser.
 * @return Chaîne JSON représentant le message de télémétrie
 * @warning La mémoire allouée pour la chaîne JSON doit être libérée par l'appelant.
 */
char *telemetry_message_serialize_json(const telemetry_message_t *msg) {
	cJSON *root = cJSON_CreateObject();
	if(!root) return NULL;

	if(!cJSON_AddStringToObject(root, "origin", msg->origin)) goto error_cleanup;
	if(!cJSON_AddNumberToObject(root, "timestamp", (double) msg->timestamp)) goto error_cleanup;
	if(!cJSON_AddStringToObject(root, "level", logger_level_to_string(msg->level))) goto error_cleanup;
	if(!cJSON_AddStringToObject(root, "message", msg->message)) goto error_cleanup;

	char *json_str = CJSON_PRINT(root);
	cJSON_Delete(root);

	return json_str;

	error_cleanup:
		cJSON_Delete(root);
		return NULL;
}

/**
 * @brief Désérialise un message de télémétrie à partir d'une chaîne JSON.
 * @param json Chaîne JSON représentant le message de télémétrie.
 * @param msg Pointeur vers la structure de message de télémétrie à remplir.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 * @warning La mémoire allouée pour les champs de msg et origin doit être libérée par l'appelant.
 */
int telemetry_message_deserialize_json(const char *json, telemetry_message_t *msg) {
	if (!json || !msg) {
		return -1;
	}

	cJSON *root = cJSON_Parse(json);
	if (!root) {
		return -1;
	}

	cJSON *originItem    = cJSON_GetObjectItemCaseSensitive(root, "origin");
	cJSON *timestampItem = cJSON_GetObjectItemCaseSensitive(root, "timestamp");
	cJSON *levelItem     = cJSON_GetObjectItemCaseSensitive(root, "level");
	cJSON *messageItem   = cJSON_GetObjectItemCaseSensitive(root, "message");

	if (!cJSON_IsString(originItem) || !originItem->valuestring ||!cJSON_IsNumber(timestampItem) || !cJSON_IsString(levelItem) || !levelItem->valuestring || !cJSON_IsString(messageItem) || !messageItem->valuestring){
		cJSON_Delete(root);
		return -1;
	}

	msg->origin = strdup(originItem->valuestring);
	msg->timestamp = (long)timestampItem->valuedouble;

	msg->level = logger_string_to_level(levelItem->valuestring);
	msg->message = strdup(messageItem->valuestring);


	cJSON_Delete(root);
	return 0;
}