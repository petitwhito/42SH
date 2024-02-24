#include "ast_until.h"

#include <stdlib.h>

#include "ast.h"
#include "ast_until.h"
#include "execution/execution.h"

struct ast_node *init_ast_until(void)
{
    struct ast_until *res = malloc(sizeof(struct ast_until));
    if (!res)
        return NULL;

    static struct ast_ftable ftable = {
        .run = &execution_until,
        .free = &free_ast_until,
    };

    res->base.type = AST_UNTIL;
    res->base.ftable = &ftable;

    res->condition = NULL;
    res->todo = NULL;

    struct ast_node *base_res = &(res->base);
    return base_res;
}

void free_ast_until(struct ast_node *ast)
{
    struct ast_until *ast_until = (struct ast_until *)ast;

    reduce_ref_count(ast_until->condition);
    free_ast(ast_until->condition);

    reduce_ref_count(ast_until->todo);
    free_ast(ast_until->todo);

    free(ast_until);
}
