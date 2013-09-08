struct queue
{
	void **data;
	int nele;
	int szobj;
	int f;
	int r;
};

// Initialize queue with given size.
extern struct queue *init_queue(int , int );

//Enqueue data on the queue.
extern int enqueue(struct queue *, const void *);

//Dequeue data from the queue.
extern void *dequeue(struct queue*);

// Free the memory allocated.
extern void clear_queue(struct queue*);
