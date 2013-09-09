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

void lil_op(void)
{
	struct node *head = NULL, *tmp;
	int i = 0;

	crt_nodes(&head, 9, 20);

	tmp = head;
	
	while(tmp) {
		printf("%d %d\n", i++, *(int *)tmp->data);
		tmp = tmp->next;	
	}

	printf("middle %d\n", *(int *)middle_ele(head));

	free_list(head);		
}


void problem_lil(int s)
{
	switch(s)
	{
		case 0 : lil_test();	
			 break;

		case 1 : lil_op();
			 break;		

		default: printf("Please specify a valid prblem to run\n");			 
	}
		
}
