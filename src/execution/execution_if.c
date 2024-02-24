#include <stdlib.h>

#include "ast/ast_if.h"
#include "execution.h"

struct exit_status execution_if(struct ast_node *ast)
{
    struct ast_if *ast_if = (struct ast_if *)ast;
    struct exit_status exit_status;

    struct exit_status if_cond = execution_ast(ast_if->condition);
    if (!if_cond.exit_code && if_cond.exit_action == EXIT_OK)
        exit_status = execution_ast(ast_if->then);
    else if (if_cond.exit_action == EXIT_OK)
        exit_status = execution_ast(ast_if->else_body);
    else
        exit_status = if_cond;

    return exit_status;
}
