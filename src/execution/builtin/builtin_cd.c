#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtin.h"
#include "error_handler/error_handler.h"

int count(char **argv)
{
    int i = 0;
    while (argv[i])
        i++;
    return i;
}

void delete_until_slash(char *input_str)
{
    int index = strlen(input_str) - 1;

    while (index >= 0 && input_str[index] != '/')
        index--;

    input_str[index] = '\0';
}

struct exit_status error(char *oldirectory)
{
    free(oldirectory);
    fprintf(stderr, "cd: folder does not exist\n");
    return (struct exit_status){ 1, EXIT_OK };
}

struct exit_status minus(char *newdirectory, char *oldirectory)
{
    int good = 1;
    if (getenv("OLDPWD"))
        newdirectory = strdup(getenv("OLDPWD"));
    else
    {
        free(oldirectory);
        oldirectory = NULL;
        fprintf(stderr, "cd: OLDPWD not set\n");
        good = 0;
        newdirectory = getcwd(NULL, 0);
    }

    // Do not print cd - if oldpwd is not set
    if (good)
    {
        puts(newdirectory);
        if (chdir(newdirectory) != 0)
            return error(oldirectory);
        setenv("PWD", newdirectory, 1);
    }

    if (!oldirectory)
        setenv("OLDPWD", "", 1);
    else
        setenv("OLDPWD", oldirectory, 1);

    free(newdirectory);
    free(oldirectory);
    return (struct exit_status){ 0, EXIT_OK };
}

struct exit_status error_args(void)
{
    fprintf(stderr, "cd: to many args\n");
    return (struct exit_status){ 1, EXIT_OK };
}

int set_env_oldpwd(char **oldirectory)
{
    if (*oldirectory)
        setenv("OLDPWD", *oldirectory, 1);
    else
    {
        setenv("OLDPWD", "", 1);
        *oldirectory = getcwd(NULL, 0);
        return 0;
    }
    return 1;
}

struct exit_status hardlink(char *oldirectory, char *newdirectory)
{
    if (chdir(newdirectory) != 0)
        return error(oldirectory);
    set_env_oldpwd(&oldirectory);
    setenv("PWD", newdirectory, 1);
    free(oldirectory);
    return (struct exit_status){ 0, EXIT_OK };
}

struct exit_status builtin_cd(char **argv)
{
    // check empty cd and to many args
    int i = count(argv);
    if (i == 1)
        return (struct exit_status){ 0, EXIT_OK };
    else if (i != 2)
        return error_args();

    // Initaite old and new dir
    char *newdirectory = argv[1];
    char *oldirectory = NULL;
    if (getenv("PWD"))
        oldirectory = strdup(getenv("PWD"));

    // case where / at the end
    int index = strlen(newdirectory) - 1;

    // Case of hardlink
    if (newdirectory[0] == '/')
        return hardlink(oldirectory, newdirectory);

    // Remove last / for chdir compliance
    if (newdirectory[index] == '/')
        newdirectory[index] = '\0';

    // case with .
    if (!strcmp(argv[1], "."))
    {
        int ch = set_env_oldpwd(&oldirectory);
        if (!ch)
            setenv("PWD", oldirectory, 1);
        free(oldirectory);
        return (struct exit_status){ 0, EXIT_OK };
    }

    // Case with -
    if (!strcmp(newdirectory, "-"))
        return minus(newdirectory, oldirectory);

    // Check symlink
    struct stat file_info;
    lstat(newdirectory, &file_info);

    // Go into newdirectory
    if (chdir(newdirectory) != 0)
        return error(oldirectory);

    int check = set_env_oldpwd(&oldirectory);

    char old[10000];
    strcpy(old, oldirectory);

    // Here update newdirectory
    // If .. then remove the last /.../ from oldirectory
    // If not .. then add newdirectory to oldirectory
    if (!strcmp(newdirectory, "..") && check)
        delete_until_slash(old);
    else if (check)
    {
        strcat(old, "/");
        strcat(old, newdirectory);
    }

    // symlink case
    if (S_ISLNK(file_info.st_mode))
        setenv("PWD", argv[1], 1);
    else
        setenv("PWD", old, 1);

    free(oldirectory);
    return (struct exit_status){ 0, EXIT_OK };
}
