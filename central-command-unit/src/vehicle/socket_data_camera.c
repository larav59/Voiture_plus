#include "vehicle/socket_data_camera.h"
#include "core/logger.h"

#define INITIAL_BUFFER_SIZE 4096

/**
 * @brief Convertit une chaîne de caractères en catégorie de caméra.
 * @param str Chaîne de caractères représentant la catégorie.
 * @return Catégorie de caméra.
 */
static camera_category_t string_to_category(const char *str) {
    if (!str) return CAM_CAT_UNKNOWN;

    if (strcmp(str, "yield") == 0) return CAM_CAT_YIELD;
    if (strcmp(str, "straight_forward") == 0) return CAM_CAT_STRAIGHT_FORWARD;
    if (strcmp(str, "priority_road") == 0) return CAM_CAT_PRIORITY_ROAD;
    if (strcmp(str, "parking") == 0) return CAM_CAT_PARKING;
    if (strcmp(str, "speed_limit_30") == 0) return CAM_CAT_SPEED_LIMIT_30;
    if (strcmp(str, "end_speed_limit_30") == 0) return CAM_CAT_END_SPEED_LIMIT_30;
    if (strcmp(str, "railroad_crossing") == 0) return CAM_CAT_RAILROAD_CROSSING;
    if (strcmp(str, "roundabout") == 0) return CAM_CAT_ROUNDABOUT;
    if (strcmp(str, "black_line") == 0) return CAM_CAT_BLACK_LINE;
    if (strcmp(str, "cars") == 0) return CAM_CAT_CARS;

    return CAM_CAT_UNKNOWN;
}

/**
 * @brief Convertit le JSON reçu en structures C et appelle le callback.
 */
static void process_json_payload(camera_socket_t *sock, const char *jsonStr) {
	cJSON *root = cJSON_Parse(jsonStr);
	if (!root) {
		LOG_WARNING_ASYNC("Camera: Failed to parse JSON payload.");
		return;
	}

	int count = 0;
	camera_detected_object_t *objects = NULL;

	if (cJSON_IsArray(root)) {
		count = cJSON_GetArraySize(root);
		if (count > 0) {
			objects = (camera_detected_object_t *)malloc(sizeof(camera_detected_object_t) * count);
			if (!objects) {
				LOG_ERROR_ASYNC("Camera: Failed to allocate memory for detected objects.");
				cJSON_Delete(root);
				return;
			}

			int idx = 0;
			cJSON *item = NULL;
			cJSON_ArrayForEach(item, root) {
				cJSON *cat = cJSON_GetObjectItem(item, "category");
				cJSON *conf = cJSON_GetObjectItem(item, "confidence");
				cJSON *box = cJSON_GetObjectItem(item, "box"); 

				if (cat && cat->valuestring) {
					objects[idx].category = string_to_category(cat->valuestring);
				} else {
					objects[idx].category = CAM_CAT_UNKNOWN;
				}

				objects[idx].confidence = (conf) ? (float)conf->valuedouble : 0.0f;

				if (box) {
					cJSON *x = cJSON_GetObjectItem(box, "x");
					cJSON *y = cJSON_GetObjectItem(box, "y");
					cJSON *w = cJSON_GetObjectItem(box, "w");
					cJSON *h = cJSON_GetObjectItem(box, "h");
					
					objects[idx].box.x = x ? (float)x->valuedouble : 0.0f;
					objects[idx].box.y = y ? (float)y->valuedouble : 0.0f;
					objects[idx].box.w = w ? (float)w->valuedouble : 0.0f;
					objects[idx].box.h = h ? (float)h->valuedouble : 0.0f;
				} else {
					memset(&objects[idx].box, 0, sizeof(camera_bounding_box_t));
				}
				idx++;
			}
		}
	} 

	sem_wait(&sock->dataSem);
	if (sock->callback && count > 0 && objects != NULL) {
		sock->callback(objects, count, sock->callbackUserData);
	}
	sem_post(&sock->dataSem);

	if (objects) free(objects);
	cJSON_Delete(root);
}

/**
 * @brief Gère la lecture du flux TCP client avec buffer dynamique.
 * @param sock Pointeur vers la structure de la socket caméra.
 */
