#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>

#include "ast/ast_and_or.h"
#include "parser.h"

enum parser_status parse_and_or(struct lexer *lexer, struct ast_node **res,
                                struct io_backend *io)
{
    struct ast_node *node = init_ast(AST_AND_OR);
    *res = node;
    struct ast_and_or *ast = (struct ast_and_or *)node;

    if (parse_pipeline(lexer, &(ast->pipeline), io) == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;

    struct token tok = lexer_peek(lexer, io);
    if (tok.id != TOKEN_ID_AND && tok.id != TOKEN_ID_OR)
        return PARSER_OK;

    struct ast_and_or *tmp = NULL;
    while (tok.id == TOKEN_ID_AND || tok.id == TOKEN_ID_OR)
    {
        tmp = (struct ast_and_or *)init_ast(AST_AND_OR);
        tmp->next = (struct ast_node *)ast;
        *res = (struct ast_node *)tmp;
        ast = tmp;
        ast->ope = (tok.id == TOKEN_ID_AND) ? ID_AND : ID_OR;
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);

        while (tok.id == TOKEN_ID_LF)
        {
            lexer_pop(lexer);
            tok = lexer_peek(lexer, io);
        }

        if (parse_pipeline(lexer, &(ast->pipeline), io) == PARSER_UNEXPECTED)
            return PARSER_UNEXPECTED;
        tok = lexer_peek(lexer, io);
    }

    return PARSER_OK;
}
