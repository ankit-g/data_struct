//#define DEBUG 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#define debug(fmt, args...)      printf (fmt, ##args)
#else
#define debug(fmt, arg...)
#endif
