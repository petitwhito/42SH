#ifndef BUILTIN_H
#define BUILTIN_H

#include "execution.h"

typedef struct exit_status (*built_run)(char **);

struct builtin_cmd
{
    char *name;
    built_run run;
};

built_run get_builtin(char *command);

struct exit_status builtin_true(char **argv);

struct exit_status builtin_false(char **argv);

struct exit_status builtin_echo(char **argv);

struct exit_status builtin_exit(char **argv);

struct exit_status builtin_cd(char **argv);

struct exit_status builtin_unset(char **argv);

struct exit_status builtin_break(char **argv);

struct exit_status builtin_export(char **argv);

struct exit_status builtin_continue(char **argv);

struct exit_status builtin_dot(char **argv);

struct exit_status builtin_alias(char **argv);

struct exit_status builtin_unalias(char **argv);

#endif /* ! BUILTIN_H */
