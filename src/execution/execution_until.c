#include <stdio.h>
#include <stdlib.h>

#include "ast/ast.h"
#include "ast/ast_until.h"
#include "environment/loops.h"
#include "execution.h"

struct exit_status execution_until(struct ast_node *ast)
{
    struct ast_until *ast_until = (struct ast_until *)ast;
    loop_append_level();
    struct exit_status status = execution_ast(ast_until->condition);
    while (status.exit_code != 0 && status.exit_action == EXIT_OK)
    {
        status = execution_ast(ast_until->todo);
        if (status.exit_action != EXIT_OK)
            break;
        struct loop_info *info = get_loop_info();
        if (info->action == LOOP_BREAK)
        {
            info->loop_remaining--;
            if (info->loop_remaining == 0)
                loop_reset_action();
            break;
        }
        else if (info->action == LOOP_CONTINUE)
        {
            info->loop_remaining--;
            if (info->loop_remaining == 0)
                loop_reset_action();
        }
        status = execution_ast(ast_until->condition);
    }

    // Until loop went well
    if (status.exit_action == EXIT_OK)
        status.exit_code = 0;

    loop_reduce_level();
    return status;
}
