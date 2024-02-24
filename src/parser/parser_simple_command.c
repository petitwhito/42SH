#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "ast/ast_simple_cmd.h"
#include "environment/environment.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser.h"
#include "utils/argv_utils.h"
#include "utils/hash_map.h"
#include "utils/string_utils.h"

static int expand_alias(char *word, struct ast_simple_cmd *simple_cmd)
{
    struct hash_map *alias_hash_map = get_alias_hash_map();
    char *value = hash_map_get(alias_hash_map, word);
    if (!value)
        return 0;

    argv_append(&(simple_cmd->words), strdup(value));
    return 1;
}

static int check_first(struct token tok)
{
    enum token_id first[] = {
        TOKEN_ID_WORD,      TOKEN_ID_LESS,
        TOKEN_ID_GREAT,     TOKEN_ID_CLOBBER,
        TOKEN_ID_DGREAT,    TOKEN_ID_LESSAND,
        TOKEN_ID_GREATAND,  TOKEN_ID_LESSGREAT,
        TOKEN_ID_IO_NUMBER, TOKEN_ID_ASSIGNMENT_WORD,
    };

    // sizeof(first) / sizeof(first[0]) is the length of the array
    return is_token_in_list(tok, first, sizeof(first) / sizeof(first[0]));
}

//   prefix { prefix }
// | { prefix } WORD { element }
enum parser_status parse_simple_command(struct lexer *lexer,
                                        struct ast_node **res,
                                        struct io_backend *io)
{
    // prefix {prefix}
    //  {prefix} WORD {element}
    struct token tok = lexer_peek(lexer, io);

    if (!check_first(tok))
        return PARSER_UNEXPECTED;

    struct ast_node *node = init_ast(AST_SIMPLE_CMD);
    *res = node;
    struct ast_simple_cmd *simple_cmd = (struct ast_simple_cmd *)node;

    // remember that we found at least one prefix
    bool prefix_found = false;
    while (tok.id >= TOKEN_ID_LESS && tok.id <= TOKEN_ID_ASSIGNMENT_WORD)
    {
        prefix_found = true;
        if (parse_prefix(lexer, res, io) == PARSER_UNEXPECTED)
            return PARSER_UNEXPECTED;
        tok = lexer_peek(lexer, io);
    }

    // no WORD is acceptable only if we found at least one prefix before
    if (tok.type != TOKEN_WORD)
    {
        if (!prefix_found)
            return PARSER_UNEXPECTED;
        return PARSER_OK;
    }

    if (!expand_alias(tok.value, simple_cmd))
        argv_append(&(simple_cmd->words), strdup(tok.value));

    lexer_pop(lexer);
    tok = lexer_peek(lexer, io);

    // | { prefix } WORD * { element }
    while (tok.type == TOKEN_WORD || tok.type == TOKEN_IO_NUMBER
           || (tok.id >= TOKEN_ID_LESS && tok.id <= TOKEN_ID_LESSGREAT))
    {
        if (parse_element(lexer, res, io) == PARSER_UNEXPECTED)
            return PARSER_UNEXPECTED;
        tok = lexer_peek(lexer, io);
    }

    return PARSER_OK;
}
