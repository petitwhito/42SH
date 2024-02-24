#ifndef HASH_MAP_FUNC_H
#define HASH_MAP_FUNC_H

#include <stddef.h>

#include "ast/ast_fundec.h"

struct pair_list_func
{
    char *key;
    struct ast_node *value;
    struct pair_list_func *next;
};

struct hash_map_func
{
    size_t size;
    struct pair_list_func **data;
};

struct hash_map_func *hash_map_func_init(size_t size);

int hash_map_func_insert(struct hash_map_func *hash_map, char *key,
                         struct ast_node *value, int *updated);

void hash_map_func_free(struct hash_map_func *hash_map);

struct ast_node *hash_map_func_get(const struct hash_map_func *hash_map,
                                   char *key);

int hash_map_func_remove(struct hash_map_func *hash_map, char *key);

#endif /* ! HASH_MAP_FUNC_H */
