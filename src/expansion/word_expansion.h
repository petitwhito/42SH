#ifndef WORD_EXPANSION_H
#define WORD_EXPANSION_H

#include <stddef.h>

#include "expansion.h"

struct expansion_data
{
    char *word;
    size_t len;
    size_t i;
    char *buffer;
    size_t buf_len;
    char **new_words;
    size_t word_count;
    int empty_quotes_in_word;
};

struct expansion_res expand_word(char *word);

#endif /* ! WORD_EXPANSION_H */
