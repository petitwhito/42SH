#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"
#include "environment/environment.h"
#include "environment/var.h"
#include "utils/argv_utils.h"
#include "utils/hash_map.h"

struct exit_status builtin_unalias(char **argv)
{
    // If no arguments, do nothing
    size_t len = argv_len(argv);
    if (len == 1)
    {
        fprintf(stderr, "unalias: expecting arguments\n");
        return (struct exit_status){ 2, EXIT_OK };
    }

    // Compute option
    int option_all = 0;
    size_t i = 1;
    while (i < len && strlen(argv[i]) > 1 && argv[i][0] == '-')
    {
        size_t j = 1;
        while (j < strlen(argv[i]))
        {
            if (argv[i][j] == 'a')
                option_all = 1;
            else
            {
                fprintf(stderr, "unalias: unexpected option: %s\n", argv[i]);
                return (struct exit_status){ 2, EXIT_OK };
            }

            ++j;
        }

        ++i;
    }

    struct hash_map *alias_hash_map = get_alias_hash_map();
    if (option_all)
    {
        for (size_t i = 0; i < alias_hash_map->size; ++i)
            empty_list(alias_hash_map, alias_hash_map->data[i]);
    }
    else
    {
        while (i < len)
        {
            hash_map_remove(alias_hash_map, argv[i]);
            ++i;
        }
    }

    return (struct exit_status){ 0, EXIT_OK };
}
