/**
 * @file priority_queue.h
 * @brief Définitions de la file de priorité utilisée dans les algorithmes de graphe.
 * @date 2025-11-05
 */
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "core/common.h"
#include "core/check.h"

/**
 * @brief Un élément dans la file de priorité.
 * @details Stocke un item générique (void*) et sa priorité.
 * @note La priorité est un entier
 */
typedef struct {
    void* item;
    int priority;
} pq_element_t;

/**
 * @brief Structure de la file de priorité.
 * @details Implémentée en utilisant un tableau dynamique.
 * Chaque élément du tableau est un pq_element_t.
 * La taille du tableau est ajustée dynamiquement en fonction des besoins.
 * @note La file de priorité est un min-heap (l'élément avec la plus faible priorité est en tête).
 */
typedef struct {
    pq_element_t* elements;
    int capacity;
    int size;
} priority_queue_t;

/** 
 * @brief Crée une file de priorité avec une capacité donnée. 
 * @param capacity La capacité initiale de la file.
 * @return Pointeur vers la file de priorité créée, ou NULL en cas d'erreur.
 */
priority_queue_t* pq_create(int capacity);

/**
 * @brief Détruit la file de priorité et libère toute la mémoire associée.
 * @param pq La file de priorité à détruire.
 */
void pq_destroy(priority_queue_t* pq);

/**
 * @brief Ajoute un item à la file avec une priorité donnée.
 * @param pq La file de priorité.
 * @param item L'item à ajouter.
 * @param priority La priorité de l'item.
 */
bool pq_push(priority_queue_t* pq, void* item, int priority);

/**
 * @brief Retire et retourne l'item avec la plus haute priorité (la plus faible valeur).
 * @param pq La file de priorité.
 * @return L'item avec la plus haute priorité, ou NULL si la file est vide.
 */
void* pq_pop(priority_queue_t* pq);

/**
 * @brief Vérifie si la file de priorité est vide.
 * @param pq La file de priorité.
 * @return true si la file est vide, false sinon.
 */
bool pq_is_empty(priority_queue_t* pq);

#endif // PRIORITY_QUEUE_H