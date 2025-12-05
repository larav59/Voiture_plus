#include "vehicle/uart.h"
#include "vehicle/uart_proto.h"
#include <stdio.h>

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