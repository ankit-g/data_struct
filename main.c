#include "include/header.h"

extern int problem_gen(char *);
extern void problem_lil(int);

enum probset {GEN, STR, SRT, LIL, TRE};

int main(int argc, char *argv[])
{	
	int i;
	int arg;

	char *str = "gen / str / srt / lil / tre";

	char *prb[] = {"gen", "str", "srt", "lil", "tre"};	

	if (argc < 2) {
label:
		printf("Usage : elf %s\n", str);
		printf("gen = generic\n");
		printf("str = string\n");
		printf("srt = sorting\n");
		printf("lil = linked list\n");
		printf("tre = tree\n");
		exit(0);	
	}
	
	if (argc == 3) 
		arg = atoi(argv[2]);

	for (i = 0; i < (sizeof(prb)/sizeof(*prb)); i++) 
		if(!strcmp(prb[i], argv[1]))
			break;
		
	switch(i) 
	{
		case GEN: printf("generic problem\n");
			  problem_gen(NULL);
			  break;

		case STR: printf("string problem\n");
			  break;	 

		case SRT: printf("sorting problem\n");
			  break;
	
		case LIL: printf("linked list problem\n");
			  problem_lil(arg);	
			  break;
	
		case TRE: printf("tree problem\n");
			  break;	

		default:  printf("please specify a valid case\n");
			  goto label;	 
			
	}

	return EXIT_SUCCESS;
}
