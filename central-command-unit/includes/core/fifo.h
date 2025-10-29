/**
 * @file fifo.h
 * @brief File d'attente FIFO (First In, First Out)
 * Module fournissant une implémentation simple d'une file d'attente FIFO.
 * Implémentation basée sur le principe de liste chaînée.
 * @author Lukas Grando
 * @date 2025-10-18
 */

#ifndef FIFO_H
#define FIFO_H

typedef struct fifo_node {
	void *data;
	struct fifo_node *next;
} fifo_node_t;

typedef struct {
	fifo_node_t *head;
	fifo_node_t *tail;
	size_t size;
} fifo_t;

/**
 * @brief Initialise une file d'attente FIFO
 */
void fifo_init(fifo_t *fifo);

/**
 * @brief Vérifie si la file d'attente est vide
 * @return true si la file est vide, false sinon
 */
bool fifo_is_empty(fifo_t *fifo);

/**
 * @brief Ajoute un élément à la fin de la file d'attente
 * @param fifo La file d'attente
 * @param data Le pointeur vers les données à ajouter
 */
void fifo_push(fifo_t *fifo, void *data);

/**
 * @brief Supprime et renvoie l'élément en tête de la file d'attente
 * @param fifo La file d'attente
 * @return Le pointeur vers les données de l'élément supprimé, ou NULL si la file est vide
 */
void *fifo_pop(fifo_t *fifo);

/**
 * @brief Libère les ressources associées à la file d'attente
 * @param fifo La file d'attente
 */

#endif // FIFO_H


