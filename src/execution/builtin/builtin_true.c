#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"

// /!\ 0 = true, other = false
struct exit_status builtin_true(char **argv)
{
    if (!argv)
        return (struct exit_status){ 0, EXIT_OK };
    return (struct exit_status){ 0, EXIT_OK };
}
