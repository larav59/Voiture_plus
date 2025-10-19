/**
 * @file fifo.c
 * @brief File d'attente FIFO (First In, First Out)
 * Module fournissant une implémentation simple d'une file d'attente FIFO.
 * Implémentation basée sur le principe de liste chaînée.
 * @author Lukas Grando
 * @date 2025-10-18
 */
#include "core/common.h"
#include "core/fifo.h"
#include <stdlib.h>
#include <stdbool.h>

/**
 * @brief Initialise une file d'attente FIFO
 */
void fifo_init(fifo_t *fifo) {
	fifo->head = NULL;
	fifo->tail = NULL;
	fifo->size = 0;
}

/**
 * @brief Vérifie si la file d'attente est vide
 * @return true si la file est vide, false sinon
 */
bool fifo_is_empty(fifo_t *fifo) {
	return fifo->size == 0;
}

/**
 * @brief Ajoute un élément à la fin de la file d'attente
 * @param fifo La file d'attente
 * @param data Le pointeur vers les données à ajouter
 */
void fifo_push(fifo_t *fifo, void *data) {
	fifo_node_t *node = malloc(sizeof(fifo_node_t));
	CHECK_ALLOC_(node);
	node->data = data;
	node->next = NULL;

	if (fifo->tail) {
		fifo->tail->next = node;
	}
	fifo->tail = node;

	if (!fifo->head) {
		fifo->head = node;
	}
	fifo->size++;
}

/**
 * @brief Supprime et renvoie l'élément en tête de la file d'attente
 * @param fifo La file d'attente
 * @return Le pointeur vers les données de l'élément supprimé, ou NULL si la file est vide
 */
void *fifo_pop(fifo_t *fifo) {
    if (fifo->head == NULL) return NULL;

    fifo_node_t *node = fifo->head;
    void *data = node->data;
    fifo->head = node->next;
    if (fifo->head == NULL) fifo->tail = NULL;
    free(node);
	fifo->size--;
    return data;
}



