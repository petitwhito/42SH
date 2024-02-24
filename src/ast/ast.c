#include "ast.h"

#include <stdlib.h>

#include "ast_and_or.h"
#include "ast_case.h"
#include "ast_cmd.h"
#include "ast_for.h"
#include "ast_fundec.h"
#include "ast_if.h"
#include "ast_list.h"
#include "ast_negation.h"
#include "ast_pipe.h"
#include "ast_redir.h"
#include "ast_simple_cmd.h"
#include "ast_until.h"
#include "ast_while.h"

static init_func init_table[] = {
    init_ast_cmd,      init_ast_simple_cmd, init_ast_if,    init_ast_list,
    init_ast_while,    init_ast_until,      init_ast_redir, init_ast_and_or,
    init_ast_negation, init_ast_pipe,       init_ast_for,   init_ast_fundec,
    init_ast_case,
};

void reduce_ref_count(struct ast_node *ast)
{
    if (!ast)
        return;
    ast->ref_counter--;
}

struct ast_node *init_ast(enum ast_type type)
{
    if (type > AST_CASE || type < AST_CMD)
        return NULL;
    struct ast_node *ast = init_table[type]();
    ast->ref_counter = 1;
    return ast;
}

void free_ast(struct ast_node *ast)
{
    if (!ast || ast->ftable->free == NULL)
        return;
    if (ast->ref_counter == 0)
        (ast->ftable->free)(ast);
}
