#define _POSIX_C_SOURCE 200809L

#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "ast/ast.h"
#include "ast/ast_simple_cmd.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser.h"
#include "redirection/redirection.h"
#include "utils/argv_utils.h"
#include "utils/string_utils.h"

static int check_first(struct token tok)
{
    enum token_id first[] = {
        TOKEN_ID_WORD,     TOKEN_ID_LESS,      TOKEN_ID_GREAT,
        TOKEN_ID_CLOBBER,  TOKEN_ID_DGREAT,    TOKEN_ID_LESSAND,
        TOKEN_ID_GREATAND, TOKEN_ID_LESSGREAT, TOKEN_ID_IO_NUMBER,
    };

    // sizeof(first) / sizeof(first[0]) is the length of the array
    return is_token_in_list(tok, first, sizeof(first) / sizeof(first[0]));
}

static char *replace_tilde(char *w)
{
    if (w[0] != '~')
        return w;

    if (w[0] == '~' && w[1] != '\0')
    {
        w[0] = '/';
        return w;
    }

    else
    {
        char *homedir;
        if ((homedir = getenv("HOME")) == NULL)
            homedir = getpwuid(getuid())->pw_dir;
        return homedir;
    }
}

enum parser_status parse_element(struct lexer *lexer, struct ast_node **res,
                                 struct io_backend *io)
{
    struct token tok = lexer_peek(lexer, io);
    if (tok.type != TOKEN_WORD && !check_first(tok))
        return PARSER_UNEXPECTED;

    // It can only be a ast_cmd at this point, element is only called from
    // simple_command rule
    struct ast_simple_cmd *simple_cmd = (struct ast_simple_cmd *)*res;
    struct ast_node **redir = &(simple_cmd->redir);

    // FIRST of redirection is either a IO_NUMBER or a redir token
    if (tok.id >= TOKEN_ID_LESS && tok.id <= TOKEN_ID_IO_NUMBER)
        return parse_redirection(lexer, redir, io);

    // WORD
    char *w = strdup(replace_tilde(tok.value));
    argv_append(&(simple_cmd->words), w);

    lexer_pop(lexer);

    return PARSER_OK;
}
