#include "diag.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char* source = "NONE";

int diag_setup(const char* s) {
    source = strdup(s); // not ideal, but works
}

void diag_free(void) {
    free(source);
}

size_t span_to_offset(Span span) {
    size_t current_line = 1;
    size_t i = 0;

    while (current_line < span.line && source[i] != '\0') {
        if (source[i] == '\n') {
            current_line++;
        }
        i++;
    }

    if (source[i] == '\0' && current_line < span.line) {
        return (size_t)-1; 
    }

    return i + (span.column - 1);
}


void pretty_error(const char *filename, size_t err_start, size_t err_end, const char *msg) {
    size_t line_num = 1;
    size_t line_start = 0;
    size_t i = 0;

    while (i < err_start && source[i] != '\0') {
        if (source[i] == '\n') {
            line_num++;
            line_start = i + 1;
        }
        i++;
    }
    size_t col_num = err_start - line_start + 1;

    size_t line_end = line_start;
    while (source[line_end] != '\n' && source[line_end] != '\0') {
        line_end++;
    }

    printf(ANSI_BOLD ANSI_RED "error: " ANSI_RESET ANSI_BOLD "%s\n" ANSI_RESET, msg);
    printf(ANSI_CYAN "  --> " ANSI_RESET "%s:%zu:%zu\n", filename, line_num, col_num);
    printf(ANSI_CYAN "   |\n" ANSI_RESET);

    printf(ANSI_CYAN "%3zu| " ANSI_RESET, line_num);
    for (size_t j = line_start; j < line_end; j++) {
        putchar(source[j]);
    }
    putchar('\n');

    printf(ANSI_CYAN "   | " ANSI_RED);
    for (size_t j = line_start; j < err_start; j++) {
        putchar(source[j] == '\t' ? '\t' : ' ');
    }
    
    size_t span_len = err_end - err_start;
    if (span_len == 0) span_len = 1;
    for (size_t j = 0; j < span_len; j++) {
        putchar('^');
    }
    printf(ANSI_RESET "\n");
    printf(ANSI_CYAN "   |\n" ANSI_RESET);
}

const char* error_type_to_string(ErrorType t) {
    switch (t) {
        case ERROR_NONE: return "None";
        case ERROR_UNKNOWN_CHARACTER: return "Unknown Character";
        case ERROR_INVALID_SYNTAX: return "Invalid Syntax";
        default: return "UNKNOWN";
    }
}

char* fmt_error(Error self) {
    const char* error_t = error_type_to_string(self.type);
    int length = snprintf(NULL, 0, 
        "%s: %s", 
        error_t, 
        self.details
    );

    // allocate exact memory (+1 for the null terminator)
    char *result = malloc(length + 1);

    if (result != NULL) {
        snprintf(result, length + 1, 
            "%s: %s", 
            error_t, 
            self.details
        );
        return result;
    }
    return NULL;
}

void show_error(ErrorType type, Span start, Span end, const char* details) {
    Error error = (Error) {
        type,
        start,
        end,
        details,
        1,
    };

    show_error_struct(error);
}

void show_error_struct(Error e) {
    char* res = fmt_error(e);
    if (res == NULL) {
        printf("Error formatting failed");
        return;
    }
    pretty_error("_", span_to_offset(e.start), span_to_offset(e.end), res);
    free(res);
}