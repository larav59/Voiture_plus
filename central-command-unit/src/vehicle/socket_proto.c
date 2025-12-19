/**
 * @file 
 * @brief Protocole de communication IPC pour le serveur IEC.
 * @details Ce fichier définit les structures et les types utilisés pour la communication entre le serveur IEC et les clients via sockets Unix.
 * @author Lukas Grando
 */

#include "vehicle/socket_proto.h"


#define BUFFER_SIZE 4096

static int g_server_fd = -1; 
static volatile bool g_running = false;
static pthread_t g_thread;
static proto_handler_t g_callback = NULL;
static char g_socket_path[PROTO_MAX_SOCKET_PATH_LEN];

/**
 * @brief Convertit une chaine de caractère en type de commande.
 * @param cmd La chaine de caractère représentant la commande.
 * @return Le type de commande correspondant.
 */
static proto_command_type_t str_to_cmd(const char* cmd) {
    if (!cmd) return PROTO_CMD_UNKNOWN;
    if (strcmp(cmd, "SIM_ON") == 0) return PROTO_CMD_SIM_ON;
    if (strcmp(cmd, "SIM_OFF") == 0) return PROTO_CMD_SIM_OFF;
    if (strcmp(cmd, "STATUS") == 0) return PROTO_CMD_STATUS;
    return PROTO_CMD_UNKNOWN;
}

static void* server_worker(void* arg) {
    (void)arg;
    struct sockaddr_un client_addr;
    socklen_t len = sizeof(client_addr);

    while (g_running) {
        int client_fd = accept(g_server_fd, (struct sockaddr*)&client_addr, &len);
        
        if (client_fd < 0) {
            if (g_running && errno != EINVAL) {
                LOG_ERROR("Accept failed: %s", strerror(errno));
            }
            continue; 
        }

        char buffer[BUFFER_SIZE] = {0};
        ssize_t n = read(client_fd, buffer, sizeof(buffer) - 1);

        if (n > 0) {
            buffer[n] = '\0';

            cJSON *json_req = cJSON_Parse(buffer);
            cJSON *json_resp = cJSON_CreateObject();

            if (!json_req) {
                cJSON_AddStringToObject(json_resp, "status", "error");
                cJSON_AddStringToObject(json_resp, "msg", "Invalid JSON format");
            } else {
                proto_msg_t msg = {0};
                
                cJSON *j_cmd = cJSON_GetObjectItem(json_req, "cmd");
                cJSON *j_iec = cJSON_GetObjectItem(json_req, "iec"); 
                cJSON *j_val = cJSON_GetObjectItem(json_req, "val");

                if (cJSON_IsString(j_cmd)) {
                    msg.type = str_to_cmd(j_cmd->valuestring);
                }

                if (cJSON_IsString(j_iec)) {
                    strncpy(msg.iec_address, j_iec->valuestring, PROTO_MAX_STR_LEN - 1);
                    msg.iec_address[PROTO_MAX_STR_LEN - 1] = '\0';
                }

                if (cJSON_IsString(j_val)) {
                    strncpy(msg.value, j_val->valuestring, PROTO_MAX_STR_LEN - 1);
                } else if (cJSON_IsNumber(j_val)) {
                    snprintf(msg.value, PROTO_MAX_STR_LEN, "%.4f", j_val->valuedouble);
                }
                msg.value[PROTO_MAX_STR_LEN - 1] = '\0';

                if (g_callback) {
                    g_callback(&msg, (void*)json_resp);
                } else {
                    cJSON_AddStringToObject(json_resp, "status", "error");
                    cJSON_AddStringToObject(json_resp, "msg", "No handler defined");
                }
                
                cJSON_Delete(json_req);
            }

            char *resp_str = cJSON_PrintUnformatted(json_resp);
            if (resp_str) {
                write(client_fd, resp_str, strlen(resp_str));
                free(resp_str);
            }
            cJSON_Delete(json_resp);
        }
        close(client_fd);
    }
    return NULL;
}

int proto_server_start(const proto_config_t *config, proto_handler_t handler) {
    if (g_running) return 0; 
    if (!config || !handler) return -1;

    g_callback = handler;

    strncpy(g_socket_path, config->socket_path, sizeof(g_socket_path) - 1);
    g_socket_path[sizeof(g_socket_path) - 1] = '\0';
    
    int backlog = (config->max_clients > 0) ? config->max_clients : 5;

    unlink(g_socket_path);

    g_server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (g_server_fd < 0) {
        LOG_ERROR("Socket creation failed");
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, g_socket_path, sizeof(addr.sun_path) - 1);

    if (bind(g_server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        LOG_ERROR("Bind failed");
        close(g_server_fd);
        return -1;
    }

    if (listen(g_server_fd, backlog) == -1) {
        LOG_ERROR("Listen failed");
        close(g_server_fd);
        return -1;
    }

    g_running = true;
    if (pthread_create(&g_thread, NULL, server_worker, NULL) != 0) {
        LOG_ERROR("Thread creation failed");
        g_running = false;
        close(g_server_fd);
        return -1;
    }

    LOG_INFO("Serveur IPC démarré sur : %s", g_socket_path);
    return 0;
}

void proto_server_stop(void) {
    if (!g_running) return;
    
    LOG_INFO("Arrêt du serveur IPC...");
    g_running = false;

    shutdown(g_server_fd, SHUT_RDWR);
    close(g_server_fd);
    pthread_join(g_thread, NULL);
    unlink(g_socket_path);
    
    LOG_INFO("Serveur IPC arrêté.");
}