#include <stdio.h>
#include "parser.h"
#include "MyPython.h"
#include "ast.h"

struct pair {
  const char* var_name;
  struct MyPyObj* obj;
};

size_t GlobalNS_index = 0;
struct pair GlobalNamespace[10] = {}; //Допускаем только конечное количество переменных в глобальной области видимости

struct MyPyObj* assignment(Node* root, struct MyPyType* type);
struct MyPyObj* create_obj(Node* node, struct MyPyType* type); //everything is an object in Python
struct MyPyObj* process_tree(Node* root, struct MyPyType* type);
size_t search_in_globalNS(struct pair* space, struct pair p);
void put_in_globalNS(struct pair p);

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



  destroy_MyPyType(MyIntType); //уничтожаем тип инта
  
  return 0;
} 

struct MyPyObj* process_tree(Node* root, struct MyPyType* type) {
  switch(root->type) {
    case TOKEN_ASSIGN:
      return assignment(root, type);
      break;
    case TOKEN_PLUS: \
      return plus_tree();
      break;
    case TOKEN_MINUS:
      return minus_tree();
      break;

    default:
      process_tree(root->left, type);
      process_tree(root->right, type);
      break;

  }
}

struct MyPyObj* assignment(Node* root, struct MyPyType* type) {
  if (root == NULL)
    return NULL;
  if (root->left && root->right) {
    struct MyPyObj* left_obj = create_obj(root->left, type);
    if (left_obj->type == TOKEN_IDENTIFIER) {
      struct pair tmp = {root->left->value, left_obj};
      put_in_globalNS(tmp); 
    }

    struct MyPyObj* right_obj = process_tree(root->right, type);
   
    int left_value = ((struct MyPyIntObj*)left_obj)->value;
    int right_value = ((struct MyPyIntObj*)right_obj)->value;
   
    struct MyPyIntObj* result_int = (struct MyPyIntObj*)left_obj;
    result_int->value = left_value + right_value;
    return left_obj;
  }
}

struct MyPyObj* plus_tree(Node* node, struct MyPyType* type) {
  if (node == NULL)
    return NULL;

  if (node->left && node->right) {

    struct MyPyObj* left_obj  = create_obj(node->left, type);
    struct MyPyObj* right_obj = create_obj(node->right, type);
   
    int left_value = ((struct MyPyIntObj*)left_obj)->value;
    int right_value = ((struct MyPyIntObj*)right_obj)->value;

    int result = left_value + right_value;

    struct MyPyObj* new_obj  = create_obj(node->left, type);

    struct pair tmp = {node->left->value, left_obj};
    size_t index = search_in_globalNS(GlobalNamespace, tmp);
    if (index != 666) {
      GlobalNamespace[index].obj = NULL;
      GlobalNamespace[index].var_name = NULL;
    } 
   
    return new_obj;
  }
  
}

struct MyPyObj* create_obj(Node* node, struct MyPyType* type) {
  if (node->type != TOKEN_IDENTIFIER && node->type != TOKEN_NUMBER) {
    strerr("Lexical or semantic mistake");
    return NULL;
  }

  struct MyPyObj* obj = MyPy_NewObject(type) ;
  struct MyPyIntObj* obj_int = (struct MyPyIntObj*)obj;
  obj_int->value = node->int_value;

  //making note in global namespace about new identifier
  if (node->type == TOKEN_IDENTIFIER) {
    struct pair tmp = {node->value, obj};
    size_t index = search_in_globalNS(GlobalNamespace, tmp);
    if (index != 666)
      GlobalNamespace[GlobalNS_index++] = tmp;
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
}

void put_in_globalNS(struct pair p) {
  size_t index = search_in_globalNS(GlobalNamespace, p);
  if (index != 666) {
    GlobalNamespace[index] = p;
  }
}
