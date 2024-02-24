#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "ast/ast_negation.h"
#include "ast/ast_pipe.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser.h"
#include "utils/string_utils.h"

enum parser_status parse_pipeline(struct lexer *lexer, struct ast_node **res,
                                  struct io_backend *io)
{
    struct token tok = lexer_peek(lexer, io);
    // Case when starts with neg
    if (tok.id == TOKEN_ID_NEG)
    {
        // Create parent ast negation
        struct ast_node *node = init_ast(AST_NEGATION);
        *res = node;
        struct ast_negation *ast_neg = (struct ast_negation *)node;
        res = &(ast_neg->child);
        lexer_pop(lexer);
    }

    // Create child ast pipe
    struct ast_node *node1 = init_ast(AST_PIPE);
    *res = node1; // Prevents memory leaks ?
    struct ast_pipe *ast_pipe = (struct ast_pipe *)node1;

    if (parse_command(lexer, &(ast_pipe->child), io) == PARSER_UNEXPECTED)
        return PARSER_UNEXPECTED;

    tok = lexer_peek(lexer, io);

    while (tok.id == TOKEN_ID_PIPE)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer, io);

        // Case for optionnal \n
        while (tok.id == TOKEN_ID_LF)
        {
            lexer_pop(lexer);
            tok = lexer_peek(lexer, io);
        }

        // check command
        if (parse_command(lexer, &(ast_pipe->next), io) == PARSER_UNEXPECTED)
            return PARSER_UNEXPECTED;
        tok = lexer_peek(lexer, io);

        // Add new pipe to the end of the pipe ast
        struct ast_node *new_next = init_ast(AST_PIPE);
        struct ast_pipe *new_pipe = (struct ast_pipe *)new_next;
        new_pipe->child = ast_pipe->next;
        ast_pipe->next = new_next;
        ast_pipe = (struct ast_pipe *)ast_pipe->next;
        tok = lexer_peek(lexer, io);
    }

    return PARSER_OK;
}
