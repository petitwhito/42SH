#ifndef TOKEN_RECOGNITION_H
#define TOKEN_RECOGNITION_H

#include "lexer.h"
#include "token.h"

enum lexer_status
{
    LEXER_OK,
    LEXER_DELIMIT,
    LEXER_ERROR,
};

struct io_backend;

void token_recognize_id(struct lexer *lexer);

enum lexer_status lex_single_quotes(struct lexer *lexer, struct io_backend *io);

enum lexer_status lex_backslash(struct lexer *lexer, struct io_backend *io);

enum lexer_status lex_double_quotes(struct lexer *lexer, struct io_backend *io);

enum lexer_status lex_dollar(struct lexer *lexer, struct io_backend *io);

enum lexer_status lex_backtick(struct lexer *lexer, struct io_backend *io);

enum lexer_status lexer_token_recognition(struct lexer *lexer, char c,
                                          struct io_backend *io);

#endif /* ! TOKEN_RECOGNITION_H */
