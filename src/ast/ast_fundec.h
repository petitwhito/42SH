#ifndef AST_FUNDEC_H
#define AST_FUNDEC_H

#include "ast.h"

struct ast_fundec
{
    struct ast_node base;
    struct ast_node *cmd;
    char *name;
    struct ast_node *redir;
};

struct ast_node *init_ast_fundec(void);

void free_ast_fundec(struct ast_node *ast);

#endif /* ! AST_FUNDEC_H */
