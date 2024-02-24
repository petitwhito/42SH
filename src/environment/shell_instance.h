#ifndef SHELL_INSTANCE_H
#define SHELL_INSTANCE_H

#include "io_backend/io.h"

struct shell_instance
{
    struct io_backend *io;
    struct lexer *lexer;
    struct ast_node *ast;
};

struct shell_instance *new_shell_instance(char *input, char *path);

struct shell_instance *new_shell_main(int argc, char **argv);

void free_shell_instance(struct shell_instance *instance);

struct exit_status execute_shell_instance(struct shell_instance *instance);

#endif /* ! SHELL_INSTANCE_H */
