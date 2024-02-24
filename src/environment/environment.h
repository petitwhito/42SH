#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#define HASH_SIZE 16

#include "ast/ast.h"

/**
 * \brief Initialize the global variable hash map
 *        This hash map contains local variables
 */
void init_hash_map(void);

/**
 * \brief Get a pointer to the global variable hash map
 */
struct hash_map *get_hash_map(void);

struct hash_map *get_alias_hash_map(void);

/**
 * \brief Free memory allocated by the variable hash map
 */
void free_hash_map(void);

int get_last_val(void);

void set_last_val(int val);

int get_random_state(void);

void set_random_state(void);

#endif /* ! ENVIRONMENT_H */
