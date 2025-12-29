#ifndef SOCKET_DATA_CAMERA_H
#define SOCKET_DATA_CAMERA_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SOCKET_PORT 5000
#define MAX_JSON_SIZE 8192

typedef struct CameraSocket {
    int server_fd;
    int client_fd;
    uint16_t port;
} CameraSocket;

int initCameraSocket(CameraSocket *sock, uint16_t port);
int acceptCameraSocket(CameraSocket *sock);
int rcvCameraSocket(CameraSocket *sock, char *json_buffer, size_t buffer_size);
void closeCameraSocket(CameraSocket *sock);

#endif // SOCKET_DATA_CAMERA_H
