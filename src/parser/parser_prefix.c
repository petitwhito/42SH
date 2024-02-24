#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "ast/ast_simple_cmd.h"
#include "environment/var.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser.h"

enum parser_status parse_prefix(struct lexer *lexer, struct ast_node **res,
                                struct io_backend *io)
{
    struct token tok = lexer_peek(lexer, io);
    if (tok.id == TOKEN_ID_ASSIGNMENT_WORD)
    {
        struct ast_simple_cmd *cmd = (struct ast_simple_cmd *)*res;

        // ASSIGNMENT WORD
        struct token tok = lexer_peek(lexer, io);
        cmd->var = append_var(cmd->var, tok.value);

        lexer_pop(lexer);
        return PARSER_OK;
    }

    struct ast_simple_cmd **ast_simple = (struct ast_simple_cmd **)res;
    struct ast_node **redir = &((*ast_simple)->redir);
    return parse_redirection(lexer, redir, io);
}
