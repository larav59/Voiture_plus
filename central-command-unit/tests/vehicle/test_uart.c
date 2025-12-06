#define _GNU_SOURCE

#include "tests/runner.h"
#include "vehicle/uart.h"

// IMPORTANT: UNPACKED permet d'éviter des octets de padding ajoutés par le compilateur
// Alternative sérialiser les données en uint8 ou autre
typedef struct __attribute__((packed)) {
	double x;
	double y;
	double angle;
	double speed;
} test_vehicle_state_t;

static int create_mock_uart_port(char *path, size_t lenght) {
	int masterFd = posix_openpt(O_RDWR | O_NOCTTY);
	if(masterFd < 0) {
		return -1;
	}

	if(grantpt(masterFd) != 0 || unlockpt(masterFd) != 0) {
		close(masterFd);
		return -1;
	}

	char *pts = ptsname(masterFd);
	if(!pts) {
		close(masterFd);
		return -1;
	}

	strncpy(path, pts, lenght - 1);
	path[lenght - 1] = '\0';

	return masterFd;
}

TEST_REGISTER(test_uart_send_receive, "Test d'envoi et de réception de trames UART") {
	char uartPath[128];
	int masterFd = create_mock_uart_port(uartPath, sizeof(uartPath));
	TEST_ASSERT(masterFd >= 0, "Failed to create mock UART port");

	uart_config_t config = {
		.baudrate = 9600,
		.timeoutMs = 1000,
		.devicePath = uartPath
	};

	int uartFd = uart_open(&config);
	TEST_ASSERT(uartFd >= 0, "Failed to open UART connection");

	test_vehicle_state_t sendState = {
		.x = 12.34,
		.y = 56.78,
		.angle = 90.0,
		.speed = 5.5
	};

	// Test d'envoie d'une trame

	ssize_t bytesSent = uart_send_frame(uartFd, &sendState, sizeof(sendState));
	TEST_ASSERT(bytesSent == sizeof(sendState), "Failed to send complete UART frame");

	// lecture brute du buffer afin de vérifier l'intégrité des données
	uint8_t rawBuffer[1000];
	ssize_t rawBytesRead = read(masterFd, rawBuffer, sizeof(rawBuffer));

	TEST_ASSERT(rawBytesRead == sizeof(sendState) + 3, "Failed to read raw UART data size"); 
    TEST_ASSERT(rawBuffer[0] == UART_START_BYTE, "Invalid start byte");
    TEST_ASSERT(rawBuffer[1] == sizeof(sendState), "Invalid Length byte"); 
	
	bool dataMatches = true;
	for(size_t i = 0; i < sizeof(sendState); i++) {
		if(rawBuffer[i + 2] != *((uint8_t *)&sendState + i)) {
			dataMatches = false;
			break;
		}
	}
	TEST_ASSERT(dataMatches, "Mismatch in raw UART frame data");

	// Test de réception d'une trame
	test_vehicle_state_t simData = {
		.x = 98.76,
		.y = 54.32,
		.angle = 180.0,
		.speed = 10.0
	};

	// écriture brute dans le buffer pour simuler une trame reçue
	uint8_t simBuffer[sizeof(simData) + 3];
	simBuffer[0] = UART_START_BYTE;
	simBuffer[1] = sizeof(simData);
	memcpy(&simBuffer[2], &simData, sizeof(simData));

	ssize_t rawBytesWritten = write(masterFd, simBuffer, sizeof(simData) + 3);
	TEST_ASSERT(rawBytesWritten == sizeof(simBuffer), "Failed to write simulated UART data");

	test_vehicle_state_t recvState;
	ssize_t bytesReceived = uart_recv_frame(uartFd, &recvState, sizeof(recvState));
	TEST_ASSERT(bytesReceived == sizeof(recvState), "Failed to receive complete UART frame");
	TEST_ASSERT(recvState.x == simData.x, "Mismatch in received x value");
	TEST_ASSERT(recvState.y == simData.y, "Mismatch in received y value");
	TEST_ASSERT(recvState.angle == simData.angle, "Mismatch in received angle value");
	TEST_ASSERT(recvState.speed == simData.speed, "Mismatch in received speed value");

	close(uartFd);
	close(masterFd);
}

