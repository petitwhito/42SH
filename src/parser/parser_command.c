#define _POSIX_C_SOURCE 200809L

#include "ast/ast.h"
#include "ast/ast_cmd.h"
#include "ast/ast_redir.h"
#include "parser.h"

/*
static int check_first(struct token tok)
{
    enum token_id first[] = { TOKEN_ID_IF,       TOKEN_ID_WHILE,
                              TOKEN_ID_UNTIL,    TOKEN_ID_ASSIGNMENT_WORD,
                              TOKEN_ID_WORD,     TOKEN_ID_LESS,
                              TOKEN_ID_GREAT,    TOKEN_ID_CLOBBER,
                              TOKEN_ID_DGREAT,   TOKEN_ID_LESSAND,
                              TOKEN_ID_GREATAND, TOKEN_ID_LESSGREAT,
                              TOKEN_ID_IO_NUMBER };

    // sizeof(first) / sizeof(first[0]) is the length of the array
    return is_token_in_list(tok, first, sizeof(first) / sizeof(first[0]));
}
*/

// simple command
// shell command { redirection }
enum parser_status parse_command(struct lexer *lexer, struct ast_node **res,
                                 struct io_backend *io)
{
    struct ast_node *ast = init_ast(AST_CMD);
    *res = ast;
    struct ast_cmd *ast_cmd = (struct ast_cmd *)ast;

    struct token tok = lexer_peek(lexer, io);
    // if (!check_first(tok))
    //     return PARSER_UNEXPECTED;

    // for the function
    if (tok.id == TOKEN_ID_WORD)
    {
        struct token tok2 = lexer_peek2(lexer, io);
        if (tok2.id == TOKEN_ID_LPAR)
            return parse_fundec(lexer, &(ast_cmd->child), io);
    }

    // no 'if'/'while'/'until' means simple_command
    if (tok.id != TOKEN_ID_IF && tok.id != TOKEN_ID_WHILE
        && tok.id != TOKEN_ID_UNTIL && tok.id != TOKEN_ID_FOR
        && tok.id != TOKEN_ID_LBRACE && tok.id != TOKEN_ID_LPAR
        && tok.id != TOKEN_ID_CASE)
        return parse_simple_command(lexer, &(ast_cmd->child), io);

    // else it is a shell_command
    if (parse_shell_command(lexer, &(ast_cmd->child), io) == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;

    tok = lexer_peek(lexer, io);
    // shell command * { redirection }
    struct ast_node **redir = &(ast_cmd->redir);
    while (tok.id >= TOKEN_ID_LESS && tok.id <= TOKEN_ID_IO_NUMBER)
    {
        if (parse_redirection(lexer, redir, io) == PARSER_UNEXPECTED)
            return PARSER_UNEXPECTED;
        tok = lexer_peek(lexer, io);
    }

    return PARSER_OK;
}
