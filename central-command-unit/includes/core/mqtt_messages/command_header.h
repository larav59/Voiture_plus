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

 #ifndef COMMAND_HEADER_H
 #define COMMAND_HEADER_H

#include "core/check.h"
#include "cJSON.h"

#define ACTION_LENGTH 64
#define COMMAND_ID_LENGTH 64
#define REPLY_TOPIC_LENGTH 128

typedef struct {
	char commandId[COMMAND_ID_LENGTH];
	char action[ACTION_LENGTH];
	char replyTopic[REPLY_TOPIC_LENGTH];
	struct timespec timestamp;
} command_header_t;


/**
 * @brief Crée et initialise une structure d'en-tête de commande.
 * @param action Chaîne représentant l'action de la commande.
 * @param replyTopic Chaîne représentant le topic de réponse.
 * @return Structure d'en-tête de commande initialisée.
 * @note la fonction génère un UUID pour commandId et initialise le timestamp courant.
 */
command_header_t create_command_header(const char *action, const char *replyTopic);
/**
 * @brief Remplit un objet cJSON avec les champs de l'en-tête.
 * @details Ajoute les clés "command_id", "action", "timestamp" à l'objet JSON racine.
 * @param header Pointeur vers la structure d'en-tête contenant les données.
 * @param root Pointeur vers l'objet cJSON racine auquel ajouter les champs.
 * @return 0 en cas de succès, -1 si une allocation cJSON échoue.
 */
int command_header_serialize(const command_header_t *header, cJSON *root);

/**
 * @brief Remplit une structure d'en-tête à partir d'un objet cJSON.
 * @details Lit les clés "command_id", "action", "timestamp" depuis l'objet JSON racine.
 * @param root Pointeur vers l'objet cJSON racine contenant les données de l'en-tête.
 * @param header Pointeur vers la structure d'en-tête à remplir.
 * @return 0 en cas de succès, -1 si un champ est manquant ou du mauvais type.
 */
int command_header_deserialize(const cJSON *root, command_header_t *header);

#endif // COMMAND_HEADER_H