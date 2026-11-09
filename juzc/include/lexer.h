#ifndef LEXER_H
#define LEXER_H

#include "diag.h"

#define TOKEN_ARR_CHUNK_CAPACITY 4
#define DIGITS "0123456789"

typedef enum {
    TT_INT = 0,
    TT_FLOAT,
    TT_PLUS,
    TT_MINUS,
    TT_MUL,
    TT_DIV,
    TT_LPAREN,
    TT_RPAREN,
    TT_EOF
} TokenType;

typedef struct {
    TokenType type;
    const char* value;
    Span pos;
} Token;

typedef struct {
    Token* data;
    int size;
    int capacity;
} TokenArray;

typedef struct {
    const char* source;
    long source_len;
    long pos;
    char current;
    TokenArray* tokens;
    Span span;
} Lexer;

TokenArray* token_array_new();

int token_array_append(TokenArray* arr, TokenType type, const char* val, Span pos);

// make sure the free(result) is called after calling fmt_token(...)!
char* fmt_token(Token self);

Lexer* lexer_new(const char* source);

void lexer_advance(Lexer* self);

void lexer_make_number(Lexer* self);

TokenArray* lexer_make_tokens(Lexer* self);

void token_array_free(TokenArray* arr);

void lexer_free(Lexer* self);

#endif