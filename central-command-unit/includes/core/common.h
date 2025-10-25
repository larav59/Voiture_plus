/**
 * @file common.h
 * @brief Common header file for all services
 * Contient les macros de debug, d'erreur et utilitaires pour l'ensemble des services MQTT.
 * @version 1.1
 * @author Lukas
 */

#ifndef COMMON_H
#define COMMON_H
#define _GNU_SOURCE

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>


#ifdef DEBUG
    #define UNUSED(x) (void)(x) //!< Supprime les warnings pour variables non utilisées
    #define UNIMPLEMENTED(func) fprintf(stderr, "[DEBUG] Unimplemented: %s\n", func) //!< Fonction non implémentée
    #define OBSOLETE(func)      fprintf(stderr, "[DEBUG] Obsolete: %s\n", func) //!< Fonction obsolète
    #define DEBUG_PRINT(...)    fprintf(stderr, "[DEBUG] " __VA_ARGS__) //!< Affichage debug
#else
    #define UNUSED(x) (void)(x) //!< Supprime les warnings pour variables non utilisées
    #define UNIMPLEMENTED(func)
    #define OBSOLETE(func)
    #define DEBUG_PRINT(...)
#endif


#define ERROR(...) fprintf(stderr, "[ERROR] " __VA_ARGS__) //!< Affichage d'erreur


// Vérifications critiques
#define CHECK_CRITICAL_RAW(cond, msg) \
    do { \
        if (!(cond)) { \
            ERROR("%s at %s:%d\n", msg, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)

// Allocation mémoire
#define CHECK_ALLOC_RAW(ptr) CHECK_CRITICAL_RAW((ptr) != NULL, "Memory allocation failed")

// POSIX semaphore
#define CHECK_SEM_RAW(ret) CHECK_CRITICAL_RAW((ret) == 0, "Semaphore error")

// POSIX pthread
#define CHECK_PTHREAD_RAW(ret) CHECK_CRITICAL_RAW((ret) == 0, "Pthread error")




// Couleurs pour la sortie console
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_RESET   "\033[0m"



#endif // COMMON_H
