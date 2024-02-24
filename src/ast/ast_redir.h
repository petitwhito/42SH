#ifndef AST_REDIR_H
#define AST_REDIR_H

#include "ast.h"
#include "redirection/redirection.h"

struct ast_redir
{
    struct ast_node base;
    redir_run run_redir; // Defines which redirection type to perform
    int fd; // == -1 if no fd is provided (set default)
    char *file; // The file path to redirect to
    struct ast_node *next;
};

struct ast_node *init_ast_redir(void);

void free_ast_redir(struct ast_node *ast);

#endif /* ! AST_REDIR_H */
