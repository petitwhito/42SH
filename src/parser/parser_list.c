#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "ast/ast_list.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser.h"
#include "utils/string_utils.h"

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
                              TOKEN_ID_LPAR,      TOKEN_ID_CASE };

    // sizeof(first) / sizeof(first[0]) is the length of the array
    return is_token_in_list(tok, first, sizeof(first) / sizeof(first[0]));
}

enum parser_status parse_list(struct lexer *lexer, struct ast_node **res,
                              struct io_backend *io)
{
    *res = init_ast(AST_LIST);
    struct ast_list *list = (struct ast_list *)*res;

    // * and_or { ';' and_or } [ ';' ]
    if (parse_and_or(lexer, &(list->child), io) == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;
    struct token tok = lexer_peek(lexer, io);
    while (tok.id == TOKEN_ID_SEMI)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);

        // { ';' * and_or }
        // check for optionnal and_or and put its resulting node
        // in list->next if PARSER_OK
        // Then, put this node in the child of a new list node
        if (!and_or_first(tok))
        {
            // [ ';' ]
            // if not it is last ';' of line/input
            // break to stop making lists
            if (tok.id != TOKEN_ID_SEMI)
                return PARSER_OK;
            return PARSER_UNEXPECTED;
        }
        else if ((parse_and_or(lexer, &(list->next), io) == PARSER_UNEXPECTED))
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
