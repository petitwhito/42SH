#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"
#include "environment/environment.h"
#include "environment/var.h"
#include "expansion/expansion.h"
#include "utils/argv_utils.h"
#include "utils/hash_map.h"

struct exit_status builtin_export(char **argv)
{
    size_t len = argv_len(argv);
    if (len == 1)
        return (struct exit_status){ 0, EXIT_OK };

    struct hash_map *hash_map = get_hash_map();
    size_t i = 1;
    while (i < len)
    {
        struct var *var = append_var(NULL, argv[i]);

        // If argv[i] is not an assignation
        if (!var)
        {
            char *value = hash_map_get(hash_map, argv[i]);
            // If value in hash map and it's not RANDOM
            if (value && strcmp(argv[i], "RANDOM") != 0)
            {
                setenv(argv[i], value, 1);
                hash_map_remove(hash_map, argv[i]);
            }
        }
        else
        {
            if (var_assignation(var) == EXPANSION_ERROR)
            {
                fprintf(stderr, "export: cannot export %s\n", var->key);
                free_var(var);
                return (struct exit_status){ 1, EXIT_ERROR };
            }

            // If not RANDOM variable then remove from hash_map and setenv
            if (strcmp(var->key, "RANDOM") != 0)
            {
                setenv(var->key, var->value, 1);
                hash_map_remove(hash_map, var->key);
            }
        }

        free_var(var);
        i++;
    }

    return (struct exit_status){ 0, EXIT_OK };
}
