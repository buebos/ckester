#ifndef __CKESTER_COMPONENTS_STRING_C__
#define __CKESTER_COMPONENTS_STRING_C__

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Ckester_String {
    size_t length;
    size_t capacity;
    char *data;
} Ckester_String;

typedef struct Ckester_StringInitParams {
    size_t capacity;
    char* initial_value;
} Ckester_StringInitParams;

#define CKESTER_STRING_DEFAULT_CAPACITY 16

/* Simple glob matching: * matches any sequence */
bool ckester_string_match(const char *str, const char *pattern) {
    if (!pattern) return false;
    if (!str) return false;
    while (*pattern) {
        if (*pattern == '*') {
            while (*pattern == '*') pattern++;
            if (!*pattern) return true;
            while (*str) {
                if (ckester_string_match(str, pattern)) return true;
                str++;
            }
            return false;
        } else if (*str == *pattern || *pattern == '?') {
            if (!*str) return false;
            str++;
            pattern++;
        } else {
            return false;
        }
    }
    return !*str;
}

static void _ensure_capacity(Ckester_String *str, size_t needed) {
    if (str->length + needed < str->capacity) {
        return;
    }

    size_t new_capacity = str->capacity == 0 ? CKESTER_STRING_DEFAULT_CAPACITY : str->capacity;
    while (str->length + needed >= new_capacity) {
        new_capacity *= 2;
    }

    char *new_data = realloc(str->data, new_capacity * sizeof(char));
    if (new_data == NULL) {
        return; 
    }

    str->data = new_data;
    str->capacity = new_capacity;
}

Ckester_String ckester_string_init(Ckester_StringInitParams options) {
    Ckester_String str;
    str.length = 0;
    str.capacity = options.capacity > 0 ? options.capacity : CKESTER_STRING_DEFAULT_CAPACITY;
    str.data = calloc(str.capacity, sizeof(char));
    
    if (str.data) {
        str.data[0] = '\0';
    }
    return str;
}

void ckester_string_push_char(Ckester_String *str, char c) {
    _ensure_capacity(str, 1);
    
    if (!str->data) return;

    str->data[str->length] = c;
    str->length++;
    str->data[str->length] = '\0';
}

void ckester_string_push(Ckester_String *str, const char *other) {
    if (other == NULL) return;

    size_t len = strlen(other);
    _ensure_capacity(str, len);

    if (!str->data) return;

    strcpy(str->data + str->length, other);
    str->length += len;
}

void ckester_string_clear(Ckester_String *str) {
    str->length = 0;
    if (str->data && str->capacity > 0) {
        str->data[0] = '\0';
    }
}

void ckester_string_free(Ckester_String *str) {
    if (str->data) {
        free(str->data);
        str->data = NULL;
    }
    str->length = 0;
    str->capacity = 0;
}

#endif