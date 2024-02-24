#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast/ast_redir.h"
#include "redirection.h"
#include "utils/string_utils.h"

// <
int redir_less(struct ast_redir *ast_redir, int *fd_backup)
{
    if (!ast_redir || !fd_backup)
        return -1;

    int old_fd = ast_redir->fd;

    // If old_fd == -1, set to default fd = 0 (stdin)
    if (old_fd == -1)
        old_fd = 0;

    // We support only fd range 0 to 9
    if (old_fd > 9 || old_fd < 0)
        return -1;

    int new_fd = open(ast_redir->file, O_RDONLY);

    // Error opening the file, (must not exit)
    if (new_fd == -1)
        return -1;

    // If both fd are the same, nothing to do
    if (new_fd == old_fd)
        return 0;

    // Backup the original fd for later restore
    // This backup only happens when we redirect the old_fd for the first time
    if (fd_backup[old_fd] == -1)
    {
        fd_backup[old_fd] = dup(old_fd);
        // No need to close the backup manually when we exec with FD_CLOEXEC
        fcntl(fd_backup[old_fd], F_SETFD, FD_CLOEXEC);
    }

    // Error redirecting
    if (dup2(new_fd, old_fd) == -1)
    {
        close(new_fd);
        return -1;
    }

    // Original file fd is of no use now that it replaced the old one
    close(new_fd);

    return 0;
}

// >
int redir_great(struct ast_redir *ast_redir, int *fd_backup)
{
    if (!ast_redir || !fd_backup)
        return -1;
    int old_fd = ast_redir->fd;

    // If old_fd == -1, set to default fd = 1 (stdout)
    if (old_fd == -1)
        old_fd = 1;

    // We support only fd range 0 to 9
    if (old_fd > 9 || old_fd < 0)
        return -1;

    int new_fd = open(ast_redir->file, O_CREAT | O_TRUNC | O_WRONLY, 0666);

    // Error opening the file, (must not exit)
    if (new_fd == -1)
        return -1;

    // If both fd are the same, nothing to do
    if (new_fd == old_fd)
        return 0;

    // Backup the original fd for later restore
    // This backup only happens when we redirect the old_fd for the first time
    if (fd_backup[old_fd] == -1)
    {
        fd_backup[old_fd] = dup(old_fd);
        // No need to close the backup manually when we exec with FD_CLOEXEC
        fcntl(fd_backup[old_fd], F_SETFD, FD_CLOEXEC);
    }

    // Error redirecting
    if (dup2(new_fd, old_fd) == -1)
    {
        close(new_fd);
        return -1;
    }

    // Original file fd is of no use now that it replaced the old one
    close(new_fd);

    return 0;
}

// >>
int redir_dgreat(struct ast_redir *ast_redir, int *fd_backup)
{
    if (!ast_redir || !fd_backup)
        return -1;
    int old_fd = ast_redir->fd;

    // If old_fd == -1, set to default fd = 1 (stdout)
    if (old_fd == -1)
        old_fd = 1;

    // We support only fd range 0 to 9
    if (old_fd > 9 || old_fd < 0)
        return -1;

    int new_fd = open(ast_redir->file, O_CREAT | O_APPEND | O_WRONLY, 0666);

    // Error opening the file, (must not exit)
    if (new_fd == -1)
        return -1;

    // If both fd are the same, nothing to do
    if (new_fd == old_fd)
        return 0;

    // Backup the original fd for later restore
    // This backup only happens when we redirect the old_fd for the first time
    if (fd_backup[old_fd] == -1)
    {
        fd_backup[old_fd] = dup(old_fd);
        // No need to close the backup manually when we exec with FD_CLOEXEC
        fcntl(fd_backup[old_fd], F_SETFD, FD_CLOEXEC);
    }

    // Error redirecting
    if (dup2(new_fd, old_fd) == -1)
    {
        close(new_fd);
        return -1;
    }

    // Original file fd is of no use now that it replaced the old one
    close(new_fd);

    return 0;
}

