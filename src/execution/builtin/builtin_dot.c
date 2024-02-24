#define _POSIX_C_SOURCE 200809L

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "builtin.h"
#include "environment/arguments.h"
#include "environment/shell_instance.h"
#include "utils/argv_utils.h"

static int contain_slash(char *filename)
{
    size_t i = 0;
    while (filename[i] != '\0')
    {
        if (filename[i] == '/')
            return 1;
        i++;
    }

    return 0;
}

static char *search_in_path(char *file)
{
    // Search in the PATH env variable for file
    char *path = getenv("PATH");
    if (!path)
        return NULL;

    char *path_copy = strdup(path);

    char *res = NULL;
    char *dir = strtok(path_copy, ":");

    int found = 0;
    while (dir && !found)
    {
        DIR *d = opendir(dir);
        struct dirent *dirent = readdir(d);
        while (dirent)
        {
            if (!strcmp(dirent->d_name, file))
            {
                size_t len = strlen(dirent->d_name) + strlen(dir) + 3;
                res = calloc(len, 1);
                sprintf(res, "%s/%s", dir, dirent->d_name);
                found = 1;
                break;
            }
            dirent = readdir(d);
        }
        closedir(d);
        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return res;
}

struct exit_status builtin_dot(char **argv)
{
    if (!argv)
        return (struct exit_status){ 0, EXIT_OK };

    // No argument provided case (bash --posix and dash hang...)
    if (argv_len(argv) < 1)
        return (struct exit_status){ 0, EXIT_OK };

    // char **args = argv + 2;
    char *file = argv[1];

    // Check if first char of file is '/'
    // PATH case
    int need_free = 0;
    if (!contain_slash(file))
    {
        file = search_in_path(file);
        if (!file)
        {
            fprintf(stderr, "file not found or cannot be opened\n");
            return (struct exit_status){ 1, EXIT_ERROR };
        }
        need_free = 1;
    }

    // Test if file can be opened
    FILE *f = fopen(file, "r");
    // Check if file was correctly opened
    if (!f)
    {
        fprintf(stderr, "file not found or cannot be opened\n");
        if (need_free)
            free(file);
        return (struct exit_status){ 1, EXIT_ERROR };
    }

    fclose(f);

    // Create a new shell instance that will be run on the file
    struct shell_instance *instance = new_shell_instance(NULL, file);

    // Backup the previous args
    char **prev_args = get_current_args();

    // SCL do not take arguments
    char **new_args = calloc(1, sizeof(char *));

    set_current_args(new_args);

    struct exit_status res = execute_shell_instance(instance);

    free_shell_instance(instance);

    set_current_args(prev_args);

    free_argv(new_args);

    if (need_free)
        free(file);

    return res;
}
