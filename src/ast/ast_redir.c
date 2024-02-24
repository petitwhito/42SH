#include "ast_redir.h"

#include <stdlib.h>

#include "ast.h"
#include "execution/execution.h"

struct ast_node *init_ast_redir(void)
{
    struct ast_redir *res = malloc(sizeof(struct ast_redir));
    if (!res)
        return NULL;

    static struct ast_ftable ftable = {
        .run = NULL,
        .free = &free_ast_redir,
    };

    res->base.type = AST_REDIR;
    res->base.ftable = &ftable;

    res->run_redir = NULL;
    res->fd = -1;
    res->file = NULL;
    res->next = NULL;

    struct ast_node *base_res = &(res->base);
    return base_res;
}

void free_ast_redir(struct ast_node *ast)
{
    if (!ast)
        return;
    struct ast_redir *ast_redir = (struct ast_redir *)ast;
    free(ast_redir->file);

    reduce_ref_count(ast_redir->next);
    free_ast(ast_redir->next);

    free(ast_redir);
}
