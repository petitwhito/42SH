#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>

#include "ast/ast.h"
#include "ast/ast_list.h"
#include "parser.h"

enum parser_status parse_shell_command(struct lexer *lexer,
                                       struct ast_node **res,
                                       struct io_backend *io)
{
    struct token tok = lexer_peek(lexer, io);
    if (tok.id == TOKEN_ID_IF)
        return parse_rule_if(lexer, res, io);
    if (tok.id == TOKEN_ID_WHILE)
        return parse_rule_while(lexer, res, io);
    if (tok.id == TOKEN_ID_UNTIL)
        return parse_rule_until(lexer, res, io);
    if (tok.id == TOKEN_ID_FOR)
        return parse_rule_for(lexer, res, io);
    if (tok.id == TOKEN_ID_CASE)
        return parse_rule_case(lexer, res, io);
    if (tok.id == TOKEN_ID_LBRACE)
    {
        lexer_pop(lexer);
        if (parse_compound_list(lexer, res, io) != PARSER_OK)
            return PARSER_UNEXPECTED;
        tok = lexer_peek(lexer, io);
        if (tok.id != TOKEN_ID_RBRACE)
            return PARSER_UNEXPECTED;
        lexer_pop(lexer);
        return PARSER_OK;
    }
    if (tok.id == TOKEN_ID_LPAR)
    {
        lexer_pop(lexer);
        if (parse_compound_list(lexer, res, io) != PARSER_OK)
            return PARSER_UNEXPECTED;
        tok = lexer_peek(lexer, io);
        if (tok.id != TOKEN_ID_RPAR)
            return PARSER_UNEXPECTED;
        lexer_pop(lexer);
        struct ast_list *ast = (struct ast_list *)*res;
        ast->is_sub = true;
        *res = (struct ast_node *)ast;
        return PARSER_OK;
    }
    return PARSER_UNEXPECTED;
}
