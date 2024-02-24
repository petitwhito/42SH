#include <stdlib.h>

#include "builtin.h"
#include "environment/loops.h"
#include "utils/argv_utils.h"

struct exit_status builtin_continue(char **argv)
{
    int n = 0;
    // If n is unspecified, n = 1
    if (!argv[1])
        n = 1;
    else
        n = atoi(argv[1]);

    loop_set_action(LOOP_CONTINUE, n);
    return (struct exit_status){ 0, EXIT_OK };
}
