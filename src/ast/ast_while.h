#ifndef AST_WHILE_H
#define AST_WHILE_H

#include "ast.h"

struct ast_while
{
    struct ast_node base;
    struct ast_node *condition;
    struct ast_node *todo;
};

struct ast_node *init_ast_while(void);

void free_ast_while(struct ast_node *ast);

#endif /* ! AST_WHILE_H */
