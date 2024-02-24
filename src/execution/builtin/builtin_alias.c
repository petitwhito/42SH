#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"
#include "environment/environment.h"
#include "environment/var.h"
#include "expansion/word_expansion.h"
#include "utils/argv_utils.h"
#include "utils/hash_map.h"

struct exit_status builtin_alias(char **argv)
{
    // If no arguments, do nothing
    size_t len = argv_len(argv);
    if (len == 1)
        return (struct exit_status){ 0, EXIT_OK };

    struct hash_map *alias_hash_map = get_alias_hash_map();
    for (size_t i = 1; i < len; ++i)
    {
        struct var *var = append_var(NULL, argv[i]);

        // If argv[i] is not an assignation
        if (!var)
        {
            char *value = hash_map_get(alias_hash_map, argv[i]);
            if (value)
                printf("%s='%s'\n", argv[i], value);
            else
            {
                fprintf(stderr, "alias: %s: not found\n", argv[i]);
                free_var(var);
                return (struct exit_status){ 1, EXIT_OK };
            }
        }
        else
        {
            if (!is_valid_alias(var->key))
            {
                fprintf(stderr, "alias: invalid alias name: %s\n", var->key);
                free_var(var);
                return (struct exit_status){ 1, EXIT_OK };
            }

            // Expand alias value
            struct expansion_res exp_res = expand_word(var->value);
            if (exp_res.status != EXPANSION_OK)
            {
                fprintf(stderr, "alias: cannot expand %s\n", var->value);
                free_var(var);
                free_argv(exp_res.new_words);
                return (struct exit_status){ 1, EXIT_OK };
            }

            // If multiple words are given, concat them all in one word
            char *val = argv_to_word(exp_res.new_words);
            int b = 0;
            hash_map_insert(alias_hash_map, strdup(var->key), strdup(val), &b);
            free(val);
            free_argv(exp_res.new_words);
        }

        free_var(var);
    }

    return (struct exit_status){ 0, EXIT_OK };
}
