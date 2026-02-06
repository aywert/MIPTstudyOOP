#include <stdio.h>
#include "dog.h"
#include "cat.h"

int main (void) {
  struct cat* cat = cat_create_with_name("Murka", "i love my owner");
  cat_voice(cat);
  cat_destroy(cat);
  return 0;
}