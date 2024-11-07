/***************************************************************************//**
  @file     cqueue.c
  @brief    Circular Queue / Ring Buffer / FIFO Implementation
  @author   Group 4: - Oms, Mariano
                     - Solari Raigoso, Agustín
                     - Wickham, Tomás
                     - Vieira, Valentin Ulises
  @note     Based on the work of Daniel Jacoby
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "cqueue.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DEVELOPMENT_MODE	1

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/**
 * @brief Queue structure
 * @param items Data, QUEUE_MAX_SIZE elements + 1 for overflow detection
 * @param front First element, equal to rear if empty
 * @param Rear Last element, points to next empty slot or QUEUE_OVERFLOW if full
 */
typedef struct {
	data_t	items;
	count_t	front;
	count_t	rear;
	count_t size;
	uint8_t	data_size;
} queue_t; // TODO: Use void* for data_t and count_t

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static queue_t queues[QUEUES_MAX_CANT];
static queue_id_t ids;

/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

queue_id_t queueInit (count_t size, uint8_t data_size)
{
	queue_id_t id = QUEUE_INVALID_ID;

	printf("IDS: %d\n", ids);
	if (ids < QUEUES_MAX_CANT)
	{
       	queues[ids].items = (data_t*)malloc(size * data_size);
        if (queues[ids].items != NULL)
		{
			queueClear(ids);
			queues[ids].size = size;
			queues[ids].data_size = data_size;
			printf("Size: %d\n", queues[id].data_size);
			id = ids++;
		}
	}
	printf("IDS: %d\n", ids);
	printf("ID: %d\n", id);

	return id;
}

void queueDelete (queue_id_t id)
{
    if (id < QUEUES_MAX_CANT)
	{
        free(queues[id].items);
		ids--;
    }
}

count_t queuePush (queue_id_t id, data_t data) // Enqueue
{
	count_t count = queueSize(id);

	printf("Id: %d, Size: %d\n", id, queues[id].data_size);
	if ((id < ids) && !queueIsFull(id))
	{
		printf("A\n");
		// printf("Data: %d\n", ((uint8_t*)queues[id].items[queues[id].rear * queues[id].size])[0]);
		for (size_t i = 0; i * sizeof(uint8_t) < queues[id].data_size; i++)
		{
			printf("Hola\n");
			printf("Rear: %d\n", queues[id].rear);
			printf("C: %d\n", ((uint8_t*)data)[i]);
			printf("D: %x\n", ((uint8_t*)queues[id].items)[queues[id].rear * queues[id].data_size]);
			((uint8_t*)queues[id].items)[queues[id].rear++ * queues[id].data_size] = ((uint8_t*)data)[i];
			printf("D: %d\n", ((uint8_t*)queues[id].items)[(queues[id].rear-1) * queues[id].data_size]);
			printf("Rear: %d\n", queues[id].rear);
			// queues[id].rear++;
		}
		printf("Data: %d\n", ((uint8_t*)queues[id].items)[(queues[id].rear-1) * queues[id].data_size]);
		if (queues[id].rear >= queues[id].size)
			queues[id].rear -= queues[id].size;
		printf("Rear: %d\n", queues[id].rear);
		printf("Size: %d\n", queues[id].data_size);
	}

	return count + 1;
}

data_t queuePop (queue_id_t id) // Dequeue
{
	data_t data = QUEUE_UNDERFLOW;

	if ((id < ids) && !queueIsEmpty(id))
	{
		data = queues[id].items + queues[id].front++ * queues[id].data_size;
		printf("Holaa\n");
		printf("Data: %d\n", *(uint8_t*)data);
		if (queues[id].front >= queues[id].size)
			queues[id].front -= queues[id].size;
	}

	return data;
}

data_t queueAccess (queue_id_t id, count_t index, queue_access_t access, data_t data)
{
	data_t _data = QUEUE_UNDERFLOW;
	count_t _index = queues[id].front + index;

	if ((id < ids) && (index < queueSize(id)))
	{
		if (queues[id].front + index >= queues[id].size)
			_index -= queues[id].size;

		if (access == QUEUE_READ)
			_data = queues[id].items + _index * queues[id].data_size;
		else if (access == QUEUE_WRITE)
		{
			for (size_t i = 0; i * sizeof(uint8_t) < queues[id].data_size; i++)
				((uint8_t*)queues[id].items)[_index * queues[id].data_size] = ((uint8_t*)data)[i];

			_data = data;
		}
	}

	return _data;
}

count_t	queueSize (queue_id_t id)
{
	count_t size = (queues[id].size + queues[id].rear) - queues[id].front;
	if (size >= queues[id].size)
		size -= queues[id].size;

	return size;
}

data_t	queueFront		(queue_id_t id) { return !queueIsEmpty(id) ? queues[id].items + queues[id].front * queues[id].data_size : QUEUE_UNDERFLOW; }
data_t	queueBack		(queue_id_t id) { return !queueIsEmpty(id) ? queues[id].items + (queues[id].rear - 1) * queues[id].data_size : QUEUE_UNDERFLOW; }
bool	queueIsEmpty	(queue_id_t id) { return !queueSize(id); }
bool	queueIsFull		(queue_id_t id) { return queueSize(id) == queues[id].size; }
void	queueClear		(queue_id_t id) { queues[id].front = queues[id].rear = 0; }

/******************************************************************************/
