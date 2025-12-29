#include "tests/runner.h"
#include "vehicle/socket_data_camera.h"
#include "vehicle/local_decision.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

TEST_REGISTER(test_socket_camera, "Test réception JSON depuis Python") {
    CameraSocket cam_sock;
    char buffer[MAX_JSON_SIZE];

    // Initialisation du serveur socket
    if (initCameraSocket(&cam_sock, 5000) < 0) {
        printf("[C TEST] Impossible d'initialiser le serveur socket\n");
        return;
    }

    // Attente de connexion du client Python
    if (acceptCameraSocket(&cam_sock) < 0) {
        printf("[C TEST] Impossible d'accepter le client\n");
        closeCameraSocket(&cam_sock);
        return;
    }

    printf("[C TEST] Serveur prêt, en attente de JSON...\n");

    // Boucle de réception d'un ou plusieurs messages JSON
    while (1) {
        int received = rcvCameraSocket(&cam_sock, buffer, MAX_JSON_SIZE);
        if (received <= 0) {
            printf("[C TEST] Client déconnecté ou erreur de réception\n");
            break;
        }

        // Terminer correctement la chaîne pour strstr
        buffer[received] = '\0';

        printf("\n[C TEST] JSON reçu (%d octets):\n%s\n", received, buffer);

        // --- APPEL DU SWITCH ---
        handleCategory(buffer);
        printf("[C TEST] Fin du traitement du JSON\n");
    }

    // Fermeture des sockets
    closeCameraSocket(&cam_sock);
}

