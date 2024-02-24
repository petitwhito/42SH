#define _POSIX_C_SOURCE 200809L

#include "special_var.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "environment/arguments.h"
#include "environment/shell_instance.h"
#include "io_backend/io.h"
#include "utils/argv_utils.h"

static char *join_argv(int argc, char **argv)
{
    char *res = malloc(1);
    int i = 0;
    for (int k = 0; k < argc; k++)
    {
        for (int j = 0; argv[k][j] != '\0'; j++)
        {
            res = realloc(res, i + 2);
            res[i] = argv[k][j];
            i++;
        }

        if (k < argc - 1)
        {
            res = realloc(res, i + 2);
            res[i] = ' ';
            i++;
        }
    }
    res[i] = '\0';

    return res;
}

char *old_pwd_sv(void)
{
    char *tmp = getenv("OLDPWD");
    if (!tmp)
        return NULL;
    char *res = strdup(tmp);
    return res;
}

char *pwd_sv(void)
{
    char *tmp = getenv("PWD");
    if (!tmp)
        return NULL;
    char *res = strdup(tmp);
    return res;
}

char *argv_sv(int i)
{
    char **argv = get_current_args();
    int argc = get_argc();

    if (i == 0 || i >= argc)
        return NULL;

    char *res = strdup(argv[i - 1]);
    return res;
}

char *argv_star_sv(void)
{
    char **argv = get_current_args();
    int argc = argv_len(argv);
    return join_argv(argc, argv);
}

char *ifs_sv(void)
{
    char *tmp = getenv("IFS");
    if (!tmp)
        return NULL;
    char *res = strdup(tmp);
    return res;
}

char **argv_at_sv(void)
{
    char **argv = get_current_args();
    return argv_copy(argv);
}
