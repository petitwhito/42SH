#include "ast_and_or.h"

#include <stdlib.h>

#include "ast.h"
#include "execution/execution.h"

struct ast_node *init_ast_and_or(void)
{
    struct ast_and_or *res = malloc(sizeof(struct ast_and_or));
    if (!res)
        return NULL;

    static struct ast_ftable ftable = {
        .run = &execution_and_or,
        .free = &free_ast_and_or,
    };

    res->base.type = AST_AND_OR;
    res->base.ftable = &ftable;

    res->next = NULL;
    res->pipeline = NULL;
    res->ope = ID_LEAF;

    struct ast_node *base_res = &(res->base);
    return base_res;
}

void free_ast_and_or(struct ast_node *ast)
{
    struct ast_and_or *ast_and_or = (struct ast_and_or *)ast;

    reduce_ref_count(ast_and_or->next);
    free_ast(ast_and_or->next);

    reduce_ref_count(ast_and_or->pipeline);
    free_ast(ast_and_or->pipeline);

    free(ast_and_or);
}
