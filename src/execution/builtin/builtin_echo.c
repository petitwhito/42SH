#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"

static void replace_sub(char *str)
{
    int i = 1;
    int len = strlen(str);
    while (str[i] != '\0')
    {
        if (str[i - 1] == '\\' && str[i] == 'n')
        {
            memmove(str + i - 1, str + i, len - i + 1);
            str[i - 1] = '\n';
        }

        if (str[i - 1] == '\\' && str[i] == 't')
        {
            memmove(str + i - 1, str + i, len - i + 1);
            str[i - 1] = '\t';
        }
        if (str[i - 1] == '\\' && str[i] == '\\')
        {
            memmove(str + i - 1, str + i, len - i + 1);
            str[i - 1] = '\\';
        }
        i++;
    }
}

static int get_options(char *str, bool *newline, bool *interpret)
{
    bool n_tmp = *newline;
    bool i_tmp = *interpret;
    for (int i = 1; str[i] != '\0'; i++)
    {
        if (str[i] == 'n')
            n_tmp = false;
        else if (str[i] == 'e')
            i_tmp = true;
        else if (str[i] == 'E')
            i_tmp = false;
        else
            return 0;
    }
    *newline = n_tmp;
    *interpret = i_tmp;
    return 1;
}

struct exit_status builtin_echo(char **argv)
{
    if (!argv)
        return (struct exit_status){ 0, EXIT_OK };
    bool newline = true;
    bool interpret = false;

    int i = 1;
    while (argv[i] != NULL && argv[i][0] == '-'
           && get_options(argv[i], &newline, &interpret))
        i++;

    while (argv[i])
    {
        if (interpret)
            replace_sub(argv[i]);
        printf("%s", argv[i]);
        if (argv[i + 1] != NULL)
            printf(" ");
        i++;
    }

    if (newline)
        printf("\n");
    fflush(stdout);
    return (struct exit_status){ 0, EXIT_OK };
}
