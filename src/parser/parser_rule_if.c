#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "ast/ast_if.h"
#include "ast/ast_list.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser.h"
#include "utils/string_utils.h"

enum parser_status parse_rule_if(struct lexer *lexer, struct ast_node **res,
                                 struct io_backend *io)
{
    // * 'if' compound_list 'then' compound_list [else_clause] 'fi' ;
    struct token tok = lexer_peek(lexer, io);
    // 'if' means shell_command
    if (tok.id != TOKEN_ID_IF)
        return PARSER_UNEXPECTED;

    struct ast_node *node = init_ast(AST_IF);
    *res = node;
    struct ast_if *ast_if = (struct ast_if *)node;

    lexer_pop(lexer);
    // 'if' * compound_list 'then' compound_list [else_clause] 'fi' ;
    if (parse_compound_list(lexer, &(ast_if->condition), io)
        == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;

    tok = lexer_peek(lexer, io);
    // 'if' compound_list * 'then' compound_list [else_clause] 'fi' ;
    if (tok.id != TOKEN_ID_THEN)
        return PARSER_UNEXPECTED;

    lexer_pop(lexer);

    // 'if' compound_list 'then' * compound_list [else_clause] 'fi' ;
    if (parse_compound_list(lexer, &(ast_if->then), io) == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;

    tok = lexer_peek(lexer, io);
    // 'if' compound_list 'then' compound_list * [else_clause] 'fi' ;
    if (tok.id == TOKEN_ID_ELSE || tok.id == TOKEN_ID_ELIF)
    {
        if (parse_else_clause(lexer, &(ast_if->else_body), io)
            == PARSER_UNEXPECTED)
            return PARSER_UNEXPECTED;
    }

    tok = lexer_peek(lexer, io);

    // 'if' compound_list 'then' compound_list [else_clause] * 'fi' ;
    if (tok.id != TOKEN_ID_FI)
        return PARSER_UNEXPECTED;

    lexer_pop(lexer);

    return PARSER_OK;
}
