#ifndef AST_IF_H
#define AST_IF_H

#include "ast.h"

struct ast_if
{
    struct ast_node base;
    struct ast_node *condition;
    struct ast_node *then;
    struct ast_node *else_body;
};

struct ast_node *init_ast_if(void);
void free_ast_if(struct ast_node *ast);

#endif /* ! AST_IF_H */
