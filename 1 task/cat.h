
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct cat {
  char* name;
  char* sound; 
};

struct cat* cat_create();
struct cat* cat_create_with_name(char* name, char* sound);
void cat_destroy(struct cat* ptr);
void cat_voice(struct cat* ptr); 

