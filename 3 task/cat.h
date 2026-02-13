#ifndef CAT_TYPE
#define CAT_TYPE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "animal.h"

struct cat {
  struct animal Super;
};

struct animal* cat_create();
void cat_destroy(struct animal* ptr);
void cat_voice(void* ptr); 
void cat_scratch(void* ptr); 

#endif