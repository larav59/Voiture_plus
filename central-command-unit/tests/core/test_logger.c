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

static int syncCalled = 0;
static char lastSyncMsg[LOG_MESSAGE_LENGTH];

static void testSyncCallback(log_level_t level, const char *msg) {
    UNUSED(level);
    strncpy(lastSyncMsg, msg, LOG_MESSAGE_LENGTH);
    syncCalled = 1;
}

TEST_REGISTER(test_logger_sync, "Test du logger synchrone avec callback") {
    logger_init(LOG_LEVEL_DEBUG, testSyncCallback);

    syncCalled = 0;
    LOG_DEBUG_SYNC("Hello %s", "World");

    TEST_ASSERT(syncCalled == 1, "Le callback sync doit être appelé");
    TEST_ASSERT(strcmp(lastSyncMsg, "Hello World") == 0, "Message logué incorrect");

    logger_destroy();
}

#define MAX_ASYNC_MSGS 5
static char asyncMsgs[MAX_ASYNC_MSGS][LOG_MESSAGE_LENGTH];
static int asyncCount = 0;
static pthread_mutex_t asyncMutex = PTHREAD_MUTEX_INITIALIZER;

static void testAsyncCallback(log_level_t level, const char *msg) {
    UNUSED(level);
    pthread_mutex_lock(&asyncMutex);
    if(asyncCount < MAX_ASYNC_MSGS) {
        strncpy(asyncMsgs[asyncCount], msg, LOG_MESSAGE_LENGTH);
        asyncCount++;
    }
    pthread_mutex_unlock(&asyncMutex);
}

TEST_REGISTER(test_logger_async, "Test du logger asynchrone avec FIFO et callback") {
    logger_init(LOG_LEVEL_DEBUG, testAsyncCallback);

    pthread_mutex_lock(&asyncMutex);
    asyncCount = 0;
    pthread_mutex_unlock(&asyncMutex);

    LOG_INFO_ASYNC("Message 1");
    LOG_WARNING_ASYNC("Message 2");
    LOG_ERROR_ASYNC("Message 3");

    // Petit délai pour laisser le thread traiter les messages
    struct timespec ts = {0, 100*1000*1000}; // 100 ms
    nanosleep(&ts, NULL);

    TEST_ASSERT(asyncCount == 3, "3 messages doivent être traités par le callback async");
    TEST_ASSERT(strcmp(asyncMsgs[0], "Message 1") == 0, "Message 1 incorrect");
    TEST_ASSERT(strcmp(asyncMsgs[1], "Message 2") == 0, "Message 2 incorrect");
    TEST_ASSERT(strcmp(asyncMsgs[2], "Message 3") == 0, "Message 3 incorrect");

    logger_destroy();
}


TEST_REGISTER(test_logger_level_filter, "Test que le logger ignore les messages sous le niveau") {
    logger_init(LOG_LEVEL_WARNING, testSyncCallback);

    syncCalled = 0;
    LOG_INFO_SYNC("Ignored message");
    LOG_WARNING_SYNC("Valid message");

    TEST_ASSERT(syncCalled == 1, "Seul le message au niveau WARNING doit passer");
    TEST_ASSERT(strcmp(lastSyncMsg, "Valid message") == 0, "Message logué incorrect");

    logger_destroy();
}

TEST_REGISTER(test_logger_reinit, "Test réinit du logger après destruction") {
    // Init initiale
    logger_init(LOG_LEVEL_DEBUG, testSyncCallback);
    syncCalled = 0;
    LOG_INFO_SYNC("First init");
    TEST_ASSERT(syncCalled == 1, "Callback doit être appelé la première fois");
    
    logger_destroy();

    // Réinitialisation
    logger_init(LOG_LEVEL_DEBUG, testSyncCallback);
    syncCalled = 0;
    LOG_INFO_SYNC("Second init");
    TEST_ASSERT(syncCalled == 1, "Callback doit être appelé après réinit");

    logger_destroy();
}

TEST_REGISTER(test_logger_truncate, "Test que les messages trop longs sont tronqués") {
    logger_init(LOG_LEVEL_DEBUG, testSyncCallback);

    char longMsg[LOG_MESSAGE_LENGTH * 2];
    for(int i = 0; i < (int)sizeof(longMsg)-1; i++) longMsg[i] = 'A';
    longMsg[sizeof(longMsg)-1] = '\0';

    syncCalled = 0;
    LOG_INFO_SYNC("%s", longMsg);

    TEST_ASSERT(syncCalled == 1, "Callback doit être appelé pour message long");
    TEST_ASSERT(strlen(lastSyncMsg) < LOG_MESSAGE_LENGTH, "Message doit être tronqué correctement");

    logger_destroy();
}