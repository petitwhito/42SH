#ifndef AST_CMD_H
#define AST_CMD_H

#include "ast.h"

struct ast_cmd
{
    struct ast_node base;
    struct ast_node *child;
    struct ast_node *redir;
};

struct ast_node *init_ast_cmd(void);

void free_ast_cmd(struct ast_node *ast);

#endif /* ! AST_CMD_H */
