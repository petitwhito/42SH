#define _POSIX_C_SOURCE 200809L

#include "expansion.h"

#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "environment/var.h"
#include "execution/execution.h"
#include "utils/argv_utils.h"
#include "utils/hash_map.h"
#include "word_expansion.h"

enum expansion_status words_expansion(char ***words)
{
    size_t i = 0;
    while ((*words)[i])
    {
        // Expand a word into one or multiple new words
        struct expansion_res exp_res = expand_word((*words)[i]);

        // Check if error or exit happened
        if (exp_res.status != EXPANSION_OK)
            return exp_res.status;

        // Shift i by the amount of new words added to avoid re-expanding
        // a word that we just added
        //  Remove the non-expanded word and replace it with the new words
        argv_remove(words, i);
        if (exp_res.new_words)
        {
            merge_argv(words, exp_res.new_words, i);
            i += argv_len(exp_res.new_words) - 1;
        }
        else
            i--;

        free_argv(exp_res.new_words);
        i++;
    }
    return EXPANSION_OK;
}

enum expansion_status var_assignation(struct var *begin)
{
    struct hash_map *var_hash = get_hash_map();
    struct var *tmp = begin;
    while (tmp)
    {
        // If we're trying to set UID, throw an error because UID is readonly
        if (strcmp(tmp->key, "UID") == 0)
            return EXPANSION_ERROR;

        // Variable Expansion
        struct expansion_res exp_res = expand_word(tmp->value);

        // Check if error or need to exit
        if (exp_res.status != EXPANSION_OK)
            return exp_res.status;

        // If multiple words are given, concat them all in one word
        char *val = argv_to_word(exp_res.new_words);

        // Set RANDOM variable seed
        if (strcmp(tmp->key, "RANDOM") == 0)
            srand(atoi(tmp->value));

        // Retrieve and backup previous value of the variable in the var struct
        int env = 1;
        char *prev_value = getenv(tmp->key);
        if (!prev_value)
        {
            env = 0;
            prev_value = hash_map_get(var_hash, tmp->key);
        }
        if (prev_value)
            tmp->prev_value = strdup(prev_value);

        // Variable Assignation
        if (!env)
        {
            int b = 0;
            hash_map_insert(var_hash, strdup(tmp->key), strdup(val), &b);
        }
        else
            setenv(tmp->key, val, 1);

        free(val);
        free_argv(exp_res.new_words);
        tmp = tmp->next;
    }
    return EXPANSION_OK;
}
