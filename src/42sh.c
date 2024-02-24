#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "environment/environment.h"
#include "environment/environment_func.h"
#include "environment/shell_instance.h"
#include "error_handler/error_handler.h"
#include "execution/execution.h"
#include "io_backend/io.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "utils/hash_map.h"

static long long time_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

static void init_variables(void)
{
    struct hash_map *hash_map = get_hash_map();

    // Init RANDOM variable
    int b = 0;
    hash_map_insert(hash_map, strdup("RANDOM"), strdup("0"), &b);
    srand(time_ms());

    // Init IFS variable
    char *default_ifs = " \t\n";
    setenv("IFS", default_ifs, 1);
}

int main(int argc, char **argv)
{
    struct shell_instance *instance = new_shell_main(argc, argv);

    // Initialize hash_maps
    init_hash_map();
    init_hash_map_func();

    // Initialize special variables
    init_variables();

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

    // Free all shell data (shell instances, close redirections, free ast...)
    free_shell_instance(instance);
    free_shell_data();

    if (st == PARSER_ERROR)
    {
        fprintf(stderr, "parser error\n");
        exit_status = (struct exit_status){ 2, EXIT_ERROR };
    }

    return exit_status.exit_code;
}
