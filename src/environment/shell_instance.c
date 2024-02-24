#include "shell_instance.h"

#include <stdlib.h>
#include <string.h>

#include "arguments.h"
#include "ast/ast.h"
#include "error_handler/error_handler.h"
#include "execution/execution.h"
#include "io_backend/io.h"
#include "parser/parser.h"
#include "utils/argv_utils.h"

struct shell_instance *new_shell_instance(char *input, char *path)
{
    if (input && path)
        exit_print(
            127, "new_shell_instance: both input and path given at same time");

    struct shell_instance *res = malloc(sizeof(struct shell_instance));
    if (!res)
        exit_print(127, "new_shell_instance: error allocating shell instance");

    res->lexer = new_lexer();

    // No input and no path mean stdin
    if (!input && !path)
        res->io = new_io_stdin();
    else if (input)
        res->io = new_io_string(input);
    else
        res->io = new_io_file(path);

    res->ast = NULL;
    return res;
}

struct shell_instance *new_shell_main(int argc, char **argv)
{
    // Shift argv to remove 42sh name from it and keep only positional arguments
    int offset = 1;

    // Stdin case
    // Check if input is in stdin
    if (argc == 1)
    {
        init_main_argv(argv, offset);
        return new_shell_instance(NULL, NULL);
    }

    // String argument case
    // Check if we have option -c
    if (strcmp("-c", argv[1]) == 0)
    {
        if (argc == 2)
            exit_print(2, "io_backend : -c needs argument");
        offset += 2;
        init_main_argv(argv, offset);
        return new_shell_instance(argv[2], NULL);
    }

    offset++;
    // File case
    // Before opening a "file", we must check if bad option
    if (argv[1][0] == '-')
        exit_print(2, "io_backend : invalid option");
    init_main_argv(argv, offset);
    return new_shell_instance(NULL, argv[1]);
}

void free_shell_instance(struct shell_instance *instance)
{
    free_ast(instance->ast);
    free_backend(instance->io);
    free_lexer(instance->lexer);
    free(instance);
}

struct exit_status execute_shell_instance(struct shell_instance *instance)
{
    struct exit_status exit_status = { 0, EXIT_OK };

    enum parser_status st =
        parse_input(instance->lexer, &(instance->ast), instance->io);
    while (st == PARSER_OK)
    {
        exit_status = execution_ast(instance->ast);
        if (exit_status.exit_action != EXIT_OK)
            break;
        free_ast(instance->ast);
        instance->ast = NULL;
        st = parse_input(instance->lexer, &(instance->ast), instance->io);
    }
    if (st == PARSER_ERROR)
        return (struct exit_status){ 2, EXIT_ERROR };

    return exit_status;
}
