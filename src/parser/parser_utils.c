#define _POSIX_C_SOURCE 200809L

#include <stddef.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser.h"

int is_token_in_list(struct token tok, enum token_id *first_list, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (first_list[i] == tok.id)
            return 1;
    }
    return 0;
}
