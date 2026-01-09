#ifndef SOCKET_DATA_CAMERA_H
#define SOCKET_DATA_CAMERA_H

#include "core/common.h"
#include "core/check.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <errno.h>

typedef enum {
    CAM_CAT_UNKNOWN = 0,
    CAM_CAT_YIELD,
    CAM_CAT_STRAIGHT_FORWARD,
    CAM_CAT_PRIORITY_ROAD,
    CAM_CAT_PARKING,
    CAM_CAT_SPEED_LIMIT_30,
    CAM_CAT_END_SPEED_LIMIT_30,
    CAM_CAT_RAILROAD_CROSSING,
    CAM_CAT_ROUNDABOUT,
    CAM_CAT_BLACK_LINE,
    CAM_CAT_CARS
} camera_category_t;

typedef struct {
    float x;
    float y;
    float w;
    float h;
} camera_bounding_box_t;

typedef struct {
    camera_category_t category;
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

/**
 * @brief Convertit une catégorie de caméra en chaîne de caractères.
 * @param cat Catégorie de la caméra.
 * @return Chaîne de caractères représentant la catégorie.
 */
const char* camera_category_to_string(camera_category_t cat);

#endif // SOCKET_DATA_CAMERA_H