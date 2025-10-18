/**
 * @file test_logger.c
 * @brief Tests unitaires pour le module de journalisation (logger)
 * @author Lukas
 * @date 2025-10-18
 */

#include "tests/runner.h"
#include "core/logger.h"
#include "core/fifo.h"
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

static int sync_called = 0;
static char last_sync_msg[LOG_MESSAGE_LENGTH];

static void test_sync_callback(log_level_t level, const char *msg) {
    UNUSED(level);
    strncpy(last_sync_msg, msg, LOG_MESSAGE_LENGTH);
    sync_called = 1;
}

// Test logger sync simple
TEST_REGISTER(test_logger_sync, "Test du logger synchrone avec callback") {
    logger_init_sync(LOG_LEVEL_DEBUG, test_sync_callback);
    sync_called = 0;
    LOG_DEBUG_SYNC("Hello %s", "World");

    TEST_ASSERT(sync_called == 1, "Le callback sync doit être appelé");
    TEST_ASSERT(strcmp(last_sync_msg, "Hello World") == 0, "Message logué incorrect");
}

// Mode async : callback pour récupérer les messages
#define MAX_ASYNC_MSGS 5
static char async_msgs[MAX_ASYNC_MSGS][LOG_MESSAGE_LENGTH];
static int async_count = 0;
static pthread_mutex_t async_mutex = PTHREAD_MUTEX_INITIALIZER;

static void test_async_callback(log_level_t level, const char *msg) {
    UNUSED(level);
    pthread_mutex_lock(&async_mutex);
    if(async_count < MAX_ASYNC_MSGS) {
        strncpy(async_msgs[async_count], msg, LOG_MESSAGE_LENGTH);
        async_count++;
    }
    pthread_mutex_unlock(&async_mutex);
}

// Test logger async simple
TEST_REGISTER(test_logger_async, "Test du logger asynchrone avec FIFO et callback") {
    logger_init_async(LOG_LEVEL_DEBUG, test_async_callback);
    pthread_mutex_lock(&async_mutex);
    async_count = 0;
    pthread_mutex_unlock(&async_mutex);

    LOG_INFO_ASYNC("Message 1");
    LOG_WARNING_ASYNC("Message 2");
    LOG_ERROR_ASYNC("Message 3");

    // Petit délai pour laisser le thread traiter les messages
    struct timespec ts = {0, 100*1000*1000}; // 100 ms
    nanosleep(&ts, NULL);

    TEST_ASSERT(async_count == 3, "3 messages doivent être traités par le callback async");
    TEST_ASSERT(strcmp(async_msgs[0], "Message 1") == 0, "Message 1 incorrect");
    TEST_ASSERT(strcmp(async_msgs[1], "Message 2") == 0, "Message 2 incorrect");
    TEST_ASSERT(strcmp(async_msgs[2], "Message 3") == 0, "Message 3 incorrect");
}

// Test filtrage selon log level
TEST_REGISTER(test_logger_level_filter, "Test que le logger ignore les messages sous le niveau") {
    logger_init_sync(LOG_LEVEL_WARNING, test_sync_callback);
    sync_called = 0;
    LOG_INFO_SYNC("Ignored message");
    LOG_WARNING_SYNC("Valid message");

    TEST_ASSERT(sync_called == 1, "Seul le message au niveau WARNING doit passer");
    TEST_ASSERT(strcmp(last_sync_msg, "Valid message") == 0, "Message logué incorrect");
}