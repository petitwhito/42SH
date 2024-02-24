#include "ast_negation.h"

#include <stdlib.h>

#include "ast.h"
#include "execution/execution.h"

struct ast_node *init_ast_negation(void)
{
    struct ast_negation *res = malloc(sizeof(struct ast_negation));
    if (!res)
        return NULL;

    static struct ast_ftable ftable = {
        .run = &execution_negation,
        .free = &free_ast_negation,
    };

    res->base.type = AST_NEGATION;
    res->base.ftable = &ftable;

    res->child = NULL;

    struct ast_node *base_res = &(res->base);
    return base_res;
}

void free_ast_negation(struct ast_node *ast)
{
    struct ast_negation *ast_negation = (struct ast_negation *)ast;

    reduce_ref_count(ast_negation->child);
    free_ast(ast_negation->child);

    free(ast_negation);
}
