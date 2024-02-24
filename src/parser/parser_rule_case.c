#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "ast/ast_case.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser.h"
#include "utils/argv_utils.h"
#include "utils/string_utils.h"
static int check_first_compound_list(struct token tok)
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

    return is_token_in_list(tok, first, sizeof(first) / sizeof(first[0]));
}

static void add_cond(struct pair_case *pair_case, char *argv)
{
    argv_append(&(pair_case->condition), argv);
}

static struct pair_case *init_pair(void)
{
    struct pair_case *pair_case = calloc(1, sizeof(struct pair_case));
    pair_case->condition = calloc(1, sizeof(char *));
    pair_case->todo = NULL;
    pair_case->next = NULL;
    return pair_case;
}

static void add_pair(struct ast_case *ast_case, struct pair_case *pair_case)
{
    if (!ast_case->pair_case)
    {
        ast_case->pair_case = pair_case;
        return;
    }
    struct pair_case *old = ast_case->pair_case;
    while (old->next)
    {
        old = old->next;
    }
    old->next = pair_case;
}

static enum parser_status parse_case_item(struct lexer *lexer,
                                          struct ast_case *res,
                                          struct io_backend *io)
{
    struct token tok = lexer_peek(lexer, io);
    if (tok.id == TOKEN_ID_LPAR)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);
    }
    if (tok.type != TOKEN_WORD)
        return PARSER_UNEXPECTED;
    struct pair_case *pair_case = init_pair();
    add_pair(res, pair_case);
    add_cond(pair_case, strdup(tok.value));
    lexer_pop(lexer);
    tok = lexer_peek(lexer, io);
    while (tok.id == TOKEN_ID_PIPE)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);
        if (tok.type != TOKEN_WORD)
            return PARSER_UNEXPECTED;
        add_cond(pair_case, strdup(tok.value));
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);
    }
    if (tok.id != TOKEN_ID_RPAR)
        return PARSER_UNEXPECTED;
    lexer_pop(lexer);

    tok = lexer_peek(lexer, io);
    while (tok.id == TOKEN_ID_LF)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);
    }
    if (check_first_compound_list(tok)
        && parse_compound_list(lexer, &pair_case->todo, io)
            == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;
    return PARSER_OK;
}
static enum parser_status parse_case_clause(struct lexer *lexer,
                                            struct ast_case *res,
                                            struct io_backend *io)
{
    if (parse_case_item(lexer, res, io) == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;
    struct token tok = lexer_peek(lexer, io);
    while (tok.id == TOKEN_ID_DSEMI)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);
        while (tok.id == TOKEN_ID_LF)
        {
            lexer_pop(lexer);
            tok = lexer_peek(lexer, io);
        }
        if (tok.id == TOKEN_ID_ESAC)
            break;
        if (parse_case_item(lexer, res, io) == PARSER_UNEXPECTED)
            return PARSER_UNEXPECTED;
        tok = lexer_peek(lexer, io);
    }
    return PARSER_OK;
}
enum parser_status parse_rule_case(struct lexer *lexer, struct ast_node **res,
                                   struct io_backend *io)
{
    struct token tok = lexer_peek(lexer, io);
    if (tok.id != TOKEN_ID_CASE)
        return PARSER_UNEXPECTED;

    struct ast_node *node = init_ast(AST_CASE);
    *res = node;
    struct ast_case *ast_case = (struct ast_case *)node;

    lexer_pop(lexer);

    tok = lexer_peek(lexer, io);
    if (tok.type != TOKEN_WORD)
        return PARSER_UNEXPECTED;
    ast_case->var = strdup(tok.value);
    lexer_pop(lexer);

    tok = lexer_peek(lexer, io);
    while (tok.id == TOKEN_ID_LF)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);
    }
    if (tok.id != TOKEN_ID_IN)
        return PARSER_UNEXPECTED;
    lexer_pop(lexer);

    tok = lexer_peek(lexer, io);
    while (tok.id == TOKEN_ID_LF)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);
    }

    if (tok.id != TOKEN_ID_ESAC)
    {
        if (parse_case_clause(lexer, ast_case, io) == PARSER_UNEXPECTED)
            return PARSER_UNEXPECTED;
        tok = lexer_peek(lexer, io);
    }

    if (tok.id != TOKEN_ID_ESAC)
        return PARSER_UNEXPECTED;

    lexer_pop(lexer);

    return PARSER_OK;
}
