/**
 * @file priority_queue.c
 * @brief Définitions de la file de priorité utilisée dans les algorithmes de graphe.
 * @date 2025-11-05
 */

#include "core/priority_queue.h"

/**
 * @brief Échange deux éléments dans la file de priorité.
 * @param a Pointeur vers le premier élément.
 * @param b Pointeur vers le second élément.
 */
static void swap_elements(pq_element_t* a, pq_element_t* b) {
    pq_element_t temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief Fait remonter un élément à sa place correcte dans le tas.
 * @internal
 */
static void heapify_up(priority_queue_t* pq, int index) {
    int parentIndex = (index - 1) / 2;
    while (index > 0 && pq->elements[index].priority < pq->elements[parentIndex].priority) {
        swap_elements(&pq->elements[index], &pq->elements[parentIndex]);
        index = parentIndex;
        parentIndex = (index - 1) / 2;
    }
}

/**
 * @brief Fait descendre un élément à sa place correcte dans le tas.
 * @internal
 */
static void heapify_down(priority_queue_t* pq, int index) {
    int leftChild, rightChild, smallest;
    while (1) {
        leftChild = 2 * index + 1;
        rightChild = 2 * index + 2;
        smallest = index; // On suppose que le parent est le plus petit

        // Si l'enfant de gauche existe et est plus petit que le parent
        if (leftChild < pq->size && pq->elements[leftChild].priority < pq->elements[smallest].priority) {
            smallest = leftChild;
        }
        // Si l'enfant de droite existe et est plus petit que le plus petit actuel
        if (rightChild < pq->size && pq->elements[rightChild].priority < pq->elements[smallest].priority) {
            smallest = rightChild;
        }

        // Si l'index du plus petit n'a pas changé, le tas est ordonné
        if (smallest == index) {
            break; 
        }
        
        // Sinon, on échange et on continue à descendre
        swap_elements(&pq->elements[index], &pq->elements[smallest]);
        index = smallest;
    }
}

/** 
 * @brief Crée une file de priorité avec une capacité donnée. 
 * @param capacity La capacité initiale de la file.
 * @return Pointeur vers la file de priorité créée, ou NULL en cas d'erreur.
 */
priority_queue_t* pq_create(int capacity) {
	priority_queue_t *pq = (priority_queue_t *)malloc(sizeof(priority_queue_t));
	if(pq == NULL) {
		return NULL;
	}

	pq->elements = (pq_element_t *)malloc(sizeof(pq_element_t) * capacity);
	if(pq->elements == NULL) {
		free(pq);
		return NULL;
	}

	pq->capacity = capacity;
	pq->size = 0;

	return pq;
}

/**
 * @brief Détruit la file de priorité et libère toute la mémoire associée.
 * @param pq La file de priorité à détruire.
 */
void pq_destroy(priority_queue_t* pq) {
	if(pq != NULL) {
		free(pq->elements);
		free(pq);
	}
}

/**
 * @brief Ajoute un item à la file avec une priorité donnée.
 * @param pq La file de priorité.
 * @param item L'item à ajouter.
 * @param priority La priorité de l'item.
 */
bool pq_push(priority_queue_t* pq, void* item, int priority) {
	if(pq->size >= pq->capacity) {
		int newCapacity = pq->capacity * 2;
		pq_element_t* newElements = (pq_element_t *)realloc(pq->elements, sizeof(pq_element_t) * newCapacity);
		if(newElements == NULL) {
			return false;
		}
		pq->elements = newElements;
		pq->capacity = newCapacity;
	}

	pq->elements[pq->size].item = item;
	pq->elements[pq->size].priority = priority;
	pq->size++;
	heapify_up(pq, pq->size - 1);

	return true;
}

/**
 * @brief Retire et retourne l'item avec la plus haute priorité (la plus faible valeur).
 * @param pq La file de priorité.
 * @return L'item avec la plus haute priorité, ou NULL si la file est vide.
 */
void* pq_pop(priority_queue_t* pq) {
	if(pq->size == 0) {
		return NULL;
	}

	void* topItem = pq->elements[0].item;
	pq->elements[0] = pq->elements[pq->size - 1];
	pq->size--;
	heapify_down(pq, 0);

	return topItem;
}

/**
 * @brief Vérifie si la file de priorité est vide.
 * @param pq La file de priorité.
 * @return true si la file est vide, false sinon.
 */
bool pq_is_empty(priority_queue_t* pq) {
	return pq->size == 0;
}