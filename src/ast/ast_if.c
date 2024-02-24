#include "ast_if.h"

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "execution/execution.h"

struct ast_node *init_ast_if(void)
{
    struct ast_if *res = malloc(sizeof(struct ast_if));
    if (!res)
        return NULL;

    static struct ast_ftable ftable = {
        .run = &execution_if,
        .free = &free_ast_if,
    };

    res->base.type = AST_IF;
    res->base.ftable = &ftable;

    res->condition = NULL;
    res->then = NULL;
    res->else_body = NULL;

    struct ast_node *base_res = &(res->base);
    return base_res;
}

void free_ast_if(struct ast_node *ast)
{
    struct ast_if *ast_if = (struct ast_if *)ast;

    reduce_ref_count(ast_if->condition);
    free_ast(ast_if->condition);

    reduce_ref_count(ast_if->then);
    free_ast(ast_if->then);

    reduce_ref_count(ast_if->else_body);
    free_ast(ast_if->else_body);

    free(ast_if);
}
