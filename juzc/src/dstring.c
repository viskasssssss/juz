#include "dstring.h"

DynamicString* create_string(size_t initial_capacity) {
    DynamicString *ds = (DynamicString *)malloc(sizeof(DynamicString));
    if (!ds) return NULL;

    ds->capacity = initial_capacity > 0 ? initial_capacity : 16;
    ds->buffer = (char *)malloc(ds->capacity * sizeof(char));
    if (!ds->buffer) {
        free(ds);
        return NULL;
    }
    ds->buffer[0] = '\0';
    ds->length = 0;
    return ds;
}

int append_string(DynamicString *ds, const char *text) {
    size_t text_len = strlen(text);
    size_t required_capacity = ds->length + text_len + 1;

    if (required_capacity > ds->capacity) {
        size_t new_capacity = ds->capacity * 2;
        while (new_capacity < required_capacity) {
            new_capacity *= 2;
        }

        char *new_buffer = (char *)realloc(ds->buffer, new_capacity * sizeof(char));
        if (!new_buffer) return 0;

        ds->buffer = new_buffer;
        ds->capacity = new_capacity;
    }

    strcpy(ds->buffer + ds->length, text);
    ds->length += text_len;
    return 1;
}

int string_compare(const DynamicString *ds1, const DynamicString *ds2) {
    if (!ds1 && !ds2) return 0;
    if (!ds1) return -1;
    if (!ds2) return 1;
    return strcmp(ds1->buffer, ds2->buffer);
}

void free_string(DynamicString *ds) {
    if (ds) {
        free(ds->buffer);
        free(ds);
    }
}