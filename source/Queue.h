/*
 * queue.h
 *
 *  Created on: 5 nov 2024
 *      Author: asolari
 */

#ifndef QUEUE_H_
#define QUEUE_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define BUFFER_SIZE 1000

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef uint8_t queuedata_t; // Queuedata podria ser en principio 8 bits. Cambiarlo despues de ver que funciona
typedef struct Queue
{
  queuedata_t buffer[BUFFER_SIZE + 1];
  uint16_t front;
  uint16_t tail;
  bool isFull;
  bool isEmpty;
} queue_t;



/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Get the first element of the queue
 *
 * @param queue Pointer to the queue
 * @return The first element of the queue
 */
queuedata_t getNext(queue_t *queue);

/**
 * @brief Get the specified number of elements from the front of the queue
 *
 * @param queue Pointer to the queue
 * @param data Pointer to the array where the elements will be stored
 * @param dataAmount Number of elements to retrieve
 * @return The number of elements actually retrieved
 */
uint32_t getNextArray(queue_t *queue, queuedata_t *data, uint32_t dataAmount);

/**
 * @brief Add a new element at the end of the queue
 *
 * @param queue Pointer to the queue
 * @param data The element to add
 * @return true if the element was added successfully, false otherwise
 */
bool put(queue_t *queue, queuedata_t data);

/**
 * @brief Add multiple elements at the end of the queue
 *
 * @param queue Pointer to the queue
 * @param data Pointer to the array of elements to add
 * @param dataAmount Number of elements to add
 * @return The number of elements actually added
 */
uint32_t putArray(queue_t *queue, queuedata_t *data, uint32_t dataAmount);

/**
 * @brief Get the last element of the queue
 *
 * @param queue Pointer to the queue
 * @return The last element of the queue
 */
queuedata_t pop(queue_t *queue);

/**
 * @brief Get the number of elements currently in the queue
 *
 * @param queue Pointer to the queue
 * @return The number of elements in the queue
 */
uint32_t getFillLevel(queue_t *queue);

/**
 * @brief Return the i-th value of the queue, where 0 is the first position in the queue corresponding to the front of the queue.
 *
 * @param queue Pointer to the queue
 * @param index Position in the queue where the value is stored
 * @return The value at the i-th position. If the index is out of bounds, return 0
 */
queuedata_t readValue(queue_t *queue, uint16_t index);



#endif /* QUEUE_H_ */
