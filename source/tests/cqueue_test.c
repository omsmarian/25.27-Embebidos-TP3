#include <stdio.h>
#include <time.h>
#include "cqueue.h"

int main (void)
{
	queue_id_t id = queueInit(QUEUE_MAX_SIZE, sizeof(uint8_t));
	uint8_t var = 0, * data = NULL, * data2 = NULL;

	struct timeval st, et;
	mingw_gettimeofday(&st, NULL);
	for(uint8_t i = 30; i < 31; i++)
	{
		queuePush(id, &i);
		queuePop(id);
	}
	mingw_gettimeofday(&et, NULL);
	double elapsed = ((double)(et.tv_sec - st.tv_sec) * 1000000) + (double)(et.tv_usec - st.tv_usec);
	printf("Time elapsed: %f micro seconds\n", elapsed);

    printf("Size: %d\n", queueSize(id));
	var = 25;
	printf("Push: %d\n", queuePush(id, &var));
	data = (uint8_t*)queuePop(id);
	printf("Pull: %d\n", *data);
	printf("B\n");

    uint8_t i = 0;
    while(++i <= QUEUE_OVERFLOW - 4)
// 	while(queueSize(id) != QUEUE_MAX_SIZE)
// 	while(!queueIsFull(id))
	{
    	if(queuePush(id, &i) == QUEUE_OVERFLOW)
    		printf("Queue Overflowed, Size: %d\n", queueSize(id));
		else
		    printf("Data pushed: %d, Size: %d\n", i, queueSize(id));
    }

	data = (uint8_t*)queueFront(id);
	data2 = (uint8_t*)queueBack(id);
	// printf("Front: %d, Back: %d, Size: %d\n", *(uint8_t*)queueFront(id), *(uint8_t*)queueBack(id), queueSize(id));
	
    printf("Front: %d, Back: %d\n", *data, *data2);

    // queueClear(id);
    printf("Front: %d, Back: %d, Size: %d\n", *(uint8_t*)queueFront(id), *(uint8_t*)queueBack(id), queueSize(id));
    printf("Push: %d, Pull: %d\n", queuePush(id, &i), *(uint8_t*)queuePop(id));
    printf("Pull: %d, Push: %d\n", *(uint8_t*)queuePop(id), queuePush(id, &i));

    i = QUEUE_OVERFLOW + 1;
    while(i--)
// 	while(queueSize(id))
//  while(!queueIsEmpty(id))
	{
		data_t data = queuePop(id);
		if(data == QUEUE_UNDERFLOW)
    		printf("Queue Underflowed, Size %d\n", queueSize(id));
		else
		    printf("Data pulled: %d, Remaining: %d\n", *(uint8_t*)data, queueSize(id));
	}
	printf("E");

	data = (uint8_t*)queueFront(id);
	data2 = (uint8_t*)queueBack(id);
	if (data != QUEUE_UNDERFLOW)
    	printf("Front: %d", *data);
	if (data2 != QUEUE_UNDERFLOW)
		printf(", Back: %d", *data2);

	queueDelete(id);

	return 0;
}
