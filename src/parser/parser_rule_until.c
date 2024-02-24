#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "ast/ast_until.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser.h"
#include "utils/string_utils.h"

enum parser_status parse_rule_until(struct lexer *lexer, struct ast_node **res,
                                    struct io_backend *io)
{
    // * 'until' compound_list 'do' compound_list 'done' ;
    struct token tok = lexer_peek(lexer, io);
    // 'while' means shell_command
    if (tok.id != TOKEN_ID_UNTIL)
        return PARSER_UNEXPECTED;

    struct ast_node *node = init_ast(AST_UNTIL);
    *res = node;
    struct ast_until *ast_until = (struct ast_until *)node;

    lexer_pop(lexer);
    // 'until' * compound_list 'do' compound_list 'done' ;
    if (parse_compound_list(lexer, &(ast_until->condition), io)
        == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;

    tok = lexer_peek(lexer, io);
    // 'until' compound_list * 'do' compound_list 'done' ;
    if (tok.id != TOKEN_ID_DO)
        return PARSER_UNEXPECTED;

    lexer_pop(lexer);

    // 'until' compound_list 'do' * compound_list 'done' ;
    if (parse_compound_list(lexer, &(ast_until->todo), io) == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;

    tok = lexer_peek(lexer, io);
    // 'until' compound_list 'do' compound_list * 'done' ;
    if (tok.id != TOKEN_ID_DONE)
        return PARSER_UNEXPECTED;

    lexer_pop(lexer);

    return PARSER_OK;
}
