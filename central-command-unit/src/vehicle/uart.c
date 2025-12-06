/**
 * @file uart.h
 * @brief Interface pour la communication UART.
 * @details
 * Ce fichier définit les fonctions pour initialiser, lire, écrire et fermer une connexion UART.
 * Inspiré du fonctionnement des sockets POSIX.
 * @author Lukas Grando
 * @date 2025-11-25
 */
#include "vehicle/uart.h"

static speed_t get_baud(int baudrate) {
	switch (baudrate) {
		case 9600: return B9600;
		case 19200: return B19200;
		case 38400: return B38400;
		case 57600: return B57600;
		case 115200: return B115200;
		case 230400: return B230400;
		case 460800: return B460800;
		case 921600: return B921600;
		default: return B9600;
	}
}

/**
 * @brief Initialise une connexion UART avec la configuration spécifiée.
 * @param config Pointeur vers la configuration UART.
 * @return Descripteur de fichier pour la connexion UART, ou -1 en cas d'erreur.
 */
int uart_open(const uart_config_t *config) {
	// O_RDWR : lecture et écriture
	// O_NOCTTY : ne pas faire de ce terminal le terminal de contrôle du processus
	// O_SYNC : écriture synchronisée

	int fd = open(config->devicePath, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd < 0) {
		LOG_ERROR_ASYNC("Failed to open UART device at %s", config->devicePath);
		return -1;
	}

	struct termios tty;
	if(tcgetattr(fd, &tty) != 0) {
		LOG_ERROR_ASYNC("Failed to get UART attributes for device at %s", config->devicePath);
		close(fd);
		return -1;
	}

	cfmakeraw(&tty);

	speed_t baud = get_baud(config->baudrate);
	cfsetispeed(&tty, baud);
	cfsetospeed(&tty, baud);

	if(config->timeoutMs > 0) {
		tty.c_cc[VMIN] = 0;
		tty.c_cc[VTIME] = config->timeoutMs / 100;
	} else if(config->timeoutMs == -1) {
		tty.c_cc[VMIN] = 1;
		tty.c_cc[VTIME] = 0;
	} else {
		tty.c_cc[VMIN] = 0;
		tty.c_cc[VTIME] = 0;
	}

	if(tcsetattr(fd, TCSANOW, &tty) != 0) {
		LOG_ERROR_ASYNC("Failed to set UART attributes for device at %s", config->devicePath);
		close(fd);
		return -1;
	}
	return fd;
}

ssize_t uart_recv_frame(int fd, void *buffer, size_t length) {
    uint8_t byte;
    uint8_t *bufferU8 = (uint8_t *)buffer;
    int maxNoiseBytes = MAX_NOISE_BYTES;

    while(1) {
        if(read(fd, &byte, 1) != 1) return -1;
        if(byte == UART_START_BYTE) break;
        if(--maxNoiseBytes <= 0) return -1;
    }

    uint8_t dataLength;
    if(read(fd, &dataLength, 1) != 1) return -1;

    if(dataLength > length) {
        return -1; 
    }

    size_t bytesRead = 0;
    while(bytesRead < dataLength) {
        ssize_t result = read(fd, bufferU8 + bytesRead, dataLength - bytesRead);
        if(result <= 0) return -1;
        bytesRead += result;
    }

    return bytesRead;
}

/**
 * @brief Écrit des données vers la connexion UART.
 * @details Modifié : N'envoie plus le byte de fin.
 */
ssize_t uart_send_frame(int fd, const void *buffer, size_t length) {
    if (length > 255) return -1;

    uint8_t header[2];
    header[0] = UART_START_BYTE;
    header[1] = (uint8_t)length;

    if (write(fd, header, 2) != 2) return -1;
    if (write(fd, buffer, length) != (ssize_t)length) return -1;

    return length;
};