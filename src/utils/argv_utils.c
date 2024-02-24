#define _POSIX_C_SOURCE 200809L

#include "utils/argv_utils.h"

#include <stdlib.h>
#include <string.h>

size_t argv_len(char **argv)
{
    size_t i = 0;
    while (argv[i])
        i++;
    return i;
}

char *argv_to_word(char **argv)
{
    if (!argv)
        return calloc(1, 1);

    // words must be null-terminated
    char *res = calloc(1, 1);
    size_t len = 0;
    for (size_t i = 0; argv[i]; i++)
    {
        size_t prev_len = len;
        len += strlen(argv[i]) + 1;
        res = realloc(res, len + 2);
        if (i != 0)
        {
            res[prev_len - 1] = ' ';
            res[prev_len] = '\0';
        }
        strcat(res, argv[i]);
    }
    return res;
}

char **argv_copy(char **argv)
{
    if (!argv)
        return NULL;
    size_t size = 1;
    while (argv[size - 1] != NULL)
        size++;

    char **tmp = calloc(size + 1, sizeof(char *));
    if (!tmp)
        return NULL;

    for (size_t i = 0; argv[i]; ++i)
        tmp[i] = strdup(argv[i]);
    return tmp;
}

int argv_append(char ***argv, char *str)
{
    char **argv_list = *argv;
    // argv is NULL terminated, size is 1 minimum
    size_t size = 1;
    while (argv_list[size - 1] != NULL)
        size++;
    char **tmp = realloc(*argv, sizeof(char *) * (size + 1));
    if (!tmp)
        return -1;
    *argv = tmp;
    tmp[size - 1] = str;
    tmp[size] = NULL;
    return 0;
}

int argv_remove(char ***argv, size_t pos)
{
    if (!argv || !*argv || !(*argv)[0])
        return -1;

    char **argv_list = *argv;
    size_t size = argv_len(argv_list);

    if (pos >= size)
        return -1;

    char *to_remove = argv_list[pos];

    // Shift to the left from pos
    memmove(argv_list + pos, argv_list + pos + 1,
            (size - pos) * sizeof(char *));

    char **tmp = realloc(argv_list, sizeof(char *) * size);
    if (!tmp)
        return -1;

    *argv = tmp;

    // Free the word to be removed
    free(to_remove);
    return 0;
}

void argv_replace(char **argv, char *str, size_t pos)
{
    free(argv[pos]);
    argv[pos] = strdup(str);
}

int argv_insert(char ***argv, char *str, size_t pos)
{
    char **argv_list = *argv;

    size_t size = argv_len(argv_list);

    if (pos > size)
        return -1;

    char **tmp = realloc(*argv, sizeof(char *) * (size + 2));

    if (!tmp)
        return -1;

    *argv = tmp;

    // Shift right the argv to make room for the new string inserted
    memmove(*argv + pos + 1, *argv + pos, (size - pos + 1) * sizeof(char *));

    tmp[pos] = strdup(str);

    return 0;
}

void merge_argv(char ***argv1, char **argv2, size_t pos)
{
    size_t len2 = argv_len(argv2);
    for (size_t i = 0; i < len2; i++)
        argv_insert(argv1, argv2[i], pos++);
}

void free_argv(char **argv)
{
    if (!argv)
        return;

    for (char **c = argv; *c != NULL; c++)
        free(*c);
    free(argv);
}
