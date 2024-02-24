#define _POSIX_C_SOURCE 200809L

#include "redirection.h"

#include <stdlib.h>
#include <unistd.h>

#include "ast/ast_redir.h"

void append_redir(struct ast_redir **first, struct ast_redir *new_redir)
{
    struct ast_redir *p = *first;
    if (!p)
    {
        *first = new_redir;
        return;
    }
    while (p->next)
        p = (struct ast_redir *)p->next;
    p->next = (struct ast_node *)new_redir;
}

// Execute a redirection according to the redir struct informations
static int redir_execute(struct ast_redir *ast_redir, int *fd_backup)
{
    return ast_redir->run_redir(ast_redir, fd_backup);
}

// Execute all redirs in the redir ast list in argument
// every redir must succeed for the command to proceed
// (stop if one redir fails and abort command)
int redir_execute_all(struct ast_redir *ast_redir, int *fd_backup)
{
    struct ast_redir *current = ast_redir;
    int status = 0;
    // We have to stop at first error in redir list
    while (current && !status)
    {
        status = redir_execute(current, fd_backup);
        current = (struct ast_redir *)current->next;
    }
    return status;
}

void restore_fd(int *fd_backup)
{
    for (int i = 0; i < MAX_FD; i++)
    {
        if (fd_backup[i] != -1)
        {
            dup2(fd_backup[i], i);
            close(fd_backup[i]);
        }
    }
}

void close_all_fd(int *fd_array)
{
    for (int i = 0; i < MAX_FD; i++)
    {
        if (fd_array[i] != -1)
        {
            close(fd_array[i]);
        }
    }
}
