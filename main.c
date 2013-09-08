#include "include/header.h"

extern int problem(char *);

int main(int argc, char *argv[])
{	
	printf("From Main\n");
	problem(NULL);
	return EXIT_SUCCESS;
}
