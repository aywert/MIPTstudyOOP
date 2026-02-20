#include "class_obj.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

struct Class_obj* class_createClass(struct Class_obj* super, const char* name, size_t size) {
  struct Class_obj* new = malloc (sizeof(struct Class_obj));
  new->name_ = name;
  new->super_ = super;
  new->size_ = size;

  return new;
}

void class_addMethod(struct Class_obj* obj, const char* selector, func_t func) {
  (obj->methods_.lst[obj->methods_.index]).SEL  = selector;
  (obj->methods_.lst[obj->methods_.index]).func = func;
  obj->methods_.index++;
}

struct obj* class_createObj(struct Class_obj* class_obj) {
  struct obj* obj = malloc (class_obj->size_);
  obj->isa = class_obj;
  return obj;
}

void class_deleteClass(struct Class_obj* class) {
  free(class); class = NULL;
}


void obj_send(struct obj* obj , struct Class_obj* isa, const char* func_name, void* ptr) {
  size_t index_max = isa->methods_.index;
  bool found = false;

  for (size_t index = 0; index < index_max; index++) {
    if (!strcmp(func_name, isa->methods_.lst[index].SEL)) {
      (isa->methods_.lst[index].func)(ptr); // calling function from the method_list
      found = true;
      break;
    }
  }

  if (found == false && isa->super_ != NULL){
    obj_send(NULL, isa->super_, func_name, ptr);
  }
}
