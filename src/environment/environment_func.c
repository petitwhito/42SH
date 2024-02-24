#include "environment_func.h"

#include <stdlib.h>

#include "ast/ast.h"
#include "utils/hash_map_func.h"

// This file stores the global function hash_map

static struct hash_map_func *function_hash_map;

void init_hash_map_func(void)
{
    function_hash_map = hash_map_func_init(HASH_SIZE);
}

struct hash_map_func *get_hash_map_func(void)
{
    return function_hash_map;
}

void free_hash_map_func(void)
{
    hash_map_func_free(function_hash_map);
}
