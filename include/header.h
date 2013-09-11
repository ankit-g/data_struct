#define DEBUG 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#define debug(fmt, args...)      printf(fmt, ##args)
#define IF()			 printf("FUNC ""%s \n", __func__)
#else
#define debug(fmt, arg...)
#define IF()
#endif

extern void *ak_alloc(size_t);
