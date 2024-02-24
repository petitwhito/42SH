#include <stdlib.h>

#include "utils/argv_utils.h"

// this is the main 42sh argv
// NEVER TRY TO FREE THIS
static char **main_argv = NULL;

// offset is the amount to shift on the right the main argv until the positional
// arguments
static int argv_offset = 0;

// describes the current positional arguments $@
// must be updated each time a dot or a function is executed
static char **current_args = NULL;

void init_main_argv(char **argv, int offset)
{
    argv_offset = offset;
    main_argv = argv + offset;
    current_args = main_argv;
}

char **get_current_args(void)
{
    return current_args;
}

void set_current_args(char **new_args)
{
    current_args = new_args;
}

int get_argc(void)
{
    return argv_len(current_args);
}