static void handle_client_connection(camera_socket_t *sock) {
	char tempBuffer[1024];
	
	while (sock->running) {
		ssize_t bytesRead = recv(sock->clientFd, tempBuffer, sizeof(tempBuffer), 0);
		
		if (bytesRead < 0) {
			if (errno == EINTR) continue;
			LOG_ERROR_ASYNC("Camera: Recv error: %s", strerror(errno));
			break;
		}
		else if (bytesRead == 0) {
			LOG_INFO_ASYNC("Camera: Client disconnected.");
			break;
		}

		sem_wait(&sock->dataSem);

		if (sock->bufferLen + bytesRead + 1 > sock->bufferCapacity) {
			size_t newCap = sock->bufferCapacity * 2;
			if (newCap < sock->bufferLen + bytesRead + 1) {
				newCap = sock->bufferLen + bytesRead + 1024;
			}

			char *newPtr = realloc(sock->recvBuffer, newCap);
			if (!newPtr) {
				sem_post(&sock->dataSem);
				LOG_FATAL_ASYNC("Camera: Failed to reallocate receive buffer.");
				break;
			}
			sock->recvBuffer = newPtr;
			sock->bufferCapacity = newCap;
		}

		memcpy(sock->recvBuffer + sock->bufferLen, tempBuffer, bytesRead);
		sock->bufferLen += bytesRead;
		sock->recvBuffer[sock->bufferLen] = '\0';

		sem_post(&sock->dataSem);
		const char *parseEnd = NULL;
		
		while (1) {
			sem_wait(&sock->dataSem);
			if (sock->bufferLen == 0) {
				sem_post(&sock->dataSem);
				break;
			}
			
			const char *jsonStart = sock->recvBuffer;
			cJSON *testJson = cJSON_ParseWithOpts(jsonStart, &parseEnd, 0);
			
			if (testJson) {
				size_t jsonLen = parseEnd - jsonStart;
				char *validJsonStr = malloc(jsonLen + 1);
				if(validJsonStr) {
					memcpy(validJsonStr, jsonStart, jsonLen);
					validJsonStr[jsonLen] = '\0';
				}
				
				size_t remaining = sock->bufferLen - jsonLen;
				if (remaining > 0) {
					memmove(sock->recvBuffer, parseEnd, remaining);
				}
				sock->bufferLen = remaining;
				sock->recvBuffer[sock->bufferLen] = '\0';
				
				sem_post(&sock->dataSem);

				if (validJsonStr) {
					process_json_payload(sock, validJsonStr);
					free(validJsonStr);
				}
				cJSON_Delete(testJson);
			} 
			else {
				cJSON_Delete(testJson);
				sem_post(&sock->dataSem);
				break;
			}
		}
	}
}

