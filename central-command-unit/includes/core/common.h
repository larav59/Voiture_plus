/**
 * @file common.h
 * @brief Common header file for all services
 * Contient les macros de debug, d'erreur et utilitaires pour l'ensemble des services MQTT.
 * @version 1.1
 * @author Lukas
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
#define CHECK_CRITICAL(cond, msg) \
    do { \
        if (!(cond)) { \
            ERROR("%s at %s:%d\n", msg, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)

// Allocation mémoire
#define CHECK_ALLOC(ptr) CHECK_CRITICAL((ptr) != NULL, "Memory allocation failed")

// POSIX semaphore
#define CHECK_SEM(ret) CHECK_CRITICAL((ret) == 0, "Semaphore error")

// POSIX pthread
#define CHECK_PTHREAD(ret) CHECK_CRITICAL((ret) == 0, "Pthread error")




// Couleurs pour la sortie console
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_RESET   "\033[0m"



#endif // COMMON_H
