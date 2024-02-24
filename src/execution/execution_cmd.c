#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast/ast.h"
#include "ast/ast_cmd.h"
#include "builtin/builtin.h"
#include "execution.h"
#include "redirection/redirection.h"

struct exit_status execution_cmd(struct ast_node *ast)
{
    struct ast_cmd *ast_cmd = (struct ast_cmd *)ast;

    // Redirections
    // We make a backup array to store the original file descriptors value
    // before redirections (only affects the scope of this node)
    int fd_backup[MAX_FD];

    memset(fd_backup, -1, sizeof(fd_backup));

    redir_execute_all((struct ast_redir *)ast_cmd->redir, fd_backup);

    // Execute child node
    struct exit_status exit_status = execution_ast(ast_cmd->child);

    restore_fd(fd_backup);
    return exit_status;
}
