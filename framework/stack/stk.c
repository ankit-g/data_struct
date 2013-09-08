#include "../../include/stack.h"

struct stk *init_stk(int size)
{
	debug("Kopal %s\n", __func__);
	
	return NULL;
}

// Return data and update top
extern void *stk_pop(struct stk *dstk)
{
	debug("Kopal %s\n", __func__);

	return NULL;
}

// Push data onto the stk
extern int stk_push(struct stk *dstk, void *data)
{
	debug("Kopal %s\n", __func__);

	return EXIT_SUCCESS;
}

// Clear Memory of the heap
extern int clear_stk(struct stk *dstk)
{
	debug("Kopal %s\n", __func__);

	return EXIT_SUCCESS;
}
