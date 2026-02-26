#include <stdio.h>
#include <unistd.h>
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
size_t search_in_globalNS(struct pair* space, struct pair p);
void put_in_globalNS(struct pair p);
struct MyPyObj* get_from_globalNS(const char* name);

int main (void) {
  FILE* file = fopen("INPUT.txt", "r");
    
  if (file == NULL) {
    printf("Error in opening the file\n");
    return 1; 
  }

  char buffer[1024];

  size_t  elem_read = fread(&buffer, sizeof(char), 1024, file);
  printf("Прочитано элементов: %zu\n", elem_read);

  Lexer lexer;
  lexer_init(&lexer, buffer);
  Token token_buffer[1024];

  size_t index = get_token_buffer(token_buffer, &lexer);

  for (size_t i = 0; i < index; i++) {
    printf("TokenType = %d\n value = %s\n line = %d\n", 
      token_buffer[i].type, token_buffer[i].value, token_buffer[i].line);
  }

  // Строим абстрактное дерево программы
  Node* root = build_ast_recursive(token_buffer, index);

  //подготавливаемся к производству элементов типа MyIntType
  struct MyPyType* MyIntType = create_MyPyType("MyInt", sizeof(struct MyPyIntObj), alloc, dealloc);

  //Проходимся по построенному дереву
  process_tree(root, MyIntType);

  destroy_MyPyType(MyIntType); //уничтожаем тип инта
  
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
    // case TOKEN_BLOCK: // Узел, соединяющий строки
    //     process_tree(root->left, type);
    //     return process_tree(root->right, type);
    default:
      printf("default\n");
      if (root->left) process_tree(root->left, type);
      if (root->right) process_tree(root->right, type);
      return NULL;
  }
}

struct MyPyObj* assignment(Node* root, struct MyPyType* type) {
  if (!root || !root->left || !root->right) return NULL;

  struct MyPyObj* value_obj = process_tree(root->right, type);
  
  if (!value_obj) {
    MyPy_Decref(value_obj);
    return NULL;
  }
  // 2. Левая часть — это имя переменной
  if (root->left->type == TOKEN_IDENTIFIER) {
    struct pair tmp = {root->left->value, value_obj};
    put_in_globalNS(tmp);
    printf("Assigned %s = %d\n", tmp.var_name, ((struct MyPyIntObj*)value_obj)->value);
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

size_t search_in_globalNS(struct pair* space, struct pair p) {
  for (size_t i = 0; i < 10; i++) {
    if (space[i].var_name == NULL || space[i].var_name[0] == '\0') return i;
    else {
      if (!strcmp(space[i].var_name, p.var_name)) {
        printf("Name already exists!");
        return 666;
      }
    }
  }

  return 666;
}

void put_in_globalNS(struct pair p) {
  size_t index = search_in_globalNS(GlobalNamespace, p);
  if (index != 666) {
    GlobalNamespace[index] = p;
    GlobalNS_index++;
    MyPy_Incref(p.obj); //namespace remembering the obj
  }
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
