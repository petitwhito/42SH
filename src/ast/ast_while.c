#include "ast_while.h"

#include <stdlib.h>

#include "ast.h"
#include "execution/execution.h"

struct ast_node *init_ast_while(void)
{
    struct ast_while *res = malloc(sizeof(struct ast_while));
    if (!res)
        return NULL;

    static struct ast_ftable ftable = {
        .run = &execution_while,
        .free = &free_ast_while,
    };

    res->base.type = AST_WHILE;
    res->base.ftable = &ftable;

    res->condition = NULL;
    res->todo = NULL;

    struct ast_node *base_res = &(res->base);
    return base_res;
}

void free_ast_while(struct ast_node *ast)
{
    struct ast_while *ast_while = (struct ast_while *)ast;

    reduce_ref_count(ast_while->condition);
    free_ast(ast_while->condition);

    reduce_ref_count(ast_while->todo);
    free_ast(ast_while->todo);

    free(ast_while);
}
