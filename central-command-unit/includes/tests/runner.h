/**
 * @file test.h
 * @brief Module pour faciliter la création de tests unitaires.
 * @author Lukas
 * @date 2025-10-18
 */
#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include "core/common.h"

/**
 * @typedef test_function_t
 * @brief Type de fonction de test unitaire
 * Un test unitaire est une fonction qui ne prend pas d'arguments et ne retourne rien.
 * Elle utilise des assertions internes pour valider le comportement du code testé.
 */
typedef void (*test_function_t)(void);

/**
 * @typedef test_entry_t
 * @brief Structure représentant une entrée de test unitaire
 * Contient le nom du test et un pointeur vers la fonction de test.
 */
typedef struct {
    const char *name;          /**< Nom du test */
	const char *description;   /**< Description du test */
    test_function_t function; /**< Pointeur vers la fonction de test */
} test_entry_t;


/**
 * @brief Enregistre un test unitaire
 * @param name Nom du test
 * @param function Pointeur vers la fonction de test
 */
void test_register(const char *name, test_function_t function, const char *description);

/**
 * @brief Exécute tous les tests enregistrés
 * @note Affiche les résultats des tests sur la sortie standard.
 */
void test_run(void);

/**
 * @brief Macro pour enregistrer un test unitaire
 * @param fn Nom de la fonction de test
 * @param desc Description du test
 * Pour information :
 * - La macro déclare la signature de la fonction de test
 * - Crée une fonction qui est ensuite exécutée avant le main() grâce à l'attribut constructor.
 * - Puis on définit la fonction de test elle-même.
 * 
 * @example 
 * ```
 * REGISTER_TEST(test_example, "Exemple de test unitaire") {
 *    // Code du test ici
 * }
 * ```
 */
#define TEST_REGISTER(fn, desc) \
    static void fn(void); \
    __attribute__((constructor)) static void register_##fn(void) { test_register(#fn, fn, desc); } \
    static void fn(void)


/**
 * @brief Macro d'assertion pour les tests unitaires
 * @param cond Condition à vérifier
 * @param msg Message à afficher en cas d'échec
 * Affiche un message de succès ou d'échec selon le résultat de la condition.
 * Utilise les couleurs ANSI pour une meilleure lisibilité.
 * @example
 * ```
 * TEST_ASSERT(value == expected, "Vérification de la valeur attendue");
 * ```
 */
#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, COLOR_RED"[FAIL] %s (at %s:%d)"COLOR_RESET"\n", msg, __FILE__, __LINE__); \
        } else { \
            fprintf(stdout, COLOR_GREEN"[PASS] %s"COLOR_RESET"\n", msg); \
        } \
    } while(0)

#endif // TEST_H