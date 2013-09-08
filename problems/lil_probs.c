#include "../include/sll.h"
#include "../include/header.h"

struct abc
{
        int a;
        int b;
} static obj[5] = {
                {1, 2},
                {3, 4},
                {5, 6},
                {7, 8},
                {9, 10},
};

void lil_test(void)
{
	struct node *head = NULL;
	int i = 5;
	
	while(i--)
	add_node(&head, &obj[i], sizeof(struct abc));

	i = 0;

	while(i < 5)
	{	
		struct abc *ob = ret_ele(head, i);
		printf("%d %d\n", ob->a, ob->b);
		i++;
	}

	free_list(head);	
}


void problem_lil(void)
{
	lil_test();	
}
