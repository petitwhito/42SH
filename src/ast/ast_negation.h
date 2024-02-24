#ifndef AST_NEGATION_H
#define AST_NEGATION_H

#include "ast.h"

struct ast_negation
{
    struct ast_node base;
    struct ast_node *child;
};

struct ast_node *init_ast_negation(void);

void free_ast_negation(struct ast_node *ast);

#endif /* ! AST_NEGATION_H */
