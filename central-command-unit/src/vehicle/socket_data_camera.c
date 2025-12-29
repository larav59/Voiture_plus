#include "vehicle/socket_data_camera.h"

int initCameraSocket(CameraSocket *sock, uint16_t port) {
    sock->port = port;

    sock->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock->server_fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(sock->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // écoute toutes les interfaces
    addr.sin_port = htons(port);

    if (bind(sock->server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(sock->server_fd, 1) < 0) {
        perror("listen");
        return -1;
    }

    printf("[C] Serveur socket en écoute sur le port %d\n", port);
    return 0;
}

int acceptCameraSocket(CameraSocket *sock) {
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    sock->client_fd = accept(sock->server_fd, (struct sockaddr *)&addr, &addrlen);
    if (sock->client_fd < 0) {
        perror("accept");
        return -1;
    }

    printf("[C] Client connecté\n");
    return 0;
}

int rcvCameraSocket(CameraSocket *sock, char *buffer, size_t max_size) {
    uint32_t msg_size;

    ssize_t r = recv(sock->client_fd, &msg_size, sizeof(msg_size), MSG_WAITALL);
    if (r <= 0) return -1;

    msg_size = ntohl(msg_size);
    if (msg_size > max_size) {
        fprintf(stderr, "[C] Message trop grand (%u octets)\n", msg_size);
        return -1;
    }

    r = recv(sock->client_fd, buffer, msg_size, MSG_WAITALL);
    if (r <= 0) return -1;

    buffer[msg_size] = '\0';
    return msg_size;
}

void closeCameraSocket(CameraSocket *sock) {
    if (sock->client_fd > 0) close(sock->client_fd);
    if (sock->server_fd > 0) close(sock->server_fd);
    printf("[C] Sockets fermées\n");
}
