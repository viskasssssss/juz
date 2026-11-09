#include "shell.h"
#include "lexer.h"
#include "parser.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int shell_run() {
    while (1) {
        char input[256];
        printf("juz > ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\r\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            printf("Stopping...");
            break;
        }

        Lexer* lex = lexer_new(input);
        if (lex == NULL) {
            printf("Something went wrong, continuing...\n");
            continue;
        }
        TokenArray* tokens = lexer_make_tokens(lex);
        if (tokens == NULL) {
            printf("Something went wrong, continuing...\n");
            continue;
        }
        for (int i = 0; i < tokens->size; i++) {
            Token t = tokens->data[i];
            char* fmt = fmt_token(t);
            if (fmt != NULL) {
                printf("%s ", fmt);
                free(fmt);
            }
        }
        printf("\n");
        Parser* par = parser_new(tokens);
        if (par == NULL) {
            printf("Something went wrong, continuing...\n");
            continue;
        }
        ASTNode* ast = parser_parse(par);
        char* fmt_ast = fmt_ast_node(ast);
        if (fmt_ast != NULL) {
            printf("%s ", fmt_ast);
            free(fmt_ast);
        }
        printf("\n");
        lexer_free(lex);
        free(par);
    }
}