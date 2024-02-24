#ifndef AST_PIPE_H
#define AST_PIPE_H

#include "ast.h"

struct ast_pipe
{
    struct ast_node base;
    struct ast_node *next;
    struct ast_node *child;
};

struct ast_node *init_ast_pipe(void);

void free_ast_pipe(struct ast_node *ast);

#endif /* ! AST_PIPE_H */
