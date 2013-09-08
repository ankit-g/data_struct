struct node
{
	void *data;
	struct node *next;
};

extern int add_node(struct node **, void *, int );

extern void free_list(struct node *);

extern void *ret_ele(struct node *, int );
