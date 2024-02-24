#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

#include "token.h"

struct io_backend;

struct lexer
{
    char *buffer;
    size_t size;
    struct token current_tok;
    struct token next_tok;
};

struct lexer *new_lexer(void);

void free_lexer(struct lexer *lexer);

int append_buffer(struct lexer *lexer, char c);

struct token lexer_peek(struct lexer *lexer, struct io_backend *io);

struct token lexer_peek2(struct lexer *lexer, struct io_backend *io);

void lexer_pop(struct lexer *lexer);

#endif /* ! LEXER_H */
