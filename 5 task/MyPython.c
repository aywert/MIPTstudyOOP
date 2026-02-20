#include "MyPython.h"


struct MyPyType* create_MyPyType(const char* name, size_t size, alloc_func_t alloc, dealloc_func_t dealloc) {
  struct MyPyType* NewType = malloc(sizeof(struct MyPyType));
  NewType->name = name;
  NewType->size = size;
  NewType->alloc = alloc;
  NewType->dealloc = dealloc;
  return NewType;
}

void destroy_MyPyType(struct MyPyType* type) {
  free(type); type = NULL;
}

struct MyPyObj* MyPy_NewObject(struct MyPyType* type) {
  struct MyPyObj* obj = type->alloc(type->size);
  obj->refcount = 1;
  return obj;
}

void MyPy_Incref(struct MyPyObj* obj) {
  if (obj) obj->refcount++;
}

void MyPy_Decref(struct MyPyObj* obj) {
    if (!obj) return;
    
    if (--obj->refcount == 0) {
        if (obj->type && obj->type->dealloc) {
            obj->type->dealloc(obj);
        } else {
          free(obj);
        }
    }
}

struct MyPyObj* alloc(int size) {
  struct MyPyObj* obj = malloc(size);
  if (obj) {
    obj->refcount = 1;
  }
  return (struct MyPyObj*)obj;
}

void dealloc(struct MyPyObj* obj) {
  free(obj); obj = NULL;
}

struct MyPyObj* int_alloc(int size) {
 
  (void)size;
  
  // Выделяем память под структуру MyPyIntObj
  struct MyPyIntObj* obj = (struct MyPyIntObj*)malloc(sizeof(struct MyPyIntObj));
  if (!obj) return NULL;
  
  // Инициализируем базовые поля
  obj->varobj.obj.refcount = 1;
  obj->varobj.obj.type = NULL;  // тип будет установлен позже
  obj->varobj.varsize = 0;       // для int varsize не используется
  obj->value = 0;
  
  return (struct MyPyObj*)obj;
}