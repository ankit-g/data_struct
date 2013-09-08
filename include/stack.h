#include "header.h"

struct stk {
	void **data;
	int size;
	int sobj;
	int top;	
};

/* Initialize Stack, providing the required size. 
 * Second parameter is the size of each element
 */
extern struct stk *init_stk(int , int);

// Return data and update top.
extern void *stk_pop(struct stk *);

// Push data onto the stk.
extern int stk_push(struct stk*, void *);

// Clear Memory of the heap.
extern int clear_stk(struct stk*);
