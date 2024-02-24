#include "hash_map.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t hash(char *key)
{
    size_t result = 0;
    size_t i = 0;
    while (key[i])
        result += key[i++];

    return result;
}

struct hash_map *hash_map_init(size_t size)
{
    struct hash_map *h = malloc(sizeof(struct hash_map));
    if (h == NULL)
        return NULL;
    h->size = size;
    h->data = calloc(size, sizeof(struct pair_list));
    return h;
}

static int find(struct pair_list *pl, char *key, char *value)
{
    if (pl == NULL)
        return 0;
    if (strcmp(pl->key, key) == 0)
    {
        free(pl->key);
        free(pl->value);
        pl->key = key;
        pl->value = value;
        return 1;
    }
    return find(pl->next, key, value);
}

int hash_map_insert(struct hash_map *hash_map, char *key, char *value,
                    int *updated)
{
    if (hash_map == NULL || hash_map->size == 0)
        return 0;
    size_t i = hash(key) % hash_map->size;
    *updated = 0;
    if (hash_map->data[i] == NULL)
    {
        struct pair_list *newpl = malloc(sizeof(struct pair_list));
        if (newpl == NULL)
            return 0;
        newpl->key = key;
        newpl->value = value;
        newpl->next = NULL;
        hash_map->data[i] = newpl;
    }
    else
    {
        *updated = find(hash_map->data[i], key, value);
        if (*updated == 1)
            return 1;
        struct pair_list *newpl = malloc(sizeof(struct pair_list));
        if (newpl == NULL)
            return 0;
        newpl->key = key;
        newpl->value = value;
        newpl->next = hash_map->data[i];
        hash_map->data[i] = newpl;
    }
    return 1;
}

static void free_list(struct pair_list *pl)
{
    if (pl == NULL)
        return;
    struct pair_list *npl = pl->next;
    while (pl != NULL)
    {
        free(pl->key);
        free(pl->value);
        free(pl);
        pl = npl;
        if (pl == NULL)
            return;
        npl = pl->next;
    }
}

void hash_map_free(struct hash_map *hash_map)
{
    if (hash_map == NULL)
        return;
    for (size_t i = 0; i < hash_map->size; i++)
    {
        free_list(hash_map->data[i]);
    }
    free(hash_map->data);
    free(hash_map);
}

static char *get_list(struct pair_list *pl, char *key)
{
    if (pl == NULL)
        return NULL;
    if (strcmp(key, pl->key) == 0)
        return pl->value;
    return get_list(pl->next, key);
}

char *hash_map_get(const struct hash_map *hash_map, char *key)
{
    if (hash_map == NULL || hash_map->size == 0)
        return NULL;
    size_t i = hash(key) % hash_map->size;
    return get_list(hash_map->data[i], key);
}

void empty_list(struct hash_map *hash_map, struct pair_list *pl)
{
    if (pl == NULL)
        return;
    struct pair_list *npl = pl->next;
    while (pl != NULL)
    {
        hash_map_remove(hash_map, pl->key);
        pl = npl;
        if (pl == NULL)
            return;
        npl = pl->next;
    }
}

static int rm_list(struct pair_list *pl, char *key, struct pair_list *ppl)
{
    if (pl == NULL)
        return 0;
    if (strcmp(key, pl->key) == 0)
    {
        ppl->next = pl->next;
        free(pl->key);
        free(pl->value);
        free(pl);
        return 1;
    }
    return rm_list(pl->next, key, pl);
}

int hash_map_remove(struct hash_map *hash_map, char *key)
{
    if (hash_map == NULL || hash_map->size == 0)
        return 0;
    size_t i = hash(key) % hash_map->size;
    if (hash_map->data[i] == NULL)
        return 0;
    if (strcmp(hash_map->data[i]->key, key) == 0)
    {
        struct pair_list *pl = hash_map->data[i];
        hash_map->data[i] = pl->next;
        free(pl->key);
        free(pl->value);
        free(pl);
        return 1;
    }
    return rm_list(hash_map->data[i]->next, key, hash_map->data[i]);
}
