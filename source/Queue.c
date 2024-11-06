/*
 * queue.c
 *
 *  Created on: 5 nov 2024
 *      Author: asolari
 */

#include "Queue.h"

queuedata_t getNext(queue_t *queue) {
	// If the queue is empty, return 0
	if (queue->tail == queue->front) {
		return 0;
	}
	// If the queue was full, mark it as not full
	else if (queue->isFull) {
		queue->isFull = false;
	}

	// Store the data at the front of the queue in a temporary variable
	queuedata_t aux_event = queue->buffer[queue->front];
	// Move the front pointer to the next position, wrapping around if necessary
	queue->front = (queue->front == BUFFER_SIZE - 1) ? 0 : queue->front + 1;

	// If the queue is now empty, mark it as empty
	if (queue->tail == queue->front)
		queue->isEmpty = true;

	// Return the data that was at the front of the queue
	return aux_event;
}

queuedata_t pop(queue_t *queue) {
	// If the queue is empty, return 0
	if (queue->tail == queue->front) {
		return 0;
	}
	// If the queue was full, mark it as not full
	else if (queue->isFull) {
		queue->isFull = false;
	}

	// Move the tail pointer to the previous position, wrapping around if necessary
	queue->tail = (queue->tail == 0) ? BUFFER_SIZE - 1 : queue->tail - 1;

	// If the queue is now empty, mark it as empty
	if (queue->tail == queue->front)
		queue->isEmpty = true;

	// Return the data that was at the tail of the queue
	return queue->buffer[queue->tail];
}

bool put(queue_t *queue, queuedata_t data) {
	// If the queue was empty, mark it as not empty
	if (queue->tail == queue->front) {
		queue->isEmpty = false;
	}
	// If the queue is full, do not add the data and return false
	else if (queue->isFull) {
		return false;
	}

	// Add the data to the tail of the queue
	queue->buffer[queue->tail] = data;
	// Move the tail pointer to the next position, wrapping around if necessary
	queue->tail = (queue->tail == BUFFER_SIZE - 1) ? 0 : queue->tail + 1;

	// If the queue is now full, mark it as full
	if ((queue->tail == (queue->front - 1))
			|| (queue->front == 0 && queue->tail == (BUFFER_SIZE - 1))) {
		queue->isFull = true;
	}

	// Return true to indicate the data was added successfully
	return true;
}

uint32_t getNextArray(queue_t *queue, queuedata_t *data, uint32_t dataAmount) {
	// If the queue is empty, return 0
	if (queue->isEmpty) {
		return 0;
	}

	uint32_t i = 0;
	// Retrieve dataAmount elements from the queue or until the queue is empty
	for (i = 0; i < dataAmount && !queue->isEmpty; i++) {
		data[i] = getNext(queue);
	}
	// Return the number of elements retrieved
	return (i + 1);
}

uint32_t putArray(queue_t *queue, queuedata_t *data, uint32_t dataAmount) {
	// If the queue is full, return 0
	if (queue->isFull) {
		return 0;
	}

	uint32_t i = 0;
	// Add dataAmount elements to the queue or until the queue is full
	for (; i < dataAmount && !queue->isFull; i++) {
		put(queue, data[i]);
	}
	// Return the number of elements added
	return (i + 1);
}

uint32_t getFillLevel(queue_t *queue) {
	// Calculate the number of elements in the queue
	int diff = queue->tail - queue->front;
	// Adjust for wrap-around
	return diff < 0 ? diff + BUFFER_SIZE : diff;
}

queuedata_t readValue(queue_t *queue, uint16_t index) {
	queuedata_t value = 0;

	// If the index is within the bounds of the queue, retrieve the value
	if (getFillLevel(queue) > index) {
		value = queue->buffer[queue->front - index];
	}

	// Return the value or 0 if the index was out of bounds
	return value;
}

