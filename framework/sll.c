#include "../include/header.h"
#include "../include/sll.h"

int add_node(struct node **head, void *data, int objsz)
{
	if (!*head) {
		*head = ak_alloc(sizeof(**head));
		(*head)->data = ak_alloc(objsz);
		memcpy((char *)((*head)->data), (char *)data, objsz);
		(*head)->next = NULL;
	}
	else 
		add_node(&((*head)->next), data, objsz);
	return 0;
}

void free_list(struct node *head)
{
	struct node *temp = head;

	while (temp) {	
		head = head->next;
		free(temp->data);
		free(temp);
		temp = head;				
	}
}

void *ret_ele(struct node *head, int location)
{
	while(location--)
		head = head->next;

	return head->data;
}

// Create n nodes with randum integers 
void crt_nodes(struct node **head, int n, int max)
{
	int g;

	while(n--) {
		g = rand()%max;
		add_node(head, &g, sizeof(int));
	}
}
