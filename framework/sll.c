#include "../include/header.h"
#include "../include/sll.h"

void prnt_list(struct node *tmp)
{
	int i = 0;
	IF();

	while(tmp) {
                printf("%d %d\n", i++, *(int *)tmp->data);
                tmp = tmp->next;
        }
}

void rev_list(struct node **head)
{
	struct node *q, *r;
	struct node *hd = *head;
	IF();
	
	q = hd->next;
	hd->next = NULL;

	while (q) {
		r = q->next;
		q->next = hd;
		hd = q;
		q = r;
	}	
	
	*head = hd;
}

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
	IF();

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
	IF();

	while(n--) {
		g = rand()%max;
		add_node(head, &g, sizeof(int));
	}
}

void *middle_ele(struct node *head)
{
	struct node *fst, *slw;
	fst = slw = head;
	IF();
	
	while(fst && fst->next) {
		fst = fst->next->next;
		slw = slw->next;
	}	

	return slw->data;
}
