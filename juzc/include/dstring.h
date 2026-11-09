#ifndef STRING_H
#define STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *buffer;
    size_t length;
    size_t capacity;
} DynamicString;

DynamicString* create_string(size_t initial_capacity);

int append_string(DynamicString *ds, const char *text);

void free_string(DynamicString *ds);

#endif