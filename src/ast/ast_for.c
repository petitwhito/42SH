#include "ast_for.h"

#include <stdlib.h>

#include "ast.h"
#include "execution/execution.h"

struct ast_node *init_ast_for(void)
{
    struct ast_for *res = malloc(sizeof(struct ast_for));
    if (!res)
        return NULL;
    static struct ast_ftable ftable = {
        .run = &execution_for,
        .free = &free_ast_for,
    };

    res->base.type = AST_FOR;
    res->base.ftable = &ftable;

    res->var = NULL;
    res->words = malloc(1 * sizeof(char *));
    res->words[0] = NULL;
    res->todo = NULL;

    struct ast_node *base_res = &(res->base);
    return base_res;
}

void free_ast_for(struct ast_node *ast)
{
    struct ast_for *ast_for = (struct ast_for *)ast;
    free(ast_for->var);
    for (int i = 0; ast_for->words[i]; i++)
    {
        free(ast_for->words[i]);
    }
    free(ast_for->words);

    reduce_ref_count(ast_for->todo);
    free_ast(ast_for->todo);

    free(ast_for);
}
