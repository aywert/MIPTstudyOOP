#include "cat.h"

void cat_create(void* ptr) {
  assert(ptr);
   printf("i am Murka\n");
  ((struct cat*)ptr)->Super.name  = "Murka";
  ((struct cat*)ptr)->Super.sound = "Meow";
  ((struct cat*)ptr)->claws = 1;
}

void cat_destroy(void* ptr) {
  if (ptr != NULL) {free(ptr); }
  ptr = NULL;
}


void cat_voice(void*) {
  printf("Meow Meow\n");
}

void cat_scratch(void*) {
  printf("scratch.. scratch...\n");
}