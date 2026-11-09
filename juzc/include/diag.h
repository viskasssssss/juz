#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>

typedef enum {
    ERROR_UNKNOWN_CHARACTER,
    ERROR_INVALID_SYNTAX,
} ErrorType;

typedef struct {
    size_t line;
    size_t column;
} Span;

typedef struct {
    ErrorType type;
    Span pos;
    const char* details;
} Error;

const char* error_type_to_string(ErrorType t);

char* fmt_error(Error self);

void show_error(ErrorType type, Span pos, const char* details);


#endif