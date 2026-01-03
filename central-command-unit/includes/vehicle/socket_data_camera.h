#ifndef SOCKET_DATA_CAMERA_H
#define SOCKET_DATA_CAMERA_H

#include "core/common.h"
#include "core/check.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <errno.h>


typedef struct {
    float x;
    float y;
    float w;
    float h;
} camera_bounding_box_t;

typedef struct {
    char category[255];
    float confidence;
    camera_bounding_box_t box;
} camera_detected_object_t;

typedef void (*on_objects_received_callback_t)(const camera_detected_object_t* objects, int count, void* context);

typedef struct {
    int serverFd;
    int clientFd;
    int port;
    char bindIp[16];
    
    volatile bool running;
    pthread_t threadId;
    sem_t dataSem;

    char *recvBuffer;
    size_t bufferLen;
    size_t bufferCapacity;

    on_objects_received_callback_t callback;
    void* callbackUserData;

} camera_socket_t;

/**
 * @brief Initialise le serveur socket pour la caméra.
 * @param cameraSocket Pointeur vers la structure de la socket caméra.
 * @param ip Adresse IP à lier
 * @param port Port à écouter
 * @param cb Callback à appeler lors de la réception d'objets détectés.
 * @param context Contexte utilisateur à passer au callback.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int camera_server_init(camera_socket_t* cameraSocket, const char* ip, int port, on_objects_received_callback_t cb, void* context);

/**
 * @brief Démarre le serveur socket de la caméra dans un thread séparé.
 * @param cameraSocket Pointeur vers la structure de la socket caméra.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int camera_server_start(camera_socket_t* cameraSocket);

/**
 * @brief Arrête le serveur socket de la caméra et attend la fin du thread.
 * @param cameraSocket Pointeur vers la structure de la socket caméra.
 */
void camera_server_stop(camera_socket_t* cameraSocket);

/**
 * @brief Nettoie les ressources allouées pour la socket caméra.
 * @param cameraSocket Pointeur vers la structure de la socket caméra.
 */
void camera_server_cleanup(camera_socket_t* cameraSocket);

#endif // SOCKET_DATA_CAMERA_H