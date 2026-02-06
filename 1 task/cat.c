#include "cat.h"

struct cat* cat_create() {
  struct cat* ptr = malloc(sizeof(struct cat));
  ptr->name  = "Myrsik";
  ptr->sound = "Myau";
  return ptr;
}

struct cat* cat_create_with_name(char* name, char* sound) {
  struct cat* ptr = malloc(sizeof(struct cat));
  ptr->name  = name;
  ptr->sound = sound;

  return ptr;
}

void cat_destroy(struct cat* ptr) {
  if (ptr != NULL) {free(ptr); }
  ptr = NULL;
}

void cat_voice(struct cat* ptr) {
  assert(ptr);
  printf("%s\n", ptr->sound);
}

