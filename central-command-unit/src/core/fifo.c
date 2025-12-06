/**
 * @file fifo.c
 * @brief File d'attente FIFO (First In, First Out)
 * Module fournissant une implémentation simple d'une file d'attente FIFO.
 * Implémentation basée sur le principe de liste chaînée.
 * @author Lukas Grando
 * @date 2025-10-18
 */
#include "core/fifo.h"

sem_t lockSem;

/**
 * @brief Initialise une file d'attente FIFO
 */
void fifo_init(fifo_t *fifo) {
	fifo->head = NULL;
	fifo->tail = NULL;
	fifo->size = 0;
	sem_init(&lockSem, 0, 1);
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
	sem_wait(&lockSem);
	fifo_node_t *node = malloc(sizeof(fifo_node_t));
	CHECK_ALLOC_RAW(node);
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
	sem_post(&lockSem);
}

/**
 * @brief Supprime et renvoie l'élément en tête de la file d'attente
 * @param fifo La file d'attente
 * @return Le pointeur vers les données de l'élément supprimé, ou NULL si la file est vide
 */
void *fifo_pop(fifo_t *fifo) {
	sem_wait(&lockSem);
    if (fifo->head == NULL) return NULL;

    fifo_node_t *node = fifo->head;
    void *data = node->data;
    fifo->head = node->next;
    if (fifo->head == NULL) fifo->tail = NULL;
    free(node);
	fifo->size--;
	sem_post(&lockSem);
    return data;
}

/**
 * @brief Libère les ressources associées à la file d'attente
 * @param fifo La file d'attente
 */
void fifo_destroy(fifo_t *fifo) {
	fifo_node_t *current = fifo->head;
	while (current) {
		fifo_node_t *next = current->next;
		free(current);
		current = next;
	}
	fifo->head = NULL;
	fifo->tail = NULL;
	fifo->size = 0;
	sem_destroy(&lockSem);
}



