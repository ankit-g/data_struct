struct node
{
	void *data;
	struct node *next;
};


/* Provide the address of head pointer, the pointer to data 
 * and the size of data to be stored.
 */
extern int add_node(struct node **, void *, int );

extern void free_list(struct node *);

// Provide pointer head and the location of the node.
extern void *ret_ele(struct node *, int );

/* Provide the address of head pointer, no of nodes to be created 
 * and the max val of element. Creates n integer nodes. 
 */
extern void crt_nodes(struct node **, int, int);

extern void *middle_ele(struct node *);

extern void rev_list(struct node **);

extern void prnt_list(struct node *);
