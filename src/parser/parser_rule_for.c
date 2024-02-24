#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "ast/ast_for.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser.h"
#include "utils/argv_utils.h"
#include "utils/string_utils.h"

// 'for' WORD ( [';'] | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ] ) {'\n'} 'do'
// compound_list 'done';

static enum parser_status
parse_words(struct lexer *lexer, struct ast_for *ast_for, struct io_backend *io)
{
    struct token tok = lexer_peek(lexer, io);
    if (tok.id == TOKEN_ID_SEMI)
    {
        lexer_pop(lexer);
        return PARSER_OK;
    }
    while (tok.id == TOKEN_ID_LF)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);
    }

    // get in
    if (tok.id != TOKEN_ID_IN)
        return PARSER_OK;

    lexer_pop(lexer);
    tok = lexer_peek(lexer, io);

    while (tok.type == TOKEN_WORD)
    {
        argv_append(&ast_for->words, strdup(tok.value));
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);
    }
    if (tok.id != TOKEN_ID_LF && tok.id != TOKEN_ID_SEMI)
        return PARSER_UNEXPECTED;
    lexer_pop(lexer);

    return PARSER_OK;
}

enum parser_status parse_rule_for(struct lexer *lexer, struct ast_node **res,
                                  struct io_backend *io)
{
    struct token tok = lexer_peek(lexer, io);

    if (tok.id != TOKEN_ID_FOR)
        return PARSER_UNEXPECTED;

    struct ast_node *node = init_ast(AST_FOR);
    *res = node;
    struct ast_for *ast_for = (struct ast_for *)node;

    lexer_pop(lexer);

    // get WORD and put it in var
    tok = lexer_peek(lexer, io);
    // check if it's a valid var
    if (tok.type != TOKEN_WORD)
        return PARSER_UNEXPECTED;
    ast_for->var = strdup(tok.value);
    lexer_pop(lexer);

    // call parse_word
    if (parse_words(lexer, ast_for, io) == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;

    tok = lexer_peek(lexer, io);
    while (tok.id == TOKEN_ID_LF)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);
    }

    // get do
    tok = lexer_peek(lexer, io);
    if (tok.id != TOKEN_ID_DO)
        return PARSER_UNEXPECTED;
    lexer_pop(lexer);

    // get compound_list
    if (parse_compound_list(lexer, &(ast_for->todo), io) == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;

    // get done
    tok = lexer_peek(lexer, io);
    if (tok.id != TOKEN_ID_DONE)
        return PARSER_UNEXPECTED;
    lexer_pop(lexer);

    return PARSER_OK;
}
