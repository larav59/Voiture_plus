/**
 * @file uart_proto.h
 * @brief Protocole de communication UART entre le central-command-unit et l'Arduino du véhicule.
 * @details
 * Définit les types de messages et les structures de données utilisées pour la communication UART.
 * Ce protocole est simple et défini un paquet de données de ce type :
 * - Chaque trame commence par un identifiant de message (1 octet)
 * - Suivi d'un payload variable selon le type de message
 * - Les messages de commande sont ceux de valeur 0x10 à 0x1F (16 commandes possibles)
 * - Les messages de télémétrie sont ceux de valeur 0x20 à 0x2F (16 types de télémétrie possibles)
 * 
 * Ce paquet est ensuite encapsulé dans une trame UART définie dans uart.c
 * Pour avoir une trame UART complète de cette forme :
 * [START_BYTE] [LENGTH] [MSG_ID] [PAYLOAD]
 * @note Ce protocol considère que la liaison UART est fiable sans erreurs de transmission. Un checksum pourrait être ajouté pour plus de robustesse.
 * @author Lukas Grando
 * @date 2025-12-05
 */
#ifndef UART_PROTO_H
#define UART_PROTO_H

#include <stdint.h>

typedef enum {
    MSG_CMD_SET_POSITION = 0x10,  ///< Envoie une commande de position
    MSG_CMD_SET_SPEED    = 0x11,  ///< Envoie une commande de vitesse
    MSG_CMD_STOP         = 0x12,  ///< Commande d'arrêt immédiat 

    MSG_TELEMETRY_POS    = 0x20   ///< Télémetrie de position
} uart_msg_type_t;

typedef struct __attribute__((packed)) {
    int16_t x;
    int16_t y;
    int16_t angle;
} position_data_t;

typedef struct __attribute__((packed)) {
    int16_t speed;
} speed_data_t;

/**
 * @brief Envoie une trame UART avec un identifiant de message et une charge utile.
 * @param fd Le descripteur de fichier UART.
 * @param msg_id L'identifiant du message UART.
 * @param payload Pointeur vers la charge utile à envoyer.
 * @param payload_len Longueur de la charge utile en octets.
 * @return Le nombre d'octets envoyés, ou -1 en cas d'erreur.
 */
int protocol_send_packet(int fd, uint8_t msg_id, void *payload, size_t payload_len);

/**
 * @brief Envoie l'ordre d'aller à une position (MSG_CMD_SET_POSITION).
 * @param fd Le descripteur de fichier UART.
 * @param x La position X (en mm ou cm par exemple).
 * @param y La position Y.
 * @param angle L'angle d'orientation.
 * @return 0 en succès, -1 en erreur.
 */
int protocol_send_set_position(int fd, int16_t x, int16_t y, int16_t angle);

/**
 * @brief Envoie la consigne de vitesse (MSG_CMD_SET_SPEED).
 * @param fd Le descripteur de fichier UART.
 * @param speed La vitesse souhaitée.
 * @return 0 en succès, -1 en erreur.
 */
int protocol_send_set_speed(int fd, int16_t speed);

/**
 * @brief Envoie l'ordre d'arrêt (MSG_CMD_STOP).
 * @param fd Le descripteur de fichier UART.
 * @note cette commande n'a pas de charge utile.
 * @return 0 en succès, -1 en erreur.
 */
int protocol_send_stop(int fd);

/**
 * @brief Envoie la télémétrie de position courante (MSG_TELEMETRY_POS).
 * @param fd Le descripteur de fichier UART.
 * @param x La position X (en mm ou cm par exemple).
 * @param y La position Y.
 * @param angle L'angle d'orientation.
 * @return 0 en succès, -1 en erreur.
 */
int protocol_send_position_telemetry(int fd, int16_t x, int16_t y, int16_t angle);

/**
 * @brief Envoie une commande de position à l'Arduino.
 * @param fd Le descripteur de fichier UART.
 * @param x La position X (en mm ou cm par exemple).
 * @param y La position Y.
 * @return 0 en succès, -1 en erreur.
 */
int protocol_send_position_telemetry(int fd, int16_t x, int16_t y, int16_t angle);

#endif // UART_PROTO_H