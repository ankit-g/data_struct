#include "../include/stack.h"
#include "../include/header.h"

struct abc
{
	int a;
	int b;
} obj[5] = {
		{1, 2},
		{3, 4},
		{5, 6},
		{7, 8},
		{9, 10},
};

// supply problem instance 
int problem(char *str)
{
	int i = 5;
	struct stk *new_stk;
	new_stk = init_stk(5, sizeof(struct abc));


	while(i--)
		stk_push(new_stk, &obj[i]);
	i = 5;
	while(i--) {
		struct abc *obj = stk_pop(new_stk);
		printf("%d %d\n", obj->a, obj->b);
	}
	clear_stk(new_stk);

	return EXIT_SUCCESS;		
}
