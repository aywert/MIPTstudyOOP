#include "dog.h"

struct animal* dog_create() {
 
  struct dog* ptr = malloc(sizeof(struct animal));
  ptr->Super.name  = "bobik";
  ptr->Super.sound = "Gouf";

  ptr->Super.vtbl.voice = dog_voice;
  ptr->Super.vtbl.specific_method = dog_bite;

  return (struct animal*)ptr;
}

void dog_destroy(struct animal* ptr) {
  if (ptr != NULL) {free(ptr); }
  ptr = NULL;
}


void dog_voice(void* ptr) {
  assert(ptr);
  printf("%s\n", ((struct animal*)ptr)->sound);
}

void dog_bite(void* ptr) {
  assert(ptr);
  printf("%s", "bites\n");
}