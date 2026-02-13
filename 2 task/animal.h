#ifndef ANIMAL_TYPE
#define ANIMAL_TYPE

#include "class_obj.h"

struct animal {
  struct obj base;
  char* name;
  char* sound;
};

#endif