#include "parser.h"

#include <stdio.h>

char* fmt_ast_node(ASTNode* self) {
    if (self == NULL)
        return NULL;
    if (self->type == NODE_NUMBER) {
        NumberNode* num = (NumberNode*) self;
        int length = snprintf(NULL, 0, "%s", num->token.value);

        // allocate exact memory (+1 for the null terminator)
        char *result = malloc(length + 1);

        if (result != NULL) {
            snprintf(result, length + 1,  "%s", num->token.value);
            return result;
        }
    }
    else if (self->type == NODE_BINOP) {
        BinOpNode* bin = (BinOpNode*) self;
        char* fmt_left = fmt_ast_node(bin->left);
        char* fmt_right = fmt_ast_node(bin->right);
        int length = snprintf(NULL, 0, 
            "(%d %s %s)", 
            bin->op_tok.type, 
            fmt_left, fmt_right
        );

        // allocate exact memory (+1 for the null terminator)
        char *result = malloc(length + 1);

        if (result != NULL) {
            snprintf(result, length + 1, 
                "(%d %s %s)", 
                bin->op_tok.type, 
                fmt_left, fmt_right
            );
            free(fmt_left);
            free(fmt_right);
            return result;
        }
        free(fmt_left);
        free(fmt_right);
    }
    return NULL;
}

// Make sure tokens are deleted! Parser doesn't delete them
Parser* parser_new(TokenArray* tokens) {
    if (tokens->size <= 0) {
        printf("Can't create parser, the token array length is 0!\n");
        return NULL;
    }

    Parser* l = (Parser*)malloc(sizeof(Parser));
    if (l == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    l->tokens = tokens;
    l->pos = 0;
    l->current = tokens->data[0];

    return l;
}

Token parser_advance(Parser* self) {
    self->pos++;
    
    if (self->pos < self->tokens->size) {
        self->current = self->tokens->data[self->pos];
    }
    return self->current;
}

ASTNode* parser_parse(Parser* self) {
    ASTNode* res = parser_expr(self);
    return res;
}

ASTNode* parser_bin_op(Parser* self, ASTNode* (*func)(Parser*), TokenType (*ops)[2]) {
    ASTNode* left = func(self);

    while (self->current.type == (*ops)[0] ||
        self->current.type == (*ops)[1]) {
            Token op = self->current;
            parser_advance(self);
            ASTNode* right = func(self);
            
            BinOpNode* bin = (BinOpNode*) malloc(sizeof(BinOpNode));
            bin->base.type = NODE_BINOP;
            bin->left = left;
            bin->op_tok = op;
            bin->right = right;
            left = (ASTNode*) bin;
    }

    return left;
}

// factor = INT|FLOAT
ASTNode* parser_factor(Parser* self) {
    Token tok = self->current;

    if (tok.type == TT_INT ||
        tok.type == TT_FLOAT) {
            parser_advance(self);
            NumberNode* num = (NumberNode*) malloc(sizeof(NumberNode));
            num->token = tok;
            num->base = (ASTNode) { NODE_NUMBER };
            return (ASTNode*) num;
    }
    return NULL;
}

// term = factor ((MUL|DIV)) factor)*
ASTNode* parser_term(Parser* self) {
    static TokenType ops[2] = {TT_MUL, TT_DIV};
    return parser_bin_op(self, parser_factor, &ops);
}

// expr = term ((PLUS|MINUS)) term)*
ASTNode* parser_expr(Parser* self) {
    static TokenType ops[2] = {TT_PLUS, TT_MINUS};
    return parser_bin_op(self, parser_term, &ops);
}