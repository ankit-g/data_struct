#include <string.h>
#include <stdlib.h>

extern void *ak_alloc(size_t size)
{
	void *poi;
	
	poi = malloc(size);
	if(!poi) {
		printf("Unable to allocate %s\n", __func__);
		exit(0);	
	}

	memset((char *)poi, 0, size);

	return poi;		
}
