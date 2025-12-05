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
    size_t frameSize = 2 + payloadLen;
    uint8_t frame[UART_MAX_FRAME_SIZE];

    // todo on pourrait faire un système d'envoie de plusieurs trames si la taille dépasse UART_MAX_FRAME_SIZE
    if(frameSize > UART_MAX_FRAME_SIZE) {
        return -1;
    }

    frame[0] = UART_START_BYTE;
    frame[1] = msg_id;
    memcpy(&frame[2], payload, payloadLen);

    ssize_t bytesSent = uart_send_frame(fd, frame, frameSize);
    free(frame);

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
int protocol_send_set_position(int fd, int16_t x, int16_t y, int16_t angle) {
    position_data_t payload;
    
    payload.x = x;
    payload.y = y;
    payload.angle = angle;

    ssize_t sent = uart_send_frame(fd, &payload, sizeof(position_data_t));
    if (sent > 0) {
        return 0;
    }

    return -1;
}

/**
 * @brief Envoie la consigne de vitesse (MSG_CMD_SET_SPEED).
 * @param fd Le descripteur de fichier UART.
 * @param speed La vitesse souhaitée.
 * @return 0 en succès, -1 en erreur.
 */
int protocol_send_set_speed(int fd, int16_t speed) {
    speed_data_t payload;
    
    payload.speed = speed;

    ssize_t sent = uart_send_frame(fd, &payload, sizeof(speed_data_t));

    if (sent > 0) {
        return 0;
    }
    return -1;
}

/**
 * @brief Envoie l'ordre d'arrêt (MSG_CMD_STOP).
 * @param fd Le descripteur de fichier UART.
 * @note cette commande n'a pas de charge utile.
 * @return 0 en succès, -1 en erreur.
 */
int protocol_send_stop(int fd) {
    ssize_t sent = uart_send_frame(fd, NULL, 0);

    if (sent > 0) {
        return 0;
    }
    return -1;
}

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

/**
 * @brief Envoie une commande de position à l'Arduino.
 * @param fd Le descripteur de fichier UART.
 * @param x La position X (en mm ou cm par exemple).
 * @param y La position Y.
 * @return 0 en succès, -1 en erreur.
 */
int protocol_send_position(int fd, int16_t x, int16_t y, int16_t angle) {
    position_data_t payload;
    
    payload.x = x;
    payload.y = y;
    payload.angle = angle;

    ssize_t sent = uart_send_frame(fd, &payload, sizeof(position_data_t));

    if (sent != sizeof(position_data_t)) {
        return -1;
    }
    return 0;
}