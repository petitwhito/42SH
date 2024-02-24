#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"
#include "environment/environment.h"
#include "environment/environment_func.h"
#include "utils/argv_utils.h"
#include "utils/hash_map.h"
#include "utils/hash_map_func.h"

static int compute_option(size_t argc, char **argv, size_t *j)
{
    int option = 0;

    while (*j < argc && strlen(argv[*j]) > 1 && argv[*j][0] == '-')
    {
        size_t i = 1;
        while (i < strlen(argv[*j]))
        {
            if (argv[*j][i] != 'v' && argv[*j][i] != 'f')
            {
                fprintf(stderr, "unset: unrecognized option -%c\n",
                        argv[*j][i]);
                return -2;
            }

            if (argv[*j][i] == 'v')
            {
                if (option == 2)
                {
                    fprintf(stderr,
                            "unset: cannot unset a function and a variable\n");
                    return -1;
                }

                option = 1;
            }
            else if (argv[*j][i] == 'f')
            {
                if (option == 1)
                {
                    fprintf(stderr,
                            "unset: cannot unset a function and a variable\n");
                    return -1;
                }

                option = 2;
            }

            i++;
        }

        (*j)++;
    }

    return option;
}

struct exit_status builtin_unset(char **argv)
{
    size_t len = argv_len(argv);
    if (len == 1)
        return (struct exit_status){ 0, EXIT_OK };

    // Compute option (-v xor -f)
    size_t j = 1;
    int option = compute_option(len, argv, &j);
    if (option < 0)
        return (struct exit_status){ -option, EXIT_ERROR };

    // Unset variable from hash_map when there is no option or -v is specified
    if (option <= 1)
    {
        struct hash_map *hash_map = get_hash_map();
        // Starts i after option if specified
        for (size_t i = j; i < len; ++i)
        {
            // Error when trying to unset readonly variable
            if (strcmp(argv[i], "UID") == 0)
            {
                fprintf(stderr, "unset: cannot unset UID: readonly variable\n");
                return (struct exit_status){ 1, EXIT_ERROR };
            }

            // If variable wasn't in env, check if in hash map and removes it
            char *env = getenv(argv[i]);
            if (env)
                unsetenv(argv[i]);
            else
            {
                if (hash_map_remove(hash_map, argv[i]))
                {
                    // If RANDOM variable was unset, it loses its special
                    // properties
                    if (strcmp(argv[i], "RANDOM") == 0)
                        set_random_state();
                }
            }
        }
    }
    else
    {
        struct hash_map_func *hash_map_func = get_hash_map_func();
        for (size_t i = j; i < len; ++i)
            hash_map_func_remove(hash_map_func, argv[i]);
    }

    return (struct exit_status){ 0, EXIT_OK };
}
