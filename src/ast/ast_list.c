#include "ast_list.h"

#include <stdbool.h>
#include <stdlib.h>

#include "ast.h"
#include "execution/execution.h"

struct ast_node *init_ast_list(void)
{
    struct ast_list *res = malloc(sizeof(struct ast_list));
    if (!res)
        return NULL;

    static struct ast_ftable ftable = {
        .run = &execution_list,
        .free = &free_ast_list,
    };

    res->base.type = AST_LIST;
    res->base.ftable = &ftable;

    res->next = NULL;
    res->child = NULL;
    res->is_sub = false;

    struct ast_node *base_res = &(res->base);
    return base_res;
}

void free_ast_list(struct ast_node *ast)
{
    struct ast_list *ast_list = (struct ast_list *)ast;

    reduce_ref_count(ast_list->next);
    free_ast(ast_list->next);

    reduce_ref_count(ast_list->child);
    free_ast(ast_list->child);

    free(ast_list);
}
