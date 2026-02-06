#ifndef DOG_TYPE
#define DOG_TYPE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "animal.h"

struct dog {
  struct animal Super;
};

struct animal* dog_create();
//struct dog* dog_create_with_name(char* name, char* sound);
void dog_destroy(struct animal* ptr);
void dog_voice(void* ptr); 

#endif