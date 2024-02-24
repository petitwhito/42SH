#ifndef REDIRECTION_H
#define REDIRECTION_H

#define MAX_FD 10

struct ast_redir;

typedef int (*redir_run)(struct ast_redir *, int *);

void append_redir(struct ast_redir **first, struct ast_redir *new_redir);

void restore_fd(int *fd_backup);

void close_all_fd(int *fd_backup);

int redir_execute_all(struct ast_redir *ast_redir, int *fd_backup);

int redir_less(struct ast_redir *ast_redir, int *fd_backup);

int redir_great(struct ast_redir *ast_redir, int *fd_backup);

int redir_dgreat(struct ast_redir *ast_redir, int *fd_backup);

int redir_lessand(struct ast_redir *ast_redir, int *fd_backup);

int redir_greatand(struct ast_redir *ast_redir, int *fd_backup);

int redir_lessgreat(struct ast_redir *ast_redir, int *fd_backup);

#endif /* ! REDIRECTION_H */
