#include "environment.h"

#include "utils/hash_map.h"

static struct hash_map *variables_hash_map;
static struct hash_map *alias_hash_map;
static int last_val = 0;
static int random_state = 1;

void init_hash_map(void)
{
    variables_hash_map = hash_map_init(HASH_SIZE);
    alias_hash_map = hash_map_init(HASH_SIZE);
}

struct hash_map *get_hash_map(void)
{
    return variables_hash_map;
}

struct hash_map *get_alias_hash_map(void)
{
    return alias_hash_map;
}

void free_hash_map(void)
{
    hash_map_free(variables_hash_map);
    hash_map_free(alias_hash_map);
}

int get_last_val(void)
{
    return last_val;
}

void set_last_val(int val)
{
    last_val = val;
}

int get_random_state(void)
{
    return random_state;
}

void set_random_state(void)
{
    random_state = 0;
}
