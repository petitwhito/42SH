#include <stdlib.h>

#include "ast/ast_and_or.h"
#include "execution.h"

struct exit_status execution_and_or(struct ast_node *ast)
{
    struct ast_and_or *ast_and_or = (struct ast_and_or *)ast;
    struct exit_status res = { 0, EXIT_OK };

    if (ast_and_or->ope == ID_LEAF)
        return execution_ast(ast_and_or->pipeline);

    if (ast_and_or->ope == ID_OR)
    {
        struct exit_status left = execution_ast(ast_and_or->next);
        if (left.exit_code == 0 || left.exit_action != EXIT_OK)
            return left;
        struct exit_status right = execution_ast(ast_and_or->pipeline);

        res.exit_code = !(!left.exit_code || !right.exit_code);
        res.exit_action = right.exit_action;
        return res;
        // return !(!execution_ast(ast_and_or->next)
        //          || !execution_ast(ast_and_or->pipeline));
    }

    struct exit_status left = execution_ast(ast_and_or->next);
    if (left.exit_code != 0 || left.exit_action != EXIT_OK)
        return left;
    struct exit_status right = execution_ast(ast_and_or->pipeline);

    res.exit_code = !(!left.exit_code && !right.exit_code);
    res.exit_action = right.exit_action;
    return res;

    // return !(!execution_ast(ast_and_or->next)
    //         && !execution_ast(ast_and_or->pipeline));
}
