#include "lexer.h"
#include "dstring.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

TokenArray* token_array_new() {
    TokenArray* arr = (TokenArray*)malloc(sizeof(TokenArray));
    if (!arr) return NULL;

    arr->capacity = TOKEN_ARR_CHUNK_CAPACITY;
    arr->size = 0;
    arr->data = (Token*)malloc(arr->capacity * sizeof(Token));
    
    if (!arr->data) {
        free(arr);
        return NULL;
    }
    return arr;
}

int token_array_append(TokenArray* arr, TokenType type, const char* val, Span pos) {
    if (arr->size >= arr->capacity) {
        int new_cap = arr->capacity + TOKEN_ARR_CHUNK_CAPACITY;
        Token* temp = (Token*)realloc(arr->data, new_cap * sizeof(Token));
        if (!temp) {
            printf("Token array extend failure\n");
            return 0;
        }
        arr->data = temp;
        arr->capacity = new_cap;
    }

    arr->data[arr->size].type = type;
    arr->data[arr->size].value = strdup(val);
    arr->data[arr->size].pos = pos;
    arr->size++;
    
    return 1;
}

char* fmt_token(Token self) {
    int length = snprintf(NULL, 0, "[%d: %s]", (int)self.type, self.value);

    // allocate exact memory (+1 for the null terminator)
    char *result = malloc(length + 1);

    if (result != NULL) {
        snprintf(result, length + 1, "[%d: %s]", (int)self.type, self.value);
        return result;
    }
}

Lexer* lexer_new(const char* source) {
    long len = strlen(source);
    if (len <= 0) {
        printf("Can't create lexer, the source length is 0!\n");
        return NULL;
    }

    Lexer* l = (Lexer*)malloc(sizeof(Lexer));
    if (l == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    l->source = source;
    l->source_len = len;
    l->pos = 0;
    l->current = source[0];
    l->tokens = token_array_new();
    l->span = (Span){1, 1};

    return l;
}

void lexer_advance(Lexer* self) {
    self->pos++;
    self->span.column++;

    if (self->pos >= 0 && (size_t)self->pos < self->source_len) {
        self->current = self->source[self->pos];
    } else {
        self->current = '\0';
    }
}

void lexer_make_number(Lexer* self) {
    DynamicString* num_str = create_string(1);
    int dot_count = 0;

    while (self->current != '\0' && (
        isdigit(self->current) || 
        self->current == '.'
    )) {
        if (self->current == '.') {
            dot_count += 1;
        }
        append_string(num_str, (char[]){self->current, '\0'});
        lexer_advance(self);
    }

    if (dot_count == 0) {
        token_array_append(self->tokens, TT_INT, num_str->buffer, self->span);
    } else {
        token_array_append(self->tokens, TT_FLOAT, num_str->buffer, self->span);
    }
}

TokenArray* lexer_make_tokens(Lexer* self) {
    while (self->current != '\0') {
        if (strchr("\n", self->current) != NULL) {
            self->span.line++;
            lexer_advance(self);
        }
        if (strchr(" \t", self->current) != NULL) {
            lexer_advance(self);
        } else if (isdigit(self->current)) {
            lexer_make_number(self);
        } else if (self->current == '+') {
            token_array_append(self->tokens, TT_PLUS, "+", self->span); lexer_advance(self);
        } else if (self->current == '-') {
            token_array_append(self->tokens, TT_MINUS, "-", self->span); lexer_advance(self);
        } else if (self->current == '*') {
            token_array_append(self->tokens, TT_MUL, "*", self->span); lexer_advance(self);
        } else if (self->current == '/') {
            token_array_append(self->tokens, TT_DIV, "/", self->span); lexer_advance(self);
        } else if (self->current == '(') {
            token_array_append(self->tokens, TT_LPAREN, "(", self->span); lexer_advance(self);
        } else if (self->current == ')') {
            token_array_append(self->tokens, TT_RPAREN, ")", self->span); lexer_advance(self);
        } else {
            char details[4] = { '\"', self->current, '\"', '\0' };
            const char *ptr = details; 
            show_error(ERROR_UNKNOWN_CHARACTER, self->span, ptr);
            return NULL;
        }
        
    }
    //token_array_append(self->tokens, TT_EOF, " ", self->span);
    return self->tokens;
}

void token_array_free(TokenArray* arr) {
    if (arr == NULL) return;

    for (int i = 0; i < arr->size; i++) {
        if (arr->data[i].type == TT_INT || arr->data[i].type == TT_FLOAT) {
            free((void*)arr->data[i].value);
        }
    }

    if (arr->data != NULL) {
        free(arr->data);
    }

    free(arr);
}

void lexer_free(Lexer* self) {
    if (self == NULL) return;

    if (self->tokens != NULL) {
        token_array_free(self->tokens);
    }

    free(self);
}
