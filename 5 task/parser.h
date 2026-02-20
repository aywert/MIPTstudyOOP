
#pragma once

#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef enum {
  TOKEN_IDENTIFIER,
  TOKEN_NUMBER,
  TOKEN_ASSIGN,    // =
  TOKEN_PLUS,      // +
  TOKEN_MINUS,     // -
  TOKEN_EOF,
  TOKEN_EOL,
  TOKEN_ERROR
} TokenType;

typedef struct {
  TokenType type;
  char* value;
  int line;
} Token;

typedef struct {
  const char* source;
  int position;
  int line;
  char current_char;
} Lexer;

void lexer_init(Lexer* lexer, const char* source);
void lexer_skip_whitespace(Lexer* lexer); 
Token lexer_next_token(Lexer* lexer);
size_t get_token_buffer(Token* buffer, Lexer* lexer);
