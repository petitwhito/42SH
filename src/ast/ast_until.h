#ifndef AST_UNTIL_H
#define AST_UNTIL_H

#include "ast.h"

struct ast_until
{
    struct ast_node base;
    struct ast_node *condition;
    struct ast_node *todo;
};

struct ast_node *init_ast_until(void);

void free_ast_until(struct ast_node *ast);

#endif /* ! AST_UNTIL_H */
