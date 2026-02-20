#pragma once
#include <stdlib.h>

typedef struct MyPyObj* (*alloc_func_t)(int size);
typedef void (*dealloc_func_t)(struct MyPyObj* obj); 

struct MyPyType {
  const char* name;
  int size;

  alloc_func_t     alloc; //function to create
  dealloc_func_t dealloc; // function to destroy
};

struct MyPyObj {
  int refcount;
  struct MyPyType* type;
};

struct MyPyVarObj {
  struct MyPyObj obj;
  int varsize;
};

struct MyPyIntObj {
  struct MyPyVarObj varobj;
  int value;
};

struct MyPyListObj {
  struct MyPyVarObj varobj;
  struct MyPyObj** items;  // массив указателей на объекты
};

struct MyPyType* create_MyPyType(const char* name, size_t size, alloc_func_t alloc, dealloc_func_t dealloc);
struct MyPyObj* MyPy_NewObject(struct MyPyType* type);

struct MyPyObj* alloc(int size); 
void dealloc(struct MyPyObj* obj); 


void MyPy_Incref(struct MyPyObj* obj);
void MyPy_Decref(struct MyPyObj* obj);
