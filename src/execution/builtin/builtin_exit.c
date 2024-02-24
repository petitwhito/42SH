#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"
#include "environment/environment.h"
#include "error_handler/error_handler.h"

int count_args(char **argv)
{
    int i = 0;
    while (argv[i])
        i++;

    return i;
}

int check_num(char *string)
{
    for (int i = 0; string[i]; i++)
        if (!isdigit(string[i]))
            return 0;
    return 1;
}

struct exit_status builtin_exit(char **argv)
{
    struct exit_status status = { 0, EXIT_NORMALLY };
    int i = count_args(argv);
    if (i == 1)
        exit_noprint(get_last_val());
    else if (i != 2)
    {
        fprintf(stderr, "exit: too many args");
        return (struct exit_status){ 1, EXIT_NORMALLY };
    }
    else if (!check_num(argv[1]))
    {
        fprintf(stderr, "exit: too many args");
        return (struct exit_status){ 2, EXIT_NORMALLY };
    }

    int exit_code = atoi(argv[1]);
    status = (struct exit_status){ exit_code, EXIT_NORMALLY };
    return status;
}
