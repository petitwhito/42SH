#define _POSIX_C_SOURCE 200809L

#include <string.h>

#include "ast/ast.h"
#include "ast/ast_fundec.h"
#include "ast/ast_redir.h"
#include "parser.h"

enum parser_status parse_fundec(struct lexer *lexer, struct ast_node **res,
                                struct io_backend *io)
{
    struct ast_node *ast = init_ast(AST_FUNDEC);
    *res = ast;
    struct ast_fundec *ast_fundec = (struct ast_fundec *)ast;

    // * WORD '(' ')' {'\n'} shell_command
    struct token tok = lexer_peek(lexer, io);
    if (tok.id != TOKEN_ID_WORD)
        return PARSER_UNEXPECTED;
    ast_fundec->name = strdup(tok.value);
    lexer_pop(lexer);

    // WORD * '(' ')' {'\n'} shell_command
    tok = lexer_peek(lexer, io);
    if (tok.id != TOKEN_ID_LPAR)
        return PARSER_UNEXPECTED;

    lexer_pop(lexer);

    // WORD '(' * ')' {'\n'} shell_command
    tok = lexer_peek(lexer, io);
    if (tok.id != TOKEN_ID_RPAR)
        return PARSER_UNEXPECTED;

    lexer_pop(lexer);

    // WORD '(' ')' * {'\n'} shell_command
    tok = lexer_peek(lexer, io);
    while (tok.id == TOKEN_ID_LF)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);
    }

    // WORD '(' ')' {'\n'} * shell_command
    if (parse_shell_command(lexer, &(ast_fundec->cmd), io) == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;

    // Redirections need to be processed from here and not parse_command
    // WORD '(' ')' {'\n'} shell_command *
    // fundec * { redirection }
    tok = lexer_peek(lexer, io);
    struct ast_node **redir = &(ast_fundec->redir);
    while (tok.id >= TOKEN_ID_LESS && tok.id <= TOKEN_ID_IO_NUMBER)
    {
        if (parse_redirection(lexer, redir, io) == PARSER_UNEXPECTED)
            return PARSER_UNEXPECTED;
        tok = lexer_peek(lexer, io);
    }
    return PARSER_OK;
}
