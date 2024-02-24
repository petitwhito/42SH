#ifndef COMMAND_SUBSTITUTION_H
#define COMMAND_SUBSTITUTION_H

#include "expansion.h"

struct sub_res
{
    enum expansion_status status;
    char *buffer;
};

struct sub_res execute_substitution(char *command);

#endif /* ! COMMAND_SUBSTITUTION_H */
