#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "ast/ast_redir.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser.h"

// Function table for redir_run functions
static redir_run redir_table[] = {
    [TOKEN_ID_LESS - TOKEN_ID_LESS] = redir_less,
    [TOKEN_ID_GREAT - TOKEN_ID_LESS] = redir_great,
    [TOKEN_ID_CLOBBER - TOKEN_ID_LESS] = redir_great,
    [TOKEN_ID_DGREAT - TOKEN_ID_LESS] = redir_dgreat,
    [TOKEN_ID_LESSAND - TOKEN_ID_LESS] = redir_lessand,
    [TOKEN_ID_GREATAND - TOKEN_ID_LESS] = redir_greatand,
    [TOKEN_ID_LESSGREAT - TOKEN_ID_LESS] = redir_lessgreat,
};

// [IONUMBER] ( '>' | '<' | '>>' | '>&' | '>&' | '<&' | '>|' | '<>' )
//      WORD
enum parser_status parse_redirection(struct lexer *lexer, struct ast_node **res,
                                     struct io_backend *io)
{
    // * [IONUMBER] ( '>' | '<' | '>>' | '>&' | '>&' | '<&' | '>|' | '<>' )
    //   WORD
    int fd = -1;
    struct token tok = lexer_peek(lexer, io);
    if (tok.type == TOKEN_IO_NUMBER)
    {
        fd = atoi(tok.value);
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);
    }

    // [IONUMBER] * ( '>' | '<' | '>>' | '>&' | '>&' | '<&' | '>|' | '<>' )
    // WORD
    //      check if it is a redir operator
    if (tok.id < TOKEN_ID_LESS || tok.id > TOKEN_ID_LESSGREAT)
        return PARSER_UNEXPECTED;

    redir_run run_redir = redir_table[tok.id - TOKEN_ID_LESS];

    lexer_pop(lexer);
    tok = lexer_peek(lexer, io);

    // [IONUMBER] ( '>' | '<' | '>>' | '>&' | '>&' | '<&' | '>|' | '<>' )
    //   *  WORD
    if (tok.type != TOKEN_WORD)
        return PARSER_UNEXPECTED;

    struct ast_node *ast_redir = (struct ast_node *)init_ast(AST_REDIR);
    struct ast_redir *tmp = (struct ast_redir *)ast_redir;
    append_redir((struct ast_redir **)res, tmp);
    tmp->run_redir = run_redir;
    tmp->fd = fd;
    tmp->file = strdup(tok.value);

    lexer_pop(lexer);

    return PARSER_OK;
}