static void* camera_thread_func(void* arg) {
	camera_socket_t *sock = (camera_socket_t*)arg;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	int opt = 1;

	if ((sock->serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		LOG_ERROR_ASYNC("Camera: Socket failed");
		return NULL;
	}

	if (setsockopt(sock->serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		LOG_ERROR_ASYNC("Camera: setsockopt failed");
		close(sock->serverFd);
		return NULL;
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(sock->port);

	if (strcmp(sock->bindIp, "0.0.0.0") == 0) {
		address.sin_addr.s_addr = INADDR_ANY;
		LOG_INFO_ASYNC("Camera: Binding to ALL interfaces (0.0.0.0)");
	} else {
		if (inet_pton(AF_INET, sock->bindIp, &address.sin_addr) <= 0) {
			LOG_ERROR_ASYNC("Camera: Invalid IP address %s", sock->bindIp);
			close(sock->serverFd);
			return NULL;
		}
		LOG_DEBUG_ASYNC("Camera: Binding to IP %s", sock->bindIp);
	}

	if (bind(sock->serverFd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		LOG_ERROR_ASYNC("Camera: Bind failed on port %d", sock->port);
		close(sock->serverFd);
		return NULL;
	}

	if (listen(sock->serverFd, 1) < 0) {
		LOG_ERROR_ASYNC("Camera: Listen failed");
		close(sock->serverFd);
		return NULL;
	}

	LOG_INFO_ASYNC("Camera: Server listening on port %d", sock->port);

	while (sock->running) {
		sock->clientFd = accept(sock->serverFd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
		
		if (sock->clientFd < 0) {
			if (sock->running) LOG_ERROR_ASYNC("Camera: Accept failed");
			continue;
		}

		LOG_INFO_ASYNC("Camera: Client connected.");
		
		sem_wait(&sock->dataSem);
		sock->bufferLen = 0;
		if(sock->recvBuffer) sock->recvBuffer[0] = '\0';
		sem_post(&sock->dataSem);

		handle_client_connection(sock);

		close(sock->clientFd);
		sock->clientFd = -1;
		LOG_INFO_ASYNC("Camera: Client disconnected/handled.");
	}

	return NULL;
}

int camera_server_init(camera_socket_t* cameraSocket, const char* ip, int port, on_objects_received_callback_t cb, void* context) {
	if (!cameraSocket) return -1;

	memset(cameraSocket, 0, sizeof(camera_socket_t));

	strncpy(cameraSocket->bindIp, ip, sizeof(cameraSocket->bindIp) - 1);
	cameraSocket->port = port;
	cameraSocket->callback = cb;
	cameraSocket->callbackUserData = context;
	
	cameraSocket->serverFd = -1;
	cameraSocket->clientFd = -1;
	cameraSocket->running = false;

	if (sem_init(&cameraSocket->dataSem, 0, 1) != 0) {
		LOG_ERROR_ASYNC("Camera: Failed to init semaphore");
		return -1;
	}

	cameraSocket->bufferCapacity = INITIAL_BUFFER_SIZE;
	cameraSocket->recvBuffer = (char*)malloc(cameraSocket->bufferCapacity);
	cameraSocket->bufferLen = 0;

	if (!cameraSocket->recvBuffer) {
		LOG_FATAL_ASYNC("Camera: Failed to allocate initial buffer");
		sem_destroy(&cameraSocket->dataSem);
		return -1;
	}

	return 0;
}

int camera_server_start(camera_socket_t* cameraSocket) {
	if (!cameraSocket) return -1;

	cameraSocket->running = true;
	int res = pthread_create(&cameraSocket->threadId, NULL, camera_thread_func, cameraSocket);
	if (res != 0) {
		LOG_ERROR_ASYNC("Camera: Failed to create thread");
		cameraSocket->running = false;
		return -1;
	}
	return 0;
}

void camera_server_stop(camera_socket_t* cameraSocket) {
	if (!cameraSocket || !cameraSocket->running) return;
	cameraSocket->running = false;

	if (cameraSocket->clientFd >= 0) {
		shutdown(cameraSocket->clientFd, SHUT_RDWR);
		close(cameraSocket->clientFd);
		cameraSocket->clientFd = -1;
	}
	if (cameraSocket->serverFd >= 0) {
		shutdown(cameraSocket->serverFd, SHUT_RDWR);
		close(cameraSocket->serverFd);
		cameraSocket->serverFd = -1;
	}

	pthread_join(cameraSocket->threadId, NULL);
	LOG_INFO_ASYNC("Camera: Server stopped.");
}

void camera_server_cleanup(camera_socket_t* cameraSocket) {
	if (!cameraSocket) return;

	camera_server_stop(cameraSocket);
	sem_destroy(&cameraSocket->dataSem);

	if (cameraSocket->recvBuffer) {
		free(cameraSocket->recvBuffer);
		cameraSocket->recvBuffer = NULL;
	}
}

/**
 * @brief Convertit une catégorie de caméra en chaîne de caractères.
 * @param cat Catégorie de la caméra.
 * @return Chaîne de caractères représentant la catégorie.
 */
const char* camera_category_to_string(camera_category_t cat) {
    switch(cat) {
        case CAM_CAT_YIELD: return "yield";
        case CAM_CAT_STRAIGHT_FORWARD: return "straight_forward";
        case CAM_CAT_PRIORITY_ROAD: return "priority_road";
        case CAM_CAT_PARKING: return "parking";
        case CAM_CAT_SPEED_LIMIT_30: return "speed_limit_30";
        case CAM_CAT_END_SPEED_LIMIT_30: return "end_speed_limit_30";
        case CAM_CAT_RAILROAD_CROSSING: return "railroad_crossing";
        case CAM_CAT_ROUNDABOUT: return "roundabout";
        case CAM_CAT_BLACK_LINE: return "black_line";
        case CAM_CAT_CARS: return "cars";
        default: return "unknown";
    }
}