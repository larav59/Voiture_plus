#ifndef UART_PROTO_H
#define UART_PROTO_H

#include <stdint.h>

typedef struct __attribute__((packed)) {
    int16_t x;
    int16_t y;
} position_data_t;


/**
 * @brief Envoie une commande de position à l'Arduino.
 * @param fd Le descripteur de fichier UART.
 * @param x La position X (en mm ou cm par exemple).
 * @param y La position Y.
 * @return 0 en succès, -1 en erreur.
 */
int protocol_send_position(int fd, int16_t x, int16_t y);

#endif // UART_PROTO_H