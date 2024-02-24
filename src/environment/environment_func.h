#ifndef ENVIRONMENT_FUNC_H
#define ENVIRONMENT_FUNC_H

#define HASH_SIZE 16

#include "ast/ast.h"

/**
 * \brief Initialize the global function hash map
 */
void init_hash_map_func(void);

/**
 * \brief Get a pointer to the global function hash map
 * \return a pointer to the function hash map
 */
struct hash_map_func *get_hash_map_func(void);

/**
 * \brief Free memory allocated by the function hash map
 */
void free_hash_map_func(void);

#endif /* ! ENVIRONMENT_FUNC_H */
