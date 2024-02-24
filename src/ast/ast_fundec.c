#include "ast_fundec.h"

#include <stdlib.h>

#include "ast.h"
#include "execution/execution.h"

struct ast_node *init_ast_fundec(void)
{
    struct ast_fundec *res = malloc(sizeof(struct ast_fundec));
    if (!res)
        return NULL;

    static struct ast_ftable ftable = {
        .run = &execution_fundec,
        .free = &free_ast_fundec,
    };

    res->base.type = AST_FUNDEC;
    res->base.ftable = &ftable;

    res->cmd = NULL;
    res->name = NULL;
    res->redir = NULL;

    struct ast_node *base_res = &(res->base);
    return base_res;
}

void free_ast_fundec(struct ast_node *ast)
{
    struct ast_fundec *ast_fundec = (struct ast_fundec *)ast;

    reduce_ref_count(ast_fundec->cmd);
    free_ast(ast_fundec->cmd);

    free(ast_fundec->name);

    reduce_ref_count(ast_fundec->redir);
    free_ast(ast_fundec->redir);

    free(ast_fundec);
}
