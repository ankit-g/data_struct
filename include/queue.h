struct queue
{
	void *data;
	const int size;
	int fnt;
	int rer;
};

// Initialize queue with given size.
extern struct queue *init_queue(int );

//Enqueue data on the queue.
extern int enqueue(struct queue *, const void *data );

//Dequeue data from the queue.
extern void *dequeue(struct queue*);

// Free the memory allocated.
extern int clear_queue(struct queue*);
