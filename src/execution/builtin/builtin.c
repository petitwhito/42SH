#include "builtin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Builtin table
static struct builtin_cmd builtin_table[] = { { "true", builtin_true },
                                              { "false", builtin_false },
                                              { "echo", builtin_echo },
                                              { "exit", builtin_exit },
                                              { "cd", builtin_cd },
                                              { "export", builtin_export },
                                              { "continue", builtin_continue },
                                              { "break", builtin_break },
                                              { ".", builtin_dot },
                                              { "unset", builtin_unset },
                                              { "alias", builtin_alias },
                                              { "unalias", builtin_unalias } };

built_run get_builtin(char *command)
{
    if (!command)
        return NULL;
    size_t len = sizeof(builtin_table) / sizeof(builtin_table[0]);
    for (size_t i = 0; i < len; i++)
    {
        if (!strcmp(command, builtin_table[i].name))
            return builtin_table[i].run;
    }
    return NULL;
}
