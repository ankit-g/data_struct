/* Truely Generic Stack. Each element is the stack is a generic
 * pointer pointing to a memory location of the object size.
 * While pushing one can copy the object on the stack. 
 * For instance you can copy the address of a pointer on the stack.
 * Later while popping you can retrieve that pointer's address.
 */

#include "../include/stack.h"
#include "../include/header.h"

static int err_val = 0xcafebabe;

// Provide no of elements and size of each element.
struct stk *init_stk(int nele, int szobj)
{
	int i = nele;
	struct stk *nstk = ak_alloc(sizeof(*nstk));
	
	nstk->data = ak_alloc(nele*sizeof(void *));

	nstk->size = nele;

	nstk->sobj = szobj;
	
	while (i--)
		nstk->data[i] = ak_alloc(szobj);
	
	nstk->top = -1;	
		
	return nstk;
}

// Return data and update top
extern void *stk_pop(struct stk *dstk)
{
	if (dstk->top == -1) {
		printf("stack is empty %s\n", __func__);
		return &err_val;		
	}
	
	return dstk->data[dstk->top--];
}

// Push data onto the stk
extern int stk_push(struct stk *dstk, void *data)
{
	if(dstk->top == (dstk->size-1)) {
		printf("Stack is full\n");
		return 0;	
	}

	memcpy((char *)(dstk->data[++dstk->top]), (char *)data, dstk->sobj);
	return 1;
}

// Clear Memory of the heap
extern int clear_stk(struct stk *dstk)
{
	int i;
	if(dstk) {
		if(dstk->data) {
			for(i = 0; i < dstk->size; i++) 
				free(dstk->data[i]);
			free(dstk->data);			
		}
		free(dstk);		
	}
	return EXIT_SUCCESS;
}
