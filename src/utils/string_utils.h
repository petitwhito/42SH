#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>

struct var;

int string_append_char(char **str, size_t size, char c);

int string_insert_substr(char **base_str, char *to_insert, size_t pos);

int string_remove_trailing_newline(char **base_str, size_t *size);

int is_number(char *word);

char *int_to_str(int n);

#endif /* ! STRING_UTILS_H */
