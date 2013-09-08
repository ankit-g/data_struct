#include "../include/queue.h"
#include "../include/header.h"

struct queue *init_queue(int nele, int szobj)
{
	struct queue *queue = ak_alloc(sizeof(*queue));

	queue->szobj = szobj;
	queue->nele  = nele;
	queue->f = -1;
	queue->r = -1;

	queue->data = ak_alloc(nele*sizeof(void *));

	while(nele--)
		queue->data[nele] = ak_alloc(szobj);
				
	return queue;		
}

int enqueue(struct queue *queue, const void *data)
{
	if ((queue->r+1) % queue->nele == queue->f) {
		printf("queue is full\n");
		return -1;
	}
	else {
		queue->r = (queue->r + 1) % queue->nele;

		memcpy((char *)queue->data[queue->r], (char *)data, queue->szobj);

		if(queue->f == -1)
			queue->f = queue->r;
	}

	return 0;	
}

void *dequeue(struct queue *queue)
{
	void *ret;

	if (queue->f == -1) {
		printf("queue is empty\n");
		return (void *)-1;
	}
	else {
		ret = queue->data[queue->f];

		if (queue->f == queue->r)
			queue->f = queue->r = -1;
		else 
			queue->f = (queue->f + 1)% queue->nele;		
	}

	return ret;
}

void clear_queue(struct queue* queue)
{
	while(queue->nele--)
		free(queue->data[queue->nele]);	

	free(queue->data);

	free(queue);
}
