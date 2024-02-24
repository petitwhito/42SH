#define _POSIX_C_SOURCE 200809L

#include "string_utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int string_append_char(char **str, size_t size, char c)
{
    char *tmp = realloc(*str, size + 2);
    if (!tmp)
        return -1;
    *str = tmp;
    (*str)[size] = c;
    (*str)[size + 1] = '\0';
    return 0;
}

int string_insert_substr(char **base_str, char *to_insert, size_t pos)
{
    if (!*base_str || !to_insert)
        return -1;

    size_t len_insert = strlen(to_insert);
    // Nothing to insert if it is empty string
    if (len_insert == 0)
        return 0;

    size_t len_base = strlen(*base_str);

    // Limit position to end of string max
    if (pos > len_base)
        pos = len_base;

    size_t new_len = len_base + len_insert;
    *base_str = realloc(*base_str, new_len + 2);
    // Shift right part of the base string to the right according to the pos
    memmove(*base_str + pos + len_insert, *base_str + pos, len_base - pos);

    // Insert the to_insert string in the base_str at pos index
    memmove(*base_str + pos, to_insert, len_insert);

    (*base_str)[new_len] = '\0';
    return 0;
}

int string_remove_trailing_newline(char **base_str, size_t *size)
{
    if (!*base_str || *size == 0)
        return -1;

    int i = *size - 1;
    while (i >= 0 && (*base_str)[i] == '\n')
    {
        (*base_str)[i] = '\0';
        (*size)--;
        i--;
    }

    *base_str = realloc(*base_str, *size + 1);
    return 0;
}

int is_number(char *word)
{
    for (size_t i = 0; word[i] != '\0'; ++i)
    {
        if (!isdigit(word[i]))
            return 0;
    }

    return 1;
}

static int get_digit_number(int n)
{
    if (n < 10)
        return 1;
    return 1 + get_digit_number(n / 10);
}

char *int_to_str(int n)
{
    int nb = get_digit_number(n);
    char *buffer = malloc(sizeof(char) * (nb + 1));
    if (!buffer)
        return NULL;

    snprintf(buffer, nb + 1, "%d", n);
    return buffer;
}
