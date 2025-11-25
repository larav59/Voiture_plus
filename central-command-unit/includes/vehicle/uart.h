#ifndef UART_H
#define UART_H

#include "core/check.h"
#include "core/common.h"

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
	int baudrate;
	int timeoutMs;
	char *devicePath;
} uart_config_t;

#define UART_START_BYTE 0xFF ///< Octets de début de trame UART
#define UART_END_BYTE   0xFF ///< Octets de fin de trame UART
#define MAX_NOISE_BYTES 512 //!< Nombre maximum d'octets de bruit à ignorer avant de considérer qu'il n'y a pas de trame valide


/**
 * @brief Initialise une connexion UART avec la configuration spécifiée.
 * @param config Pointeur vers la configuration UART.
 * @return Descripteur de fichier pour la connexion UART, ou -1 en cas d'erreur.
 */
int uart_open(const uart_config_t *config);

/**
 * @brief Lit des données depuis la connexion UART.
 * @param fd Descripteur de fichier de la connexion UART.
 * @param buffer Pointeur vers le tampon où les données lues seront stockées.
 * @param length Nombre d'octets à lire.
 * @return Nombre d'octets lus, ou -1 en cas d'erreur.
 */
ssize_t uart_recv_frame(int fd, void *buffer, size_t length);

/**
 * @brief Écrit des données vers la connexion UART.
 * @param fd Descripteur de fichier de la connexion UART.
 * @param buffer Pointeur vers le tampon contenant les données à écrire.
 * @param length Nombre d'octets à écrire.
 * @return Nombre d'octets écrits, ou -1 en cas d'erreur
 */
ssize_t uart_send_frame(int fd, const void *buffer, size_t length);


#endif // UART_H