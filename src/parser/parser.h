#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

#include "ast/ast.h"
#include "lexer/lexer.h"

enum parser_status
{
    PARSER_UNEXPECTED,
    PARSER_OK,
    PARSER_FINISHED,
    PARSER_ERROR,
};

int is_token_in_list(struct token tok, enum token_id *first_list, size_t len);

enum parser_status parse_input(struct lexer *lexer, struct ast_node **res,
                               struct io_backend *io);

enum parser_status parse_list(struct lexer *lexer, struct ast_node **res,
                              struct io_backend *io);

enum parser_status parse_and_or(struct lexer *lexer, struct ast_node **res,
                                struct io_backend *io);

enum parser_status parse_pipeline(struct lexer *lexer, struct ast_node **res,
                                  struct io_backend *io);

enum parser_status parse_command(struct lexer *lexer, struct ast_node **res,
                                 struct io_backend *io);

enum parser_status parse_simple_command(struct lexer *lexer,
                                        struct ast_node **res,
                                        struct io_backend *io);

enum parser_status parse_shell_command(struct lexer *lexer,
                                       struct ast_node **res,
                                       struct io_backend *io);

enum parser_status parse_rule_if(struct lexer *lexer, struct ast_node **res,
                                 struct io_backend *io);

enum parser_status parse_element(struct lexer *lexer, struct ast_node **res,
                                 struct io_backend *io);

enum parser_status parse_else_clause(struct lexer *lexer, struct ast_node **res,
                                     struct io_backend *io);

enum parser_status parse_compound_list(struct lexer *lexer,
                                       struct ast_node **res,
                                       struct io_backend *io);

enum parser_status parse_prefix(struct lexer *lexer, struct ast_node **res,
                                struct io_backend *io);

enum parser_status parse_redirection(struct lexer *lexer, struct ast_node **res,
                                     struct io_backend *io);

enum parser_status parse_rule_while(struct lexer *lexer, struct ast_node **res,
                                    struct io_backend *io);

enum parser_status parse_rule_until(struct lexer *lexer, struct ast_node **res,
                                    struct io_backend *io);

enum parser_status parse_rule_for(struct lexer *lexer, struct ast_node **res,
                                  struct io_backend *io);

enum parser_status parse_fundec(struct lexer *lexer, struct ast_node **res,
                                struct io_backend *io);

enum parser_status parse_rule_case(struct lexer *lexer, struct ast_node **res,
                                   struct io_backend *io);

#endif /* ! PARSER_H */
