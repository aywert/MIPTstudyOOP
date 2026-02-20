#ifndef CAT_TYPE
#define CAT_TYPE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "animal.h"

struct cat {
  struct animal Super;
  int claws;
};

void cat_create(void* );
void cat_destroy(void*);
void cat_voice(void* ); 
void cat_scratch(void* ); 

#endif