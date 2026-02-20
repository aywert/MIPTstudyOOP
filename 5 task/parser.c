#include "parser.h"

size_t get_token_buffer(Token* buffer, Lexer* lexer) {
  size_t index = 0;
  Token current = lexer_next_token(lexer);
  while (index < 1024 && current.type!=TOKEN_ERROR && current.type!=TOKEN_EOF) {
    buffer[index++] = current;
    current = lexer_next_token(lexer);
  }

  return index;
}

void lexer_init(Lexer* lexer, const char* source) {
  lexer->source = source;
  lexer->position = 0;
  lexer->line = 1;
  lexer->current_char = source[0];
}

void lexer_skip_whitespace(Lexer* lexer) {
  while (lexer->current_char != '\0' && 
      (lexer->current_char == ' ' || lexer->current_char == '\t' || 
      lexer->current_char == '\n')) {
    if (lexer->current_char == '\n')
      lexer->line++;
    lexer->position++;
    lexer->current_char = lexer->source[lexer->position];
  }
}

Token lexer_next_token(Lexer* lexer) {
  Token token;
  token.value = NULL;
  
  lexer_skip_whitespace(lexer);
  
  if (lexer->current_char == '\0') {
    token.type = TOKEN_EOF;
    return token;
  }
  
  token.line = lexer->line;
  
  // Числа
  if (isdigit(lexer->current_char)) {
    token.type = TOKEN_NUMBER;
    int start = lexer->position;
    while (isdigit(lexer->current_char)) {
        lexer->position++;
        lexer->current_char = lexer->source[lexer->position];
    }
    int len = lexer->position - start;
    token.value = malloc(len + 1);
    strncpy(token.value, lexer->source + start, len);
    token.value[len] = '\0';
    return token;
  }
  
  // Идентификаторы (переменные)
  if (isalpha(lexer->current_char) || lexer->current_char == '_') {
    token.type = TOKEN_IDENTIFIER;
    int start = lexer->position;
    while (isalnum(lexer->current_char) || lexer->current_char == '_') {
        lexer->position++;
        lexer->current_char = lexer->source[lexer->position];
    }
    int len = lexer->position - start;
    token.value = malloc(len + 1);
    strncpy(token.value, lexer->source + start, len);
    token.value[len] = '\0';
    return token;
  }
  
  // Операторы
  token.type = TOKEN_ERROR;
  
  switch (lexer->current_char) {
    case '=':
        token.type = TOKEN_ASSIGN;
        token.value = "=";
        lexer->position++;
        break;
    case '+':
        token.type = TOKEN_PLUS;
        token.value = "+";
        lexer->position++;
        break;
    case '-':
        token.type = TOKEN_MINUS;
        token.value = "-";
        lexer->position++;
        break;
    case ';':
        token.type = TOKEN_EOL;
        token.value = "-";
        lexer->position++;
        break;
    default:
        break;
  }
  
  lexer->current_char = lexer->source[lexer->position];
  return token;
}
