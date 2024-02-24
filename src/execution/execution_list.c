#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast/ast.h"
#include "ast/ast_if.h"
#include "ast/ast_list.h"
#include "environment/environment.h"
#include "environment/loops.h"
#include "execution.h"

struct exit_status execution_list_sub(struct ast_node *ast)
{
    struct exit_status status = { 0, EXIT_NORMALLY };
    struct ast_node *current = ast;

    pid_t p = fork();

    if (p == -1) // Error forking
    {
        fprintf(stderr, "error forking\n");
        status = (struct exit_status){ 127, EXIT_ERROR };
    }

    if (p == 0)
    {
        while (current)
        {
            struct loop_info *info = get_loop_info();
            // Check for break or continue
            if (info->action != LOOP_NOTHING)
                break;

            struct ast_list *ast_list = (struct ast_list *)current;
            status = execution_ast(ast_list->child);

            // Stop the list execution if there is an error
            if (status.exit_action != EXIT_OK)
                break;

            current = ast_list->next;
        }
        status.exit_action = EXIT_NORMALLY;
    }
    else
    {
        int s;
        waitpid(p, &s, 0);
        set_last_val(WEXITSTATUS(s));
        status.exit_code = s;
        status.exit_action = EXIT_OK;
    }

    return status;
}

struct exit_status execution_list(struct ast_node *ast)
{
    struct exit_status status = { 0, EXIT_OK };
    struct ast_node *current = ast;

    struct ast_list *temp = (struct ast_list *)current;
    if (temp->is_sub)
        return execution_list_sub(ast);

    while (current)
    {
        struct loop_info *info = get_loop_info();
        // Check for break or continue
        if (info->action != LOOP_NOTHING)
            break;

        struct ast_list *ast_list = (struct ast_list *)current;
        status = execution_ast(ast_list->child);

        // Stop the list execution if there is an error
        if (status.exit_action != EXIT_OK)
            break;

        current = ast_list->next;
    }
    return status;
}
