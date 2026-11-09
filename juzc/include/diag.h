#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>

#define ANSI_RED     "\x1b[31m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_RESET   "\x1b[0m"
#define ANSI_BOLD    "\x1b[1m"

typedef enum {
    ERROR_NONE,
    ERROR_UNKNOWN_CHARACTER,
    ERROR_INVALID_SYNTAX,
} ErrorType;

typedef struct {
    size_t line;
    size_t column;
} Span;

typedef struct {
    ErrorType type;
    Span start;
    Span end;
    const char* details;
    int error;
} Error;

int diag_setup(const char* source);

void diag_free(void);

void pretty_error(const char *filename, size_t err_start, size_t err_end, const char *msg);

const char* error_type_to_string(ErrorType t);

char* fmt_error(Error self);

void show_error(ErrorType type, Span start, Span end, const char* details);

void show_error_struct(Error e);

#endif