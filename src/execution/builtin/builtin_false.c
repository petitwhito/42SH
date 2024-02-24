#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"

// /!\ 0 = true, other = false
struct exit_status builtin_false(char **argv)
{
    if (!argv)
        return (struct exit_status){ 1, EXIT_OK };
    return (struct exit_status){ 1, EXIT_OK };
}
