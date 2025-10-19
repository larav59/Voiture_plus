/**
 * @file core.h
 * @brief 
 * @author Lukas Grando
 * @date 2025-10-19
 */

 #ifndef CCU_CORE_H
 #define CCU_CORE_H

#include "core/logger.h"

extern bool loggerInitialized;


#define CHECK_CRITICAL(cond, msg) \
    do { \
        if (!(cond)) { \
            if (loggerInitialized) { \
                LOG_ERROR_SYNC("%s at %s:%d", msg, __FILE__, __LINE__); \
            } else { \
                CHECK_CRITICAL_RAW(cond, msg); \
            } \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

#define CHECK_ALLOC(ptr)  CHECK_CRITICAL((ptr) != NULL, "Allocation failed")
#define CHECK_SEM(res)    CHECK_CRITICAL((res) == 0, "Semaphore operation failed")
#define CHECK_PTHREAD(res) CHECK_CRITICAL((res) == 0, "Thread operation failed")

 #endif //CCU_CORE_H