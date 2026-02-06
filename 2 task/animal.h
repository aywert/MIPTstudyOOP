#ifndef ANIMAL_TYPE
#define ANIMAL_TYPE

#include <malloc.h>

enum ANIMAL {
  DOG = 0, 
  CAT = 1,
};

typedef void (*virtual_func_t)(void* ptr);

struct vtbl {
  virtual_func_t voice;
  virtual_func_t byte;
};

struct animal {
  char* name;
  char* sound;
  struct vtbl vtbl;
};

#endif