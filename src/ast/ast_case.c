#include "ast_case.h"

#include <stdlib.h>

#include "ast.h"
#include "execution/execution.h"

static void free_pair_case(struct pair_case *pair)
{
    if (pair)
    {
        free_pair_case(pair->next);
        int i = 0;
        while (pair->condition[i])
        {
            free(pair->condition[i]);
            i++;
        }
        free(pair->condition);
        reduce_ref_count(pair->todo);
        free_ast(pair->todo);
        free(pair);
    }
}

struct ast_node *init_ast_case(void)
{
    struct ast_case *res = malloc(sizeof(struct ast_case));
    if (!res)
        return NULL;

    static struct ast_ftable ftable = {
        .run = &execution_case,
        .free = &free_ast_case,
    };

    res->base.type = AST_CASE;
    res->base.ftable = &ftable;

    res->var = NULL;
    res->pair_case = NULL;

    struct ast_node *base_res = &(res->base);
    return base_res;
}

void free_ast_case(struct ast_node *ast)
{
    struct ast_case *ast_case = (struct ast_case *)ast;
    free(ast_case->var);
    free_pair_case(ast_case->pair_case);
    free(ast_case);
}
