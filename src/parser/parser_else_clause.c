#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "ast/ast_if.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser.h"
#include "utils/string_utils.h"

//   'else' compount_list
// | 'elif' compount_list 'then' compound_list [else_clause]
enum parser_status parse_else_clause(struct lexer *lexer, struct ast_node **res,
                                     struct io_backend *io)
{
    struct token tok = lexer_peek(lexer, io);
    if (tok.id != TOKEN_ID_ELSE && tok.id != TOKEN_ID_ELIF)
        return PARSER_UNEXPECTED;

    // * 'else' compound_list
    if (tok.id == TOKEN_ID_ELSE)
    {
        lexer_pop(lexer);
        // 'else' * compound_list
        return parse_compound_list(lexer, res, io);
    }

    lexer_pop(lexer);

    // * 'elif' compount_list 'then' compound_list [else_clause]
    struct ast_node *node = init_ast(AST_IF);
    *res = node;
    struct ast_if *ast_if = (struct ast_if *)node;

    // 'elif' * compount_list 'then' compound_list [else_clause]
    if (parse_compound_list(lexer, &(ast_if->condition), io)
        == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;

    // 'elif' compount_list * 'then' compound_list [else_clause]
    tok = lexer_peek(lexer, io);
    if (tok.id != TOKEN_ID_THEN)
        return PARSER_UNEXPECTED;

    lexer_pop(lexer);

    // 'elif' compount_list 'then' * compound_list [else_clause]
    if (parse_compound_list(lexer, &(ast_if->then), io) == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;
    tok = lexer_peek(lexer, io);

    // 'elif' compount_list 'then' compound_list * [else_clause]
    if (tok.id == TOKEN_ID_ELSE || tok.id == TOKEN_ID_ELIF)
    {
        if (parse_else_clause(lexer, &(ast_if->else_body), io)
            == PARSER_UNEXPECTED)
            return PARSER_UNEXPECTED;
    }

    return PARSER_OK;
}
