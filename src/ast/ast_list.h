#ifndef AST_LIST_H
#define AST_LIST_H

#include <stdbool.h>

#include "ast.h"

struct ast_list
{
    struct ast_node base;
    struct ast_node *next;
    struct ast_node *child;
    bool is_sub;
};

struct ast_node *init_ast_list(void);

void free_ast_list(struct ast_node *ast);

#endif /* ! AST_LIST_H */
