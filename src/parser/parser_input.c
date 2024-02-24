#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser.h"
#include "utils/string_utils.h"

static int check_first(struct token tok)
{
    enum token_id first[] = {
        TOKEN_ID_IF,      TOKEN_ID_WHILE,    TOKEN_ID_ASSIGNMENT_WORD,
        TOKEN_ID_UNTIL,   TOKEN_ID_WORD,     TOKEN_ID_LESS,
        TOKEN_ID_GREAT,   TOKEN_ID_CLOBBER,  TOKEN_ID_DGREAT,
        TOKEN_ID_LESSAND, TOKEN_ID_GREATAND, TOKEN_ID_LESSGREAT,
        TOKEN_ID_LF,      TOKEN_ID_EOF,      TOKEN_ID_IO_NUMBER,
        TOKEN_ID_NEG,     TOKEN_ID_FOR,      TOKEN_ID_LBRACE,
        TOKEN_ID_LPAR,    TOKEN_ID_CASE
    };

    // sizeof(first) / sizeof(first[0]) is the length of the array
    return is_token_in_list(tok, first, sizeof(first) / sizeof(first[0]));
}

enum parser_status parse_input(struct lexer *lexer, struct ast_node **res,
                               struct io_backend *io)
{
    struct token tok = lexer_peek(lexer, io);
    if (!check_first(tok))
        return PARSER_ERROR;

    // EOF
    // Lexer can't make anymore token <=> io_backend finished its input
    // EOF in first place can only happen if input is over
    if (tok.type == TOKEN_EOF)
    {
        lexer_pop(lexer);
        return PARSER_FINISHED;
    }

    // \n
    // Newline in first place means current AST is finished
    // (doesn't mean io backend finished its input)
    if (tok.id == TOKEN_ID_LF)
    {
        lexer_pop(lexer);
        return PARSER_OK;
    }

    // list \n
    // list EOF
    if (!check_first(tok))
    {
        free_ast(*res);
        *res = NULL;
        return PARSER_ERROR;
    }

    // *res = init_ast(AST_LIST);
    // struct ast_list *list = (struct ast_list *) *res;
    if (parse_list(lexer, res, io) == PARSER_UNEXPECTED)
    {
        (*res)->ref_counter = 0;
        free_ast(*res);
        *res = NULL;
        return PARSER_ERROR;
    }

    (*res)->ref_counter = 0;
    tok = lexer_peek(lexer, io);
    if (tok.type == TOKEN_EOF)
        return PARSER_OK;

    if (tok.id == TOKEN_ID_LF)
    {
        lexer_pop(lexer);
        return PARSER_OK;
    }

    return PARSER_ERROR;
}
