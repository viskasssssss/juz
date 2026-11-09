#include "diag.h"
#include <stdio.h>

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
        "%s: %s at ln %d>%d col %d>%d", 
        error_t, 
        self.details, 
        self.start.line, self.end.line,
        self.start.column, self.end.column
    );

    // allocate exact memory (+1 for the null terminator)
    char *result = malloc(length + 1);

    if (result != NULL) {
        snprintf(result, length + 1, 
            "%s: %s at ln %d>%d col %d>%d", 
            error_t, 
            self.details, 
            self.start.line, self.end.line,
            self.start.column, self.end.column
        );
        return result;
    }
}

void show_error(ErrorType type, Span start, Span end, const char* details) {
    Error error = (Error) {
        type,
        start,
        end,
        details,
        1,
    };

    char* res = fmt_error(error);
    printf("%s\n", res);

    free(res);
}

void show_error_struct(Error e) {
    show_error(
        e.type,
        e.start,
        e.end,
        e.details
    );
}