#ifndef ANIMAL
#define ANIMAL
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct dog {
  char* name;
  char* sound; 
};

struct dog* dog_create();
struct dog* dog_create_with_name(char* name, char* sound);
void dog_destroy(struct dog* ptr);
void dog_voice(struct dog* ptr); 


#endif