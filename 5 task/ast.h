#pragma once
#include "parser.h"
#include <stdio.h>

typedef struct node {
    TokenType type;
    struct node* left;
    struct node* right;
    char* value;      // для идентификаторов
    char op;          // для операторов
    int int_value;    // для чисел
} Node;

// Структура парсера
typedef struct {
    Token* tokens;
    int pos;
    int count;
} Parser;

// Функции создания узлов
Node* create_number_node(int value);
Node* create_variable_node(const char* name);
Node* create_operator_node(char op, Node* left, Node* right);
Node* create_assign_node(Node* left, Node* right);

// Функции парсера
Token* current_token(Parser* parser);
void advance(Parser* parser);
int check(Parser* parser, TokenType type);
void eat(Parser* parser, TokenType type);

// Рекурсивные функции
Node* parse_term(Parser* parser);
Node* parse_expr(Parser* parser);
Node* parse_assign(Parser* parser);
Node* parse_program(Parser* parser);

// Главная функция
Node* build_ast_recursive(Token* tokens, int token_count);

// Вспомогательные функции
void print_tree(Node* node, int level);
void free_tree(Node* node);