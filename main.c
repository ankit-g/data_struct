#include "include/header.h"

extern int problem(char *);

int main(int argc, char *argv[])
{	
	printf("Kopal %s\n", __func__);
	problem(NULL);
	return EXIT_SUCCESS;
}
