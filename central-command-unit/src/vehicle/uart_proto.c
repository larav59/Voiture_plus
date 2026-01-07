#include "vehicle/uart.h"
#include "vehicle/uart_proto.h"
#include <stdio.h>

/**
 * @brief Envoie une trame UART avec un identifiant de message et une charge utile.
 * @param fd Le descripteur de fichier UART.
 * @param msg_id L'identifiant du message UART.
 * @param payload Pointeur vers la charge utile à envoyer.
 * @param payload_len Longueur de la charge utile en octets.
 * @return Le nombre d'octets envoyés, ou -1 en cas d'erreur.
 */
int protocol_send_packet(int fd, uint8_t msg_id, void *payload, size_t payloadLen) {
	LOG_DEBUG_ASYNC("UART Proto: Sending packet ID 0x%02X with payload length %zu", msg_id, payloadLen);
    size_t frameSize = 1 + payloadLen; 
    uint8_t frame[UART_MAX_FRAME_SIZE];

    if(frameSize > UART_MAX_FRAME_SIZE) {
        return -1;
    }

    frame[0] = msg_id;
    memcpy(&frame[1], payload, payloadLen);
    ssize_t bytesSent = uart_send_frame(fd, frame, frameSize);

    return bytesSent;
}

/**
 * @brief Envoie l'ordre d'aller à une position (MSG_CMD_SET_POSITION).
 * @param fd Le descripteur de fichier UART.
 * @param x La position X (en mm ou cm par exemple).
 * @param y La position Y.
 * @param angle L'angle d'orientation.
 * @return 0 en succès, -1 en erreur.
 */
int protocol_send_set_position_command(int fd, int16_t x, int16_t y, int16_t angle) {
	position_data_t data = {
		.x = x,
		.y = y,
		.angle = angle
	};

    // Gestion automatique : On passe MSG_CMD_SET_POSITION
    return protocol_send_packet(fd, MSG_CMD_SET_POSITION, &data, sizeof(data));
}

/**
 * @brief Envoie la consigne de vitesse (MSG_CMD_SET_SPEED).
 * @param fd Le descripteur de fichier UART.
 * @param speed La vitesse souhaitée.
 * @return 0 en succès, -1 en erreur.
 */
int protocol_send_set_speed(int fd, int16_t speed) {
	speed_data_t data;
	data.speed = speed;
	return protocol_send_packet(fd, MSG_CMD_SET_SPEED, &data, sizeof(data));
}

/**
 * @brief Envoie l'ordre d'arrêt (MSG_CMD_STOP).
 * @param fd Le descripteur de fichier UART.
 * @note cette commande n'a pas de charge utile.
 * @return 0 en succès, -1 en erreur.
 */
int protocol_send_stop(int fd) {
	return protocol_send_packet(fd, MSG_CMD_STOP, NULL, 0);
}

/**
 * @brief Envoie la télémétrie de position courante (MSG_TELEMETRY_POS).
 * @param fd Le descripteur de fichier UART.
 * @param x La position X (en mm ou cm par exemple).
 * @param y La position Y.
 * @param angle L'angle d'orientation.
 * @return 0 en succès, -1 en erreur.
 */
int protocol_send_position_telemetry(int fd, int16_t x, int16_t y, int16_t angle) {
	position_data_t data = {
		.x = x,
		.y = y,
		.angle = angle
	};
	
	return protocol_send_packet(fd, MSG_TELEMETRY_POS, &data, sizeof(data));
}


