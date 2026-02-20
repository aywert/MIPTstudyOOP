#ifndef DOG_TYPE
#define DOG_TYPE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "animal.h"
#include "string.h"

struct dog {
  struct animal Super;
  int teeth;
};

void dog_create(void* );
void dog_destroy(void*);
void dog_voice(void* ); 
void dog_bite(void* ); 

#endif