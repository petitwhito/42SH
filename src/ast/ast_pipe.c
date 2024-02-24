#include "ast_pipe.h"

#include <stdlib.h>

#include "ast.h"
#include "execution/execution.h"

struct ast_node *init_ast_pipe(void)
{
    struct ast_pipe *res = malloc(sizeof(struct ast_pipe));
    if (!res)
        return NULL;

    static struct ast_ftable ftable = {
        .run = &execution_pipe,
        .free = &free_ast_pipe,
    };

    res->base.type = AST_PIPE;
    res->base.ftable = &ftable;

    res->next = NULL;
    res->child = NULL;

    struct ast_node *base_res = &(res->base);
    return base_res;
}

void free_ast_pipe(struct ast_node *ast)
{
    struct ast_pipe *ast_pipe = (struct ast_pipe *)ast;

    reduce_ref_count(ast_pipe->next);
    free_ast(ast_pipe->next);

    reduce_ref_count(ast_pipe->child);
    free_ast(ast_pipe->child);

    free(ast_pipe);
}
