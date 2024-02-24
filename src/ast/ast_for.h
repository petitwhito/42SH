#ifndef AST_FOR_H
#define AST_FOR_H

#include "ast.h"

struct ast_for
{
    struct ast_node base;
    char *var;
    char **words;
    struct ast_node *todo;
};

struct ast_node *init_ast_for(void);

void free_ast_for(struct ast_node *ast);

#endif /* ! AST_FOR_H */
