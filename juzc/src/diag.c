#include "diag.h"
#include <stdio.h>

const char* error_type_to_string(ErrorType t) {
    switch (t) {
        case ERROR_UNKNOWN_CHARACTER: return "Unknown Character";
        case ERROR_INVALID_SYNTAX: return "Invalid Syntax";
        default: return "UNKNOWN";
    }
}

char* fmt_error(Error self) {
    const char* error_t = error_type_to_string(self.type);
    int length = snprintf(NULL, 0, 
        "%s: %s at ln %d col %d", 
        error_t, 
        self.details, 
        self.pos.line, self.pos.column
    );

    // allocate exact memory (+1 for the null terminator)
    char *result = malloc(length + 1);

    if (result != NULL) {
        snprintf(result, length + 1, 
            "%s: %s at ln %d col %d", 
            error_t, 
            self.details, 
            self.pos.line, self.pos.column
        );
        return result;
    }
}

void show_error(ErrorType type, Span pos, const char* details) {
    Error error = (Error) {
        type,
        pos,
        details
    };

    char* res = fmt_error(error);
    printf("%s\n", res);

    free(res);
}