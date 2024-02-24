#include "error_handler.h"

#include <err.h>
#include <stdlib.h>

#include "ast/ast.h"
#include "environment/environment.h"
#include "environment/environment_func.h"
#include "environment/shell_instance.h"
#include "execution/execution.h"
#include "io_backend/io.h"
#include "lexer/lexer.h"

static int exit_code = -100;

void free_shell_data(void)
{
    free_hash_map();
    free_hash_map_func();
}

void exit_print(int exit_status, const char *err_msg)
{
    free_shell_data();
    if (exit_code != -100)
        exit_status = exit_code;
    errx(exit_status, "%s", err_msg);
}

void exit_noprint(int exit_status)
{
    free_shell_data();
    if (exit_code != -100)
        exit_status = exit_code;
    exit(exit_status);
}

void set_future_exit_code(int exit)
{
    exit_code = exit;
}
