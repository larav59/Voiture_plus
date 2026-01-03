#include "tests/runner.h"
#include "vehicle/socket_data_camera.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

static volatile int total_objects_received = 0;

void on_camera_data_received_test(const camera_detected_object_t* objects, int count, void* context) {
    (void)context;
    (void)objects;
    total_objects_received += count;
    // afficher les objets reçus pour le debug
    for (int i = 0; i < count; i++) {
        printf("[MOCK CALLBACK] Objet reçu: catégorie=%s, confiance=%.2f, box=(x=%.2f,y=%.2f,w=%.2f,h=%.2f)\n",
            camera_category_to_string(objects[i].category),
            objects[i].confidence,
            objects[i].box.x,
            objects[i].box.y,
            objects[i].box.w,
            objects[i].box.h);
    }
}


static void send_tcp_mock_data(const char* data, int pause_microsec_mid_stream) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("[MOCK] Erreur de connexion au serveur\n");
        close(sock);
        return;
    }

    if (pause_microsec_mid_stream > 0) {
        int len = strlen(data);
        int half = len / 2;
        send(sock, data, half, 0);
        usleep(pause_microsec_mid_stream); 
        send(sock, data + half, len - half, 0);
    } else {
        // Envoi Normal
        send(sock, data, strlen(data), 0);
    }

    close(sock);
}

// --- LE TEST UNITAIRE ---

TEST_REGISTER(test_socket_camera_full, "Test Complet Socket Caméra (Normal, Fragmenté, Batch)") {
    camera_socket_t cam_sock;
    total_objects_received = 0;

    int res_init = camera_server_init(&cam_sock, "127.0.0.1", 5000, on_camera_data_received_test, NULL);
    TEST_ASSERT(res_init == 0, "L'initialisation du serveur doit réussir.");

    int res_start = camera_server_start(&cam_sock);
    TEST_ASSERT(res_start == 0, "Le démarrage du thread serveur doit réussir.");
    usleep(100000); 

    const char *json1 = "[{\"category\": \"speed_limit_30\", \"confidence\": 0.9, \"box\": {\"x\":0,\"y\":0,\"w\":10,\"h\":10}}]";
    send_tcp_mock_data(json1, 0);
    
    usleep(50000);
    TEST_ASSERT(total_objects_received == 1, "Doit avoir reçu 1 objet après envoi standard.");

    const char *json2 = "[{\"category\": \"yield\", \"confidence\": 0.8, \"box\": {\"x\":10,\"y\":10,\"w\":5,\"h\":5}}]";
    send_tcp_mock_data(json2, 50000); 

    usleep(50000);
    TEST_ASSERT(total_objects_received == 2, "Doit avoir reçu 2 objets (1+1) après envoi fragmenté.");

    const char *json3 = 
        "[{\"category\": \"railroad_crossing\", \"confidence\": 0.4, \"box\": {\"x\":0,\"y\":0,\"w\":0,\"h\":0}}]"
        "[{\"category\": \"black_line\", \"confidence\": 0.9, \"box\": {\"x\":0,\"y\":0,\"w\":0,\"h\":0}}]";
    send_tcp_mock_data(json3, 0);

    usleep(50000);
    TEST_ASSERT(total_objects_received == 4, "Doit avoir reçu 4 objets (2+1+1) après envoi batch collé.");

    camera_server_stop(&cam_sock);
    camera_server_cleanup(&cam_sock);
}