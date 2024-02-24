#define _POSIX_C_SOURCE 200809L

#include "command_substitution.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "environment/environment.h"
#include "environment/shell_instance.h"
#include "error_handler/error_handler.h"
#include "execution/execution.h"

struct sub_res execute_substitution(char *command)
{
    // Empty command
    if (!command || strlen(command) == 0)
        return (struct sub_res){ EXPANSION_OK, strdup(command) };

    int pipefds[2];
    if (pipe(pipefds) == -1)
    {
        fprintf(stderr, "error creating pipe\n");
        return (struct sub_res){ EXPANSION_ERROR, NULL };
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "error creating pipe\n");
        return (struct sub_res){ EXPANSION_ERROR, NULL };
    }
    if (pid == 0)
    {
        close(pipefds[0]);
        dup2(pipefds[1], STDOUT_FILENO);
        close(pipefds[1]);

        struct shell_instance *shell = new_shell_instance(command, NULL);
        execute_shell_instance(shell);
        free_shell_instance(shell);
        return (struct sub_res){ EXPANSION_EXIT, NULL };
    }

    close(pipefds[1]);
    int status;
    waitpid(pid, &status, 0);
    set_last_val(WEXITSTATUS(status));

    char *buf = calloc(1, sizeof(char));
    size_t size = 0;
    ssize_t r = 1;
    while (r != 0)
    {
        char tmp = '\0';
        r = read(pipefds[0], &tmp, 1);
        if (r <= 0)
            break;

        buf[size] = tmp;
        size += r;
        buf = realloc(buf, size + 1);
        buf[size] = '\0';
    }

    buf[size] = '\0';
    close(pipefds[0]);
    return (struct sub_res){ EXPANSION_OK, buf };
}
