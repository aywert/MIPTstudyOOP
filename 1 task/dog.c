#include "dog.h"

struct dog* dog_create() {
  struct dog* ptr = ( struct dog* ) malloc(sizeof(struct dog));
  ptr->name  = "bobik";
  ptr->sound = "Gouf";
  return ptr;
}

struct dog* dog_create_with_name(char* name, char* sound) {
  struct dog* ptr = malloc(sizeof(struct dog));
  ptr->name  = name;
  ptr->sound = sound;

  return ptr;
}

void dog_destroy(struct dog* ptr) {
  if (ptr != NULL) {free(ptr); }
  ptr = NULL;
}

void dog_voice(struct dog* ptr) {
  assert(ptr);
  printf("%s\n", ptr->sound);
}