// <&
int redir_lessand(struct ast_redir *ast_redir, int *fd_backup)
{
    if (!ast_redir || !fd_backup)
        return -1;
    int old_fd = ast_redir->fd;

    // If old_fd == -1, we must redirect stdin (0)
    if (old_fd == -1)
        old_fd = 0;

    // We support only fd range 0 to 9
    if (old_fd > 9 || old_fd < 0)
        return -1;

    // [n]<&-
    // if word == '-' we should close the n fd
    if (!strcmp(ast_redir->file, "-"))
    {
        close(old_fd);
        return 0;
    }

    // Convert word to a number
    // Having not a number as a fd is unspecified behaviour in SCL
    int new_fd = atoi(ast_redir->file);

    // If both fd are the same, nothing to do
    if (new_fd == old_fd)
        return 0;

    // Backup the original fd for later restore
    // This backup only happens when we redirect the old_fd for the first time
    if (fd_backup[old_fd] == -1)
    {
        fd_backup[old_fd] = dup(old_fd);
        // No need to close the backup manually when we exec with FD_CLOEXEC
        fcntl(fd_backup[old_fd], F_SETFD, FD_CLOEXEC);
    }

    // Error redirecting
    if (dup2(new_fd, old_fd) == -1)
    {
        close(new_fd);
        return -1;
    }

    return 0;
}

// >&
int redir_greatand(struct ast_redir *ast_redir, int *fd_backup)
{
    if (!ast_redir || !fd_backup)
        return -1;
    int old_fd = ast_redir->fd;

    // If old_fd == -1, we must redirect stdout (1)
    if (old_fd == -1)
        old_fd = 1;

    // We support only fd range 0 to 9
    if (old_fd > 9 || old_fd < 0)
        return -1;

    // [n]>&-
    // if word == '-' we should close the n fd
    if (!strcmp(ast_redir->file, "-"))
    {
        close(old_fd);
        return 0;
    }

    // Convert word to a number
    // Having not a number as a fd is unspecified behaviour in SCL
    int new_fd = atoi(ast_redir->file);

    // If both fd are the same, nothing to do
    if (new_fd == old_fd)
        return 0;

    // Backup the original fd for later restore
    // This backup only happens when we redirect the old_fd for the first time
    if (fd_backup[old_fd] == -1)
    {
        fd_backup[old_fd] = dup(old_fd);
        // No need to close the backup manually when we exec with FD_CLOEXEC
        fcntl(fd_backup[old_fd], F_SETFD, FD_CLOEXEC);
    }

    // Error redirecting
    if (dup2(new_fd, old_fd) == -1)
    {
        close(new_fd);
        return -1;
    }

    return 0;
}

// <>
int redir_lessgreat(struct ast_redir *ast_redir, int *fd_backup)
{
    if (!ast_redir || !fd_backup)
        return -1;

    int old_fd = ast_redir->fd;

    // If old_fd == -1, set to default fd = 0 (stdin)
    if (old_fd == -1)
        old_fd = 0;

    // We support only fd range 0 to 9
    if (old_fd > 9 || old_fd < 0)
        return -1;

    int new_fd = open(ast_redir->file, O_RDWR | O_CREAT, 0666);

    // Error opening the file, (must not exit)
    if (new_fd == -1)
        return -1;

    // If both fd are the same, nothing to do
    if (new_fd == old_fd)
        return 0;

    // Backup the original fd for later restore
    // This backup only happens when we redirect the old_fd for the first time
    if (fd_backup[old_fd] == -1)
    {
        fd_backup[old_fd] = dup(old_fd);
        // No need to close the backup manually when we exec with FD_CLOEXEC
        fcntl(fd_backup[old_fd], F_SETFD, FD_CLOEXEC);
    }

    // Error redirecting
    if (dup2(new_fd, old_fd) == -1)
    {
        close(new_fd);
        return -1;
    }

    // Original file fd is of no use now that it replaced the old one
    close(new_fd);

    return 0;
}
