#include "hash_map_func.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast_fundec.h"
#include "environment/environment_func.h"

static size_t hash(char *key)
{
    size_t result = 0;
    size_t i = 0;
    while (key[i])
        result += key[i++];

    return result;
}

struct hash_map_func *hash_map_func_init(size_t size)
{
    struct hash_map_func *h = malloc(sizeof(struct hash_map_func));
    if (h == NULL)
        return NULL;
    h->size = size;
    h->data = calloc(size, sizeof(struct pair_list_func));
    return h;
}

static void free_list_func(struct pair_list_func *pl)
{
    if (pl == NULL)
        return;
    struct pair_list_func *npl = pl->next;
    while (pl != NULL)
    {
        free(pl->key);

        pl->value->ref_counter--;
        free_ast(pl->value);

        free(pl);
        pl = npl;
        if (pl == NULL)
            return;
        npl = pl->next;
    }
}

void hash_map_func_free(struct hash_map_func *hash_map)
{
    if (hash_map == NULL)
        return;
    for (size_t i = 0; i < hash_map->size; i++)
    {
        free_list_func(hash_map->data[i]);
    }
    free(hash_map->data);
    free(hash_map);
}

static int find_func(struct pair_list_func *pl, char *key,
                     struct ast_node *value)
{
    if (pl == NULL)
        return 0;
    if (strcmp(pl->key, key) == 0)
    {
        value->ref_counter++;

        free(pl->key);
        pl->key = key;

        pl->value->ref_counter--;
        free_ast(pl->value);

        pl->value = value;
        return 1;
    }
    return find_func(pl->next, key, value);
}

int hash_map_func_insert(struct hash_map_func *hash_map, char *key,
                         struct ast_node *value, int *updated)
{
    if (hash_map == NULL || hash_map->size == 0)
        return 0;
    size_t i = hash(key) % hash_map->size;
    *updated = 0;
    if (hash_map->data[i] == NULL)
    {
        struct pair_list_func *newpl = malloc(sizeof(struct pair_list_func));
        if (newpl == NULL)
            return 0;
        newpl->key = key;
        newpl->value = value;
        value->ref_counter++;
        newpl->next = NULL;
        hash_map->data[i] = newpl;
    }
    else
    {
        *updated = find_func(hash_map->data[i], key, value);
        if (*updated == 1)
            return 1;
        struct pair_list_func *newpl = malloc(sizeof(struct pair_list_func));
        if (newpl == NULL)
            return 0;
        newpl->key = key;
        newpl->value = value;
        value->ref_counter++;
        newpl->next = hash_map->data[i];
        hash_map->data[i] = newpl;
    }
    return 1;
}

static struct ast_node *get_list(struct pair_list_func *pl, char *key)
{
    if (pl == NULL)
        return NULL;
    if (strcmp(key, pl->key) == 0)
        return pl->value;
    return get_list(pl->next, key);
}

struct ast_node *hash_map_func_get(const struct hash_map_func *hash_map,
                                   char *key)
{
    if (hash_map == NULL || hash_map->size == 0)
        return NULL;
    size_t i = hash(key) % hash_map->size;
    return get_list(hash_map->data[i], key);
}

static int rm_list_func(struct pair_list_func *pl, char *key,
                        struct pair_list_func *ppl)
{
    if (pl == NULL)
        return 0;
    if (strcmp(key, pl->key) == 0)
    {
        ppl->next = pl->next;
        free(pl->key);

        pl->value->ref_counter--;
        free_ast(pl->value);

        free(pl);
        return 1;
    }
    return rm_list_func(pl->next, key, pl);
}

int hash_map_func_remove(struct hash_map_func *hash_map, char *key)
{
    if (hash_map == NULL || hash_map->size == 0)
        return 0;
    size_t i = hash(key) % hash_map->size;
    if (hash_map->data[i] == NULL)
        return 0;
    if (strcmp(hash_map->data[i]->key, key) == 0)
    {
        struct pair_list_func *pl = hash_map->data[i];
        hash_map->data[i] = pl->next;
        free(pl->key);

        pl->value->ref_counter--;
        free_ast(pl->value);

        free(pl);
        return 1;
    }
    return rm_list_func(hash_map->data[i]->next, key, hash_map->data[i]);
}
