#ifndef LEXER_H
#define LEXER_H

#include "diag.h"

#define TOKEN_ARR_CHUNK_CAPACITY 4
#define DIGITS "0123456789"

#define KEYWORDS_SIZE 1

extern const char* const KEYWORDS[KEYWORDS_SIZE];

typedef enum {
    TT_INT = 0,
    TT_FLOAT,
    TT_IDENTIFIER,
    TT_KEYWORD,
    TT_EQUAL,
    TT_COLON,
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
    Span start;
    Span end;
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

int token_array_append_operator(TokenArray* arr, TokenType type, const char* val, Span pos);
int token_array_append(TokenArray* arr, TokenType type, const char* val, Span start, Span end);

// make sure the free(result) is called after calling fmt_token(...)!
char* fmt_token(Token self);

int tokcmp(Token a, TokenType t, const char* v);

Lexer* lexer_new(const char* source);

void lexer_advance(Lexer* self);

int lexer_make_number(Lexer* self);

int lexer_make_identifier(Lexer* self);

TokenArray* lexer_make_tokens(Lexer* self);

void token_array_free(TokenArray* arr);

void lexer_free(Lexer* self);

#endif