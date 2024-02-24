#include "ast_cmd.h"

#include <stdlib.h>

#include "ast.h"
#include "ast_cmd.h"
#include "execution/execution.h"

struct ast_node *init_ast_cmd(void)
{
    struct ast_cmd *res = malloc(sizeof(struct ast_cmd));
    if (!res)
        return NULL;

    static struct ast_ftable ftable = {
        .run = &execution_cmd,
        .free = &free_ast_cmd,
    };

    res->base.type = AST_CMD;
    res->base.ftable = &ftable;

    res->child = NULL;
    res->redir = NULL;

    struct ast_node *base_res = &(res->base);
    return base_res;
}

void free_ast_cmd(struct ast_node *ast)
{
    struct ast_cmd *ast_cmd = (struct ast_cmd *)ast;

    reduce_ref_count(ast_cmd->redir);
    free_ast((struct ast_node *)ast_cmd->redir);

    reduce_ref_count(ast_cmd->child);
    free_ast(ast_cmd->child);

    free(ast_cmd);
}
