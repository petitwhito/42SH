#ifndef EXECUTION_H
#define EXECUTION_H

#include "ast/ast.h"

enum exit_action
{
    EXIT_OK,
    EXIT_NORMALLY, // we must exit but without an error (ex: fork finished)
    EXIT_ERROR, // a critical error happened and shell must exit
};

struct exit_status
{
    int exit_code;
    enum exit_action exit_action;
};

void init_hash_map(void);

struct hash_map *get_hash_map(void);

void free_hash_map(void);

struct exit_status execution_ast(struct ast_node *ast);

struct exit_status execution_list(struct ast_node *ast);

struct exit_status execution_if(struct ast_node *ast);

struct exit_status execution_cmd(struct ast_node *ast);

struct exit_status execution_while(struct ast_node *ast);

struct exit_status execution_until(struct ast_node *ast);

struct exit_status execution_redir(struct ast_node *ast);

struct exit_status execution_simple_cmd(struct ast_node *ast);

struct exit_status execution_and_or(struct ast_node *ast);

struct exit_status execution_negation(struct ast_node *ast);

struct exit_status execution_pipe(struct ast_node *ast);

struct exit_status execution_for(struct ast_node *ast);

struct exit_status execution_fundec(struct ast_node *ast);

struct exit_status execute_function(struct ast_node *ast);

struct exit_status execution_case(struct ast_node *ast);

#endif /* ! EXECUTION_H */
