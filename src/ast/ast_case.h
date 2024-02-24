#ifndef AST_CASE_H
#define AST_CASE_H

#include "ast.h"

struct pair_case
{
    char **condition;
    struct ast_node *todo;
    struct pair_case *next;
};

struct ast_case
{
    struct ast_node base;
    char *var;
    struct pair_case *pair_case;
};

struct ast_node *init_ast_case(void);

void free_ast_case(struct ast_node *ast);

#endif /* ! AST_CASE_H */
