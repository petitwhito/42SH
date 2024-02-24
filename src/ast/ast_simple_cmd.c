#include "ast_simple_cmd.h"

#include <stdlib.h>

#include "ast.h"
#include "environment/var.h"
#include "execution/execution.h"

struct ast_node *init_ast_simple_cmd(void)
{
    struct ast_simple_cmd *res = malloc(sizeof(struct ast_simple_cmd));
    if (!res)
        return NULL;

    static struct ast_ftable ftable = {
        .run = &execution_simple_cmd,
        .free = &free_ast_simple_cmd,
    };

    res->base.type = AST_SIMPLE_CMD;
    res->base.ftable = &ftable;

    res->words = malloc(sizeof(char **) * 1);
    res->words[0] = NULL;
    res->redir = NULL;
    res->var = NULL;

    struct ast_node *base_res = &(res->base);
    return base_res;
}

void free_ast_simple_cmd(struct ast_node *ast)
{
    struct ast_simple_cmd *ast_simple_cmd = (struct ast_simple_cmd *)ast;
    for (char **c = ast_simple_cmd->words; *c != NULL; c++)
        free(*c);
    free(ast_simple_cmd->words);
    free_var(ast_simple_cmd->var);

    reduce_ref_count(ast_simple_cmd->redir);
    free_ast(ast_simple_cmd->redir);

    free(ast);
}
