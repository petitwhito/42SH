#ifndef AST_SIMPLE_CMD_H
#define AST_SIMPLE_CMD_H

#include "ast.h"
#include "utils/string_utils.h"

struct ast_simple_cmd
{
    struct ast_node base;
    char **words;
    struct ast_node *redir;
    struct var *var;
};

struct ast_node *init_ast_simple_cmd(void);

void free_ast_simple_cmd(struct ast_node *ast);

#endif /* ! AST_SIMPLE_CMD_H */
