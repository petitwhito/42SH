#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "ast/ast_list.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser.h"
#include "utils/string_utils.h"

static int check_first(struct token tok)
{
    enum token_id first[] = { TOKEN_ID_IF,       TOKEN_ID_ASSIGNMENT_WORD,
                              TOKEN_ID_WORD,     TOKEN_ID_LESS,
                              TOKEN_ID_GREAT,    TOKEN_ID_CLOBBER,
                              TOKEN_ID_DGREAT,   TOKEN_ID_LESSAND,
                              TOKEN_ID_GREATAND, TOKEN_ID_LESSGREAT,
                              TOKEN_ID_LF,       TOKEN_ID_IO_NUMBER,
                              TOKEN_ID_NEG,      TOKEN_ID_FOR,
                              TOKEN_ID_WHILE,    TOKEN_ID_UNTIL,
                              TOKEN_ID_LBRACE,   TOKEN_ID_LPAR };

    // sizeof(first) / sizeof(first[0]) is the length of the array
    return is_token_in_list(tok, first, sizeof(first) / sizeof(first[0]));
}

// Check the FIRST of the optional and_or inside brackets
static int and_or_first(struct token tok)
{
    enum token_id first[] = { TOKEN_ID_IF,        TOKEN_ID_ASSIGNMENT_WORD,
                              TOKEN_ID_WORD,      TOKEN_ID_LESS,
                              TOKEN_ID_GREAT,     TOKEN_ID_CLOBBER,
                              TOKEN_ID_DGREAT,    TOKEN_ID_LESSAND,
                              TOKEN_ID_GREATAND,  TOKEN_ID_LESSGREAT,
                              TOKEN_ID_IO_NUMBER, TOKEN_ID_NEG,
                              TOKEN_ID_FOR,       TOKEN_ID_WHILE,
                              TOKEN_ID_UNTIL,     TOKEN_ID_LBRACE,
                              TOKEN_ID_LPAR };

    // sizeof(first) / sizeof(first[0]) is the length of the array
    return is_token_in_list(tok, first, sizeof(first) / sizeof(first[0]));
}

static int control_flow_check(struct token tok)
{
    enum token_id table[] = {
        TOKEN_ID_THEN, TOKEN_ID_FI,   TOKEN_ID_ELSE,
        TOKEN_ID_ELIF, TOKEN_ID_DONE, TOKEN_ID_DO,
    };

    // sizeof(first) / sizeof(first[0]) is the length of the array
    return is_token_in_list(tok, table, sizeof(table) / sizeof(table[0]));
}

// {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;
enum parser_status parse_compound_list(struct lexer *lexer,
                                       struct ast_node **res,
                                       struct io_backend *io)
{
    struct token tok = lexer_peek(lexer, io);
    if (!check_first(tok))
        return PARSER_UNEXPECTED;

    *res = init_ast(AST_LIST);
    struct ast_list *list = (struct ast_list *)*res;

    // * {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;
    tok = lexer_peek(lexer, io);
    while (tok.id == TOKEN_ID_LF)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);
    }

    // Stop the compound list when a else/elif/then/fi is seen
    // This is necessary to avoid parsing else/elif/then/fi as a word from a
    // simple_command
    if (control_flow_check(tok))
        return PARSER_OK;

    // {'\n'} * and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;
    if (parse_and_or(lexer, &(list->child), io) == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;

    tok = lexer_peek(lexer, io);

    // {'\n'} and_or * { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;
    // We can't know yet if there is a { ( ';' | '\n' ) {'\n'} and_or },
    // it doesn't matter as it is compatible with [';'] {'\n'}
    while (tok.id == TOKEN_ID_SEMI || tok.id == TOKEN_ID_LF)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);

        // { '\n' }
        while (tok.id == TOKEN_ID_LF)
        {
            lexer_pop(lexer);
            tok = lexer_peek(lexer, io);
        }

        // Stop the compound list when a else/elif/then/fi is seen
        if (control_flow_check(tok))
            return PARSER_OK;

        // and_or is optionnal, if it is there then we entered
        // { ( ';' | '\n' ) {'\n'} and_or }
        // else it was simply a [';'] {'\n'}
        if (!and_or_first(tok))
        {
            if (tok.id != TOKEN_ID_SEMI)
                return PARSER_OK;
            return PARSER_UNEXPECTED;
        }
        else if (parse_and_or(lexer, &(list->next), io) == PARSER_UNEXPECTED)
            return PARSER_UNEXPECTED;

        struct ast_node *new_next = init_ast(AST_LIST);
        struct ast_list *new_list = (struct ast_list *)new_next;
        new_list->child = list->next;
        list->next = new_next;
        list = (struct ast_list *)list->next;
        tok = lexer_peek(lexer, io);
    }

    return PARSER_OK;
}
