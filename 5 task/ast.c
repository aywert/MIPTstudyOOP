#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Node* create_number_node(int value) {
    Node* node = malloc(sizeof(Node));
    if (!node) return NULL;
    
    node->type = TOKEN_NUMBER;
    node->int_value = value;
    node->value = NULL;  // важно инициализировать!
    node->left = node->right = NULL;
    return node;
}

Node* create_variable_node(const char* name) {
    Node* node = malloc(sizeof(Node));
    if (!node) return NULL;
    
    node->type = TOKEN_IDENTIFIER;
    node->value = malloc(strlen(name) + 1);  // ВЫДЕЛЯЕМ память!
    if (node->value) {
        strcpy(node->value, name);
    }
    node->int_value = 0;
    node->left = node->right = NULL;
    return node;
}

Node* create_operator_node(char op, Node* left, Node* right) {
    Node* node = malloc(sizeof(Node));
    if (!node) return NULL;
    
    if (op == '+')
        node->type = TOKEN_PLUS;
    else 
        node->type = TOKEN_MINUS;
    
    node->op = op;
    node->value = NULL;  // важно инициализировать!
    node->left = left;
    node->right = right;
    return node;
}

Node* create_assign_node(Node* left, Node* right) {
    Node* node = malloc(sizeof(Node));
    if (!node) return NULL;
    
    node->type = TOKEN_ASSIGN;
    node->value = NULL;  // важно инициализировать!
    node->left = left;
    node->right = right;
    return node;
}

// Вспомогательные функции
Token* current_token(Parser* parser) {
    if (parser->pos >= parser->count) return NULL;
    return &parser->tokens[parser->pos];
}

void advance(Parser* parser) {
    parser->pos++;
}

int check(Parser* parser, TokenType type) {
    Token* token = current_token(parser);
    return token && token->type == type;
}

void eat(Parser* parser, TokenType type) {
    if (check(parser, type)) {
        advance(parser);
    } else {
        printf("Ошибка: ожидался токен типа %d, получен %d\n", type, 
               current_token(parser) ? current_token(parser)->type : -1);
        exit(1);
    }
}

// Рекурсивные функции парсинга
Node* parse_term(Parser* parser) {
    Token* token = current_token(parser);
    if (!token) {
        printf("Ошибка: неожиданный конец файла\n");
        return NULL;
    }
    
    if (check(parser, TOKEN_NUMBER)) {
        advance(parser);
        return create_number_node(atoi(token->value));
    }
    else if (check(parser, TOKEN_IDENTIFIER)) {
        advance(parser);
        return create_variable_node(token->value);
    }
    else {
        printf("Ошибка: ожидалось число или идентификатор, получен токен типа %d\n", token->type);
        return NULL;
    }
}

Node* parse_expr(Parser* parser) {
    Node* left = parse_term(parser);
    if (!left) return NULL;
    
    while (check(parser, TOKEN_PLUS) || check(parser, TOKEN_MINUS)) {
        TokenType op_type = current_token(parser)->type;
        char op = (op_type == TOKEN_PLUS) ? '+' : '-';
        
        advance(parser);
        
        Node* right = parse_term(parser);
        if (!right) {
            free_tree(left);
            return NULL;
        }
        
        left = create_operator_node(op, left, right);
        if (!left) return NULL;
    }
    
    return left;
}

Node* parse_assign(Parser* parser) {
    int saved_pos = parser->pos;
    
    if (check(parser, TOKEN_IDENTIFIER)) {
        Token* ident = current_token(parser);
        advance(parser);
        
        if (check(parser, TOKEN_ASSIGN)) {
            advance(parser);
            
            Node* right = parse_expr(parser);
            if (!right) return NULL;
            
            Node* left = create_variable_node(ident->value);
            if (!left) {
                free_tree(right);
                return NULL;
            }
            
            return create_assign_node(left, right);
        }
    }
    
    parser->pos = saved_pos;
    return parse_expr(parser);
}

Node* parse_program(Parser* parser) {
    return parse_assign(parser);
}

Node* build_ast_recursive(Token* tokens, int token_count) {
    if (!tokens || token_count == 0) return NULL;
    
    Parser parser;
    parser.tokens = tokens;
    parser.pos = 0;
    parser.count = token_count;
    
    Node* root = parse_program(&parser);
    
    if (parser.pos < token_count) {
        printf("Предупреждение: остались необработанные токены (позиция %d из %d)\n", 
               parser.pos, token_count);
    }
    
    return root;
}

void print_tree(Node* node, int level) {
    if (!node) {
        printf("NULL\n");
        return;
    }
    
    for (int i = 0; i < level; i++) printf("  ");
    
    switch (node->type) {
        case TOKEN_NUMBER:
            printf("NUM: %d\n", node->int_value);
            break;
        case TOKEN_IDENTIFIER:
            printf("VAR: %s\n", node->value ? node->value : "NULL");
            break;
        case TOKEN_PLUS:
            printf("+\n");
            print_tree(node->left, level + 1);
            print_tree(node->right, level + 1);
            break;
        case TOKEN_MINUS:
            printf("-\n");
            print_tree(node->left, level + 1);
            print_tree(node->right, level + 1);
            break;
        case TOKEN_ASSIGN:
            printf("=\n");
            print_tree(node->left, level + 1);
            print_tree(node->right, level + 1);
            break;
        default:
            printf("UNKNOWN TYPE: %d\n", node->type);
    }
}

void free_tree(Node* node) {
    if (!node) return;
    
    free_tree(node->left);
    free_tree(node->right);
    
    if (node->value) {
        free(node->value);
        node->value = NULL;
    }
    
    free(node);
}