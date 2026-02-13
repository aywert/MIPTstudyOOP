#include <stdio.h>
#include "dog.h"
#include "cat.h"

int main (void) {
  struct Class_obj* OBJ    = class_createClass(NULL, "Object", sizeof(int));
  struct Class_obj* ANIMAL = class_createClass(OBJ, "animal", sizeof(struct animal));
  struct Class_obj* DOG    = class_createClass(ANIMAL, "DOG", sizeof(struct dog));
  class_addMethod(DOG, "ctor", dog_create);
  class_addMethod(DOG, "dtor", dog_destroy);
  class_addMethod(DOG, "voice", dog_voice); 
  class_addMethod(DOG, "byte", dog_bite); 

  struct Class_obj* CAT    = class_createClass(ANIMAL, "CAT", sizeof(struct cat));
  class_addMethod(CAT, "ctor", cat_create);
  class_addMethod(DOG, "dtor", cat_destroy);
  class_addMethod(CAT, "voice", cat_voice); 
  class_addMethod(CAT, "scratch", cat_scratch); 

  struct obj* obj_dog = class_createObj(DOG);
  struct obj* obj_cat = class_createObj(CAT);

  obj_send(obj_dog, obj_dog->isa, "ctor", (void*)obj_dog);
  obj_send(obj_cat, obj_cat->isa, "ctor", (void*)obj_dog);

  obj_send(obj_dog, obj_dog->isa, "voice", NULL);
  obj_send(obj_cat, obj_cat->isa, "voice", NULL);
  
  obj_send(obj_dog, obj_dog->isa, "dtor", (void*)obj_dog);
  obj_send(obj_cat, obj_cat->isa, "dtor", (void*)obj_dog);

  class_deleteClass(DOG);
  class_deleteClass(CAT);
  class_deleteClass(ANIMAL);
  class_deleteClass(OBJ); 
}