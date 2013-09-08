#include "header.h"

struct stk {
	void *data;
	int top;	
};

// Initialize Stack, providing the required size. 
extern struct stk *init_stk(int );

// Return data and update top
extern void *stk_pop(struct stk *);

// Push data onto the stk
extern int stk_push(struct stk*, void *);

// Clear Memory of the heap
extern int clear_stk(struct stk*);
