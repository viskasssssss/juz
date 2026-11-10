#include "parser.h"

#include <stdio.h>
#include <string.h>

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
    else if (self->type == NODE_UNARYOP) {
        UnaryOpNode* un = (UnaryOpNode*) self;
        char* fmt_node = fmt_ast_node(un->node);
        int length = snprintf(NULL, 0, 
            "(%d %s)", 
            un->op_tok.type, 
            fmt_node
        );

        // allocate exact memory (+1 for the null terminator)
        char *result = malloc(length + 1);

        if (result != NULL) {
            snprintf(result, length + 1, 
                "(%d %s)", 
                un->op_tok.type, 
                fmt_node
            );
            free(fmt_node);
            return result;
        }
        free(fmt_node);
    }
    else if (self->type == NODE_VAR_ACCESS) {
        VarAccessNode* var = (VarAccessNode*) self;
        int length = snprintf(NULL, 0, "%s", var->name.value);

        // allocate exact memory (+1 for the null terminator)
        char *result = malloc(length + 1);

        if (result != NULL) {
            snprintf(result, length + 1,  "%s", var->name.value);
            return result;
        }
    }
    else if (self->type == NODE_VAR_DEF) {
        VarDefNode* var = (VarDefNode*) self;
        char* fmt_val = fmt_ast_node(var->val);
        int length = snprintf(NULL, 0, "%s %s = %s", var->type.value, var->name.value, fmt_val);

        // allocate exact memory (+1 for the null terminator)
        char *result = malloc(length + 1);

        if (result != NULL) {
            snprintf(result, length + 1, "%s %s = %s", var->type.value, var->name.value, fmt_val);
            return result;
        }
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
    l->error = (Error) {
        ERROR_NONE,
        tokens->data[0].start,
        tokens->data[0].end,
        "",
        0,
    };

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
    if (self->error.error) {
        show_error_struct(self->error);
        return NULL;
    }
    
    if (self->current.type != TT_EOF) {
        show_error(
            ERROR_INVALID_SYNTAX,
            self->current.start, self->current.end,
            "Expected an expression"
        );
        return NULL;
    }
    return res;
}

ASTNode* parser_bin_op(Parser* self, ASTNode* (*func)(Parser*), TokenType (*ops)[2]) {
    ASTNode* left = func(self);
    if (self->error.error) return NULL;

    while (self->current.type == (*ops)[0] ||
        self->current.type == (*ops)[1]) {
            Token op = self->current;
            parser_advance(self);
            ASTNode* right = func(self);

            if (self->error.error) return NULL;
            
            BinOpNode* bin = (BinOpNode*) malloc(sizeof(BinOpNode));
            bin->base.type = NODE_BINOP;
            bin->left = left;
            bin->op_tok = op;
            bin->right = right;
            left = (ASTNode*) bin;
    }

    return left;
}

// atom = INT|FLOAT|IDENTIFIER
//      = LPAREN expr RPAREN
ASTNode* parser_atom(Parser* self) {
    Token tok = self->current;

    if (tok.type == TT_INT ||
        tok.type == TT_FLOAT) {
            parser_advance(self);
            NumberNode* num = (NumberNode*) malloc(sizeof(NumberNode));
            num->token = tok;
            num->base = (ASTNode) { NODE_NUMBER };
            return (ASTNode*) num;
    }
    else if (tok.type == TT_IDENTIFIER) {
        parser_advance(self);
        VarAccessNode* var = (VarAccessNode*) malloc(sizeof(VarAccessNode));
        var->name = tok;
        var->base = (ASTNode) { NODE_VAR_ACCESS };
        return (ASTNode*) var;
    }
    else if (tok.type == TT_LPAREN) {
        parser_advance(self);
        ASTNode* expr = parser_expr(self);
        if (self->error.error) return NULL;
        if (self->current.type == TT_RPAREN) {
            parser_advance(self);
            return expr;
        } else {
            self->error = (Error) {
                ERROR_INVALID_SYNTAX,
                self->current.start,
                self->current.end,
                "Expected \")\"", // TODO: but got...
                1
            };
            return NULL;
        }
    }

    self->error = (Error) {
        ERROR_INVALID_SYNTAX,
        tok.start,
        tok.end,
        "Expected expression", // TODO: but got...
        1
    };

    return NULL;
}

// factor = (PLUS|MINUS) factor
ASTNode* parser_factor(Parser* self) {
    Token tok = self->current;

    if (tok.type == TT_PLUS ||
        tok.type == TT_MINUS) {
            parser_advance(self);
            ASTNode* factor = parser_factor(self);
            if (self->error.error) return NULL;
            UnaryOpNode* un = (UnaryOpNode*) malloc(sizeof(UnaryOpNode));
            un->op_tok = tok;
            un->node = factor;
            un->base = (ASTNode) { NODE_UNARYOP };
            return (ASTNode*) un;
    }

    return parser_atom(self);
}

// term = factor ((MUL|DIV)) factor)*
ASTNode* parser_term(Parser* self) {
    static TokenType ops[2] = {TT_MUL, TT_DIV};
    return parser_bin_op(self, parser_factor, &ops);
}

// expr = KEYWORD:let IDENTIFIER EQ expr
//      = term ((PLUS|MINUS)) term)*
ASTNode* parser_expr(Parser* self) {
    if (tokcmp(self->current, TT_KEYWORD, "let")) {
        // let
        parser_advance(self);

        // var name
        if (self->current.type != TT_IDENTIFIER) {
            self->error = (Error) {
                ERROR_INVALID_SYNTAX,
                self->current.start,
                self->current.end,
                "Expected variable name", // TODO: but got...
                1
            };
            return NULL;
        }

        Token name = self->current;
        parser_advance(self);

        // :
        if (self->current.type != TT_COLON) {
            self->error = (Error) {
                ERROR_INVALID_SYNTAX,
                self->current.start,
                self->current.end,
                "Expected variable type", // TODO: but got...
                1
            };
            return NULL;
        }

        parser_advance(self);

        // type
        if (self->current.type != TT_IDENTIFIER) {
            self->error = (Error) {
                ERROR_INVALID_SYNTAX,
                self->current.start,
                self->current.end,
                "Expected variable name", // TODO: but got...
                1
            };
            return NULL;
        }

        Token type = self->current;
        parser_advance(self);

        // =
        if (self->current.type != TT_EQUAL) {
            self->error = (Error) {
                ERROR_INVALID_SYNTAX,
                self->current.start,
                self->current.end,
                "Expected definition", // TODO: but got...
                1
            };
            return NULL;
        }

        parser_advance(self);

        ASTNode* expr = parser_expr(self);
        if (self->error.error) return NULL;

        VarDefNode* var = (VarDefNode*) malloc(sizeof(VarDefNode));
        var->name = name;
        var->val = expr;
        var->type = type;
        var->base = (ASTNode) { NODE_VAR_DEF };
        return (ASTNode*) var;
    }

    static TokenType ops[2] = {TT_PLUS, TT_MINUS};
    return parser_bin_op(self, parser_term, &ops);
}