/**
 * @file command_header.h
 * @brief Définitions du modèle de données pour les en-têtes des commandes.
 * @details
 * Ce fichier n'est pas destinée à être utilisé directement. Il contient les définitions de entêtes qui se trouveront dans les messages MQTT de commande.
 * En effet cette interface donne uniquement accès aux fonction :
 * - Remplir la structure de l'en-tête via le payload de la commande reçue.
 * - Remplir l'objet JSON mémoire avec les données de l'en-tête pour la sérialisation.
 * @author Lukas Grando
 * @date 2025-10-27
 */

#include "core/mqtt_messages/command_header.h"

/**
 * @brief Crée et initialise une structure d'en-tête de commande.
 * @param action Chaîne représentant l'action de la commande.
 * @param replyTopic Chaîne représentant le topic de réponse.
 * @return Structure d'en-tête de commande initialisée.
 * @note la fonction génère un ID pour commandId et initialise le timestamp courant.
 */
command_header_t create_command_header(const char *action, const char *replyTopic) {
	command_header_t header = {0};
	struct timespec ts;

	clock_gettime(CLOCK_REALTIME, &ts);
	snprintf(header.commandId, COMMAND_ID_LENGTH, "%s-%ld%09ld", action, ts.tv_sec, ts.tv_nsec);
	strncpy(header.action, action, ACTION_LENGTH - 1);
	strncpy(header.replyTopic, replyTopic, REPLY_TOPIC_LENGTH - 1);
	header.timestamp = ts;

	return header;
}

/**
 * @brief Remplit un objet cJSON avec les champs de l'en-tête.
 * @details Ajoute les clés "command_id", "action", "timestamp" à l'objet JSON racine.
 * @param header Pointeur vers la structure d'en-tête contenant les données.
 * @param root Pointeur vers l'objet cJSON racine auquel ajouter les champs.
 * @return 0 en cas de succès, -1 si une allocation cJSON échoue.
 */
int command_header_serialize(const command_header_t *header, cJSON *root) {
	if(!cJSON_AddStringToObject(root, "commandId", header->commandId)) return -1;
	if(!cJSON_AddStringToObject(root, "action", header->action)) return -1;
	if(!cJSON_AddStringToObject(root, "replyTopic", header->replyTopic)) return -1;
	
	return 0;
}

/**
 * @brief Remplit une structure d'en-tête à partir d'un objet cJSON.
 * @details Lit les clés "command_id", "action", "timestamp" depuis l'objet JSON racine.
 * @param root Pointeur vers l'objet cJSON racine contenant les données de l'en-tête.
 * @param header Pointeur vers la structure d'en-tête à remplir.
 * @return 0 en cas de succès, -1 si un champ est manquant ou du mauvais type.
 */
int command_header_deserialize(const cJSON *root, command_header_t *header) {
	const cJSON *commandItem = cJSON_GetObjectItemCaseSensitive(root, "commandId");
	if (!cJSON_IsString(commandItem) || (commandItem->valuestring == NULL)) return -1;

	strncpy(header->commandId, commandItem->valuestring, COMMAND_ID_LENGTH - 1);
	header->commandId[COMMAND_ID_LENGTH - 1] = '\0';

	const cJSON *actionItem = cJSON_GetObjectItemCaseSensitive(root, "action");
	if (!cJSON_IsString(actionItem) || (actionItem->valuestring == NULL)) return -1;

	strncpy(header->action, actionItem->valuestring, ACTION_LENGTH - 1);
	header->action[ACTION_LENGTH - 1] = '\0';

	const cJSON *replyTopicItem = cJSON_GetObjectItemCaseSensitive(root, "replyTopic");
	if (!cJSON_IsString(replyTopicItem) || (replyTopicItem->valuestring == NULL)) return -1;

	strncpy(header->replyTopic, replyTopicItem->valuestring, REPLY_TOPIC_LENGTH - 1);
	header->replyTopic[REPLY_TOPIC_LENGTH - 1] = '\0';

	return 0;
}
