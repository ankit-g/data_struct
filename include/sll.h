struct node
{
	void *data;
	struct node *next;
};

// Provide pointer to head, pointer to data and size of the element.
extern int add_node(struct node **, void *, int );

extern void free_list(struct node *);

// Provide pointer head and the location of the node.
extern void *ret_ele(struct node *, int );
