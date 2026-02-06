#include <stdio.h>
#include "dog.h"
#include "cat.h"
#include "animal.h"

int main (void) {
  struct animal* Bobik = dog_create();
  (Bobik->vtbl.voice)(Bobik);

  struct animal* Murka = cat_create();
  (Murka->vtbl.voice)(Murka);

  dog_destroy(Bobik);
  cat_destroy(Murka);
  return 0;
}