#ifndef METHOD_TYPE
#define METHOD_TYPE

#include <stdlib.h>
#include <stdarg.h>

typedef void (*func_t)(void*);
//force only void functions taking zero arguments up to 10 possible functions

struct pair {
  const char* SEL;
  func_t func;
};

struct Method_list {
  size_t index;
  struct pair lst[10];
};

#endif