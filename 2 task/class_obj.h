#ifndef CLASS_OBJECT_TYPE
#define CLASS_OBJECT_TYPE

#include "method_list.h"

struct Class_obj {
  struct Class_obj* super_;
  const char* name_;
  size_t size_;
  struct Method_list methods_;
};

struct obj {
  struct Class_obj* isa;
};

struct Class_obj* class_createClass(struct Class_obj* super, const char* name, size_t size);
void class_addMethod(struct Class_obj* obj, const char* selector, func_t func); 
void class_deleteClass(struct Class_obj* class);
struct obj* class_createObj(struct Class_obj* class_obj) ;

void obj_send(struct obj* obj , struct Class_obj* isa, const char* func_name, void* ptr); 

#endif