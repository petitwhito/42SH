#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast/ast.h"
#include "ast/ast_if.h"
#include "ast/ast_list.h"
#include "ast/ast_pipe.h"
#include "environment/environment.h"
#include "error_handler/error_handler.h"
#include "execution.h"

struct exit_status execution_pipe(struct ast_node *ast)
{
    struct ast_pipe *ast_pipe = (struct ast_pipe *)ast;
    struct exit_status res = { 0, EXIT_OK };

    // If pipe doesn't have a next, we don't need to fork anymore
    // we can simply execute ast_pipe->child
    if (ast_pipe->next == NULL)
    {
        res = execution_ast(ast_pipe->child);
        set_last_val(res.exit_code);
        return res;
    }

    // Prepare pipe to connect current ast_pipe and the next one
    int pipefds[2];
    if (pipe(pipefds) == -1)
        return (struct exit_status){ 127, EXIT_ERROR };

    pid_t pid = fork();
    if (pid == -1)
        return (struct exit_status){ 127, EXIT_ERROR };

    // Child executes ast_pipe->child
    if (pid == 0)
    {
        close(pipefds[0]);
        dup2(pipefds[1], STDOUT_FILENO);
        close(pipefds[1]);
        res = execution_ast(ast_pipe->child);
        set_last_val(res.exit_code);
        res.exit_action = EXIT_NORMALLY;
        return res;
    }

    // Parent waits for child to finish to execute ast_pipe->next
    close(pipefds[1]);
    int status;
    waitpid(pid, &status, 0);
    set_last_val(WEXITSTATUS(status));

    // pipefds[0] replaces definitively stdin, we can then close the duplicate
    // in pipefds[0]
    dup2(pipefds[0], STDIN_FILENO);
    close(pipefds[0]);

    return execution_ast(ast_pipe->next);
}
