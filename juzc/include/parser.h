#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum {
    NODE_NUMBER,
    NODE_BINOP
} NodeType;

typedef struct {
    NodeType type;
} ASTNode;

typedef struct {
    ASTNode base;
    Token token;
} NumberNode;

typedef struct {
    ASTNode base;
    ASTNode* left;
    Token op_tok;
    ASTNode* right;
} BinOpNode;

typedef struct {
    TokenArray* tokens;
    Token current;
    long pos;
} Parser;

char* fmt_ast_node(ASTNode* self);

Parser* parser_new(TokenArray* tokens);

ASTNode* parser_parse(Parser* self);

ASTNode* parser_bin_op(Parser* self, ASTNode* (*func)(Parser*), TokenType (*ops)[2]);

ASTNode* parser_factor(Parser* self);

ASTNode* parser_term(Parser* self);

ASTNode* parser_expr(Parser* self);

#endif