#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "parser.h"
#include "MyPython.h"
#include "ast.h"

struct pair {
  const char* var_name;
  struct MyPyObj* obj;
};

size_t GlobalNS_index = 0;
struct pair GlobalNamespace[10] = {}; //Допускаем только конечное количество переменных в глобальной области видимости

struct MyPyObj* plus_tree(Node* node, struct MyPyType* type);
struct MyPyObj* assignment(Node* root, struct MyPyType* type);
struct MyPyObj* create_obj(Node* node, struct MyPyType* type); //everything is an object in Python
struct MyPyObj* process_tree(Node* root, struct MyPyType* type);
size_t search_in_globalNS(const char* name);
bool put_in_globalNS(struct pair p);
struct MyPyObj* get_from_globalNS(const char* name);
bool decref_for_identifier(struct MyPyObj* obj);

int main (void) {
  FILE* file = fopen("INPUT.txt", "r");
    
  if (file == NULL) {
    printf("Error in opening the file\n");
    return 1; 
  }

  char buffer[1024]; memset(buffer, 0, sizeof(buffer));

  size_t  elem_read = fread(&buffer, sizeof(char), 1024, file);
  printf("Elements read: %zu\n", elem_read);

  Lexer lexer;
  lexer_init(&lexer, buffer);
  Token token_buffer[1024];

  size_t index = get_token_buffer(token_buffer, &lexer);

  //create my type for mypython MyIntType
  struct MyPyType* MyIntType = create_MyPyType("MyInt", sizeof(struct MyPyIntObj), alloc, dealloc);

  // Building abstract tree
  Node* root = build_ast_recursive(token_buffer, index);
  process_tree(root, MyIntType);
  
  for (size_t i = 0; i < GlobalNS_index; i++) {
    if (GlobalNamespace[i].obj) {
      struct MyPyIntObj* int_obj = (struct MyPyIntObj*)GlobalNamespace[i].obj;
      printf("var: %s num_of_reffs: %d value: %d\n", 
        GlobalNamespace[i].var_name, 
        (GlobalNamespace[i].obj)->refcount, 
        int_obj->value);
      MyPy_Decref(GlobalNamespace[i].obj);
    }
  }

  free_tree(root);
  destroy_MyPyType(MyIntType); //destroy type of int
  
  return 0;
} 

struct MyPyObj* process_tree(Node* root, struct MyPyType* type) {
  if (!root) return NULL;

  switch(root->type) {
    case TOKEN_ASSIGN:
      printf("assign\n");
      return assignment(root, type); 
    case TOKEN_PLUS: 
      printf("plus\n");
      return plus_tree(root, type);
    case TOKEN_NUMBER: return create_obj(root, type);
    case TOKEN_IDENTIFIER: {
      printf("identifierr or number\n");
      struct MyPyObj* gv_sample = get_from_globalNS(root->value);
      if (gv_sample) return gv_sample; 

      fprintf(stderr, "NameError: name '%s' is not defined\n", root->value);
      return NULL;
    }
    case TOKEN_BLOCK:{ // Узел, соединяющий строки
      printf("block\n");
      process_tree(root->left, type);
      return process_tree(root->right, type);
    }

    default:
      printf("default\n");
      return NULL;
  }
}

struct MyPyObj* assignment(Node* root, struct MyPyType* type) {
  if (!root || !root->left || !root->right) return NULL;

  struct MyPyObj* value_obj = process_tree(root->right, type);
  
  if (!value_obj) {
    return NULL;
  }

  if (root->left->type == TOKEN_IDENTIFIER) {
    if (root->right->type == TOKEN_IDENTIFIER) {
      struct MyPyObj* result = MyPy_NewObject(type);
      ((struct MyPyIntObj*)result)->value = ((struct MyPyIntObj*)value_obj)->value;
      
      struct pair tmp = {root->left->value, result};
      if (put_in_globalNS(tmp)) {
        printf("Assigned %s = %d\n", tmp.var_name, ((struct MyPyIntObj*)result)->value);
      } else {
        printf("Failed to initialize %s as its already exists\n", tmp.var_name);
        return NULL;
      }
      // Уменьшаем счетчик исходного объекта (убираем ссылку от process_tree)
      MyPy_Decref(value_obj);
      return result;
    }

    else {
      struct pair tmp = {root->left->value, value_obj};
      if (put_in_globalNS(tmp)) {
        printf("Assigned %s = %d\n", tmp.var_name, ((struct MyPyIntObj*)value_obj)->value);
        return value_obj;
      } else printf("Failed to initialize %s as its already exists\n", tmp.var_name); 
      //MyPy_Decref(value_obj);  // Очищаем при ошибке
      return NULL;
    }
  }

  return value_obj;
}

struct MyPyObj* plus_tree(Node* node, struct MyPyType* type) {
  if (!node) return NULL;

  // Вычисляем левую и правую части (там могут быть еще плюсы или числа)
  struct MyPyObj* left_obj  = process_tree(node->left, type);
  struct MyPyObj* right_obj = process_tree(node->right, type);

  if (!left_obj || !right_obj) {
    MyPy_Decref(left_obj);
    MyPy_Decref(right_obj);
    return NULL;
  }

  int left_val = ((struct MyPyIntObj*)left_obj)->value;
  int right_val = ((struct MyPyIntObj*)right_obj)->value;

  // Создаем новый временный объект для результата
  struct MyPyObj* res_obj = MyPy_NewObject(type);
  ((struct MyPyIntObj*)res_obj)->value = left_val + right_val;
  
  MyPy_Decref(left_obj);
  MyPy_Decref(right_obj);

  return res_obj;
  
}

struct MyPyObj* create_obj(Node* node, struct MyPyType* type) {
  if (node->type != TOKEN_IDENTIFIER && node->type != TOKEN_NUMBER) {
    fprintf(stderr, "Lexical or semantic mistake\n");
    return NULL;
  }

  struct MyPyObj* obj = MyPy_NewObject(type) ;
  struct MyPyIntObj* obj_int = (struct MyPyIntObj*)obj;
  obj_int->value = node->int_value;

  //making note in global namespace about new identifier
  if (node->type == TOKEN_IDENTIFIER) {
    struct pair tmp = {node->value, obj};
    put_in_globalNS(tmp); 
  }

  return obj;
}

size_t search_in_globalNS(const char* name) {
  for (size_t i = 0; i < 10; i++) {
    if (GlobalNamespace[i].var_name == NULL || GlobalNamespace[i].var_name[0] == '\0') return i;
    else {
      if (!strcmp(GlobalNamespace[i].var_name, name)) {
        printf("Name already exists!\n");
        return 666;
      }
    }
  }

  return 666;
}

bool put_in_globalNS(struct pair p) {
  size_t index = search_in_globalNS(p.var_name);
  if (index != 666) {
    GlobalNamespace[index] = p;
    GlobalNS_index++;
    return true;
  }

  return false;
}

struct MyPyObj* get_from_globalNS(const char* name) {
  if (!name) return NULL;

  for (size_t i = 0; i < GlobalNS_index; i++) {
    if (GlobalNamespace[i].var_name != NULL) { 
      if (strcmp(GlobalNamespace[i].var_name, name) == 0) {
        struct MyPyObj* obj = GlobalNamespace[i].obj;
        MyPy_Incref(obj); 
        return obj;
      }
    }
  }
  return NULL; 
}

bool decref_for_identifier(struct MyPyObj* obj) {
  if (!obj) return NULL;

  for (size_t i = 0; i < GlobalNS_index; i++) {
    if (GlobalNamespace[i].obj == obj) {
      return true;
    }
  }
  return false; 
}
