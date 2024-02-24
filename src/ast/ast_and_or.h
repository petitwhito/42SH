#ifndef AST_AND_OR_H
#define AST_AND_OR_H

#include "ast.h"

enum ast_ope
{
    ID_AND,
    ID_OR,
    ID_LEAF
};

struct ast_and_or
{
    struct ast_node base;
    enum ast_ope ope;
    struct ast_node *next;
    struct ast_node *pipeline;
};

struct ast_node *init_ast_and_or(void);

void free_ast_and_or(struct ast_node *ast);

#endif /* ! AST_AND_OR_H */
