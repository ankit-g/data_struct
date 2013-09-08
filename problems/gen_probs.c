#include "../include/stack.h"
#include "../include/queue.h"
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

// Using generic stack.
void stk_use(void)
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
}

void queue_use(void)
{
	int i = 5;
	struct queue *que;

	que = init_queue(i, sizeof(*que));

	while(i--)
		enqueue(que, &obj[i]);
	i = 5;

	while(i--) {
		struct abc *ob = dequeue(que);
		printf("dequeue %d %d\n", ob->a, ob->b);
	}

	clear_queue(que);
}

// supply problem instance 
int problem_gen(char *str)
{
	queue_use();
	return EXIT_SUCCESS;		
}
