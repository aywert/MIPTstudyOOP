#include "cat.h"

struct animal* cat_create() {
 
  struct cat* ptr = malloc(sizeof(struct animal));
  ptr->Super.name  = "Murka";
  ptr->Super.sound = "Meow";

  ptr->Super.vtbl.voice = cat_voice;

  return (struct animal*)ptr;
}

void cat_destroy(struct animal* ptr) {
  if (ptr != NULL) {free(ptr); }
  ptr = NULL;
}


void cat_voice(void* ptr) {
  assert(ptr);
  printf("%s\n", ((struct animal*)ptr)->sound);
}