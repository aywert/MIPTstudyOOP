#include "dog.h"

void dog_create(void* ptr) {
  assert(ptr);
  printf("i am Bobik\n");
  ((struct dog*)ptr)->Super.name  = "Bobik";
  ((struct dog*)ptr)->Super.sound = "Gaw";
  ((struct dog*)ptr)->teeth = 2;
}

void dog_destroy(void* ptr) {
  if (ptr != NULL) {free(ptr); }
  ptr = NULL;
}

void dog_voice(void*) {
  printf("Gaw Gaw\n");
}

void dog_bite(void*) {
  printf("bites\n");
}