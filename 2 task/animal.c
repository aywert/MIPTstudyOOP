#include "animal.h"

struct animal* animal_create() {
  struct animal* ptr = malloc(sizeof(struct animal));
  return ptr;
}

void animal_destroy(struct animal* ptr) {
  if (ptr != NULL) {free(ptr); }
  ptr = NULL;
}