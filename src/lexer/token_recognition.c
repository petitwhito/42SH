#define _POSIX_C_SOURCE 200809L

#include "token_recognition.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "environment/var.h"
#include "io_backend/io.h"
#include "lexer.h"
#include "utils/string_utils.h"

static enum lexer_status lex_dollar_par(struct lexer *lexer,
                                        struct io_backend *io);

static enum token_id token_recognise_assignment(struct lexer *lexer)
{
    char *word = lexer->current_tok.value;
    if (word[0] == '=')
        return TOKEN_ID_ERROR;
    if (word[0] < '0' && word[0] > '9')
        return TOKEN_ID_ERROR;

    char *eq = strchr(word, '=');
    if (eq)
    {
        char *key = strdup(word);
        char *eq2 = strchr(key, '=');
        *eq2 = '\0';
        if (is_valid_name(key))
        {
            free(key);
            return TOKEN_ID_ASSIGNMENT_WORD;
        }
        free(key);
    }

    return TOKEN_ID_ERROR;
}

void token_recognize_id(struct lexer *lexer)
{
    enum token_id id = TOKEN_ID_ERROR;
    if (lexer->current_tok.type == TOKEN_EOF)
        id = TOKEN_ID_EOF;

    if (!lexer->current_tok.value)
    {
        lexer->current_tok.id = id;
        return;
    }
    if (lexer->current_tok.type == TOKEN_WORD)
        id = TOKEN_ID_WORD;

    char *id_values[] = { "if",    "then", "elif", "else", "fi",   "while",
                          "until", "for",  "!",    "&&",   "||",   ";",
                          "\n",    "|",    "<",    ">",    ">|",   ">>",
                          "<&",    ">&",   "<>",   "do",   "done", "in",
                          "{",     "}",    "(",    ")",    "case", "esac",
                          ";;" };
    enum token_id identifiers[] = {
        TOKEN_ID_IF,      TOKEN_ID_THEN,     TOKEN_ID_ELIF,
        TOKEN_ID_ELSE,    TOKEN_ID_FI,       TOKEN_ID_WHILE,
        TOKEN_ID_UNTIL,   TOKEN_ID_FOR,      TOKEN_ID_NEG,
        TOKEN_ID_AND,     TOKEN_ID_OR,       TOKEN_ID_SEMI,
        TOKEN_ID_LF,      TOKEN_ID_PIPE,     TOKEN_ID_LESS,
        TOKEN_ID_GREAT,   TOKEN_ID_CLOBBER,  TOKEN_ID_DGREAT,
        TOKEN_ID_LESSAND, TOKEN_ID_GREATAND, TOKEN_ID_LESSGREAT,
        TOKEN_ID_DO,      TOKEN_ID_DONE,     TOKEN_ID_IN,
        TOKEN_ID_LBRACE,  TOKEN_ID_RBRACE,   TOKEN_ID_LPAR,
        TOKEN_ID_RPAR,    TOKEN_ID_CASE,     TOKEN_ID_ESAC,
        TOKEN_ID_DSEMI
    };

    enum token_id tmp = token_recognise_assignment(lexer);
    if (tmp != TOKEN_ID_ERROR)
        id = tmp;

    if (lexer->current_tok.type == TOKEN_IO_NUMBER)
        id = TOKEN_ID_IO_NUMBER;

    for (size_t i = 0; i < sizeof(identifiers) / sizeof(identifiers[0]); ++i)
    {
        if (strcmp(lexer->current_tok.value, id_values[i]) == 0)
            id = identifiers[i];
    }

    lexer->current_tok.id = id;
}

static int is_io_number(struct lexer *lexer)
{
    if (lexer->size == 0)
        return 0;

    for (size_t i = 0; i < lexer->size; ++i)
    {
        if (!isdigit(lexer->buffer[i]))
            return 0;
    }

    return 1;
}

// &   &&   (   )   ;   ;;   newline   |   ||
// <     >     >|     <<     >>     <&     >&     <<-     <>
static int is_operator(struct lexer *lexer, char c)
{
    char *tmp = malloc(lexer->size + 2);
    tmp = strcpy(tmp, lexer->buffer);
    tmp[lexer->size] = c;
    tmp[lexer->size + 1] = '\0';

    char *operators[] = { "&&", "(",  ")",  ";",  "\n", "|",  "||", "<",
                          ">",  ">|", ">>", "<&", ">&", "<>", ";;" };

    size_t len = sizeof(operators) / sizeof(operators[0]);
    for (size_t i = 0; i < len; ++i)
    {
        if (strcmp(tmp, operators[i]) == 0)
        {
            free(tmp);
            return 1;
        }
    }

    free(tmp);
    return 0;
}

static int is_first_op_char(char c)
{
    return (c == '&' || c == '(' || c == ')' || c == ';' || c == '\n'
            || c == '|' || c == '<' || c == '>');
}

// Rule 10 - Start of new word
static enum lexer_status rule_new_word(struct lexer *lexer, char c)
{
    append_buffer(lexer, c);
    lexer->current_tok.type = TOKEN_WORD;
    return LEXER_OK;
}

// Rule 9 - Comments #, newline acts as EOF in the grammar
static enum lexer_status rule_comment(struct lexer *lexer, char c,
                                      struct io_backend *io)
{
    if (c == '#' && lexer->size == 0)
    {
        while (c != '\n')
        {
            io_next(io);
            c = io_peek(io);
        }

        append_buffer(lexer, c);
        lexer->current_tok.type = TOKEN_OPERATOR;
        return LEXER_DELIMIT;
    }

    return rule_new_word(lexer, c);
}

// Rule 8 - Prev Word + Word char
static enum lexer_status rule_prev_word(struct lexer *lexer, char c,
                                        struct io_backend *io)
{
    if (lexer->current_tok.type == TOKEN_WORD)
    {
        append_buffer(lexer, c);
        return LEXER_OK;
    }

    return rule_comment(lexer, c, io);
}

// Rule 7 - Unquoted blank
static enum lexer_status rule_unquoted_blank(struct lexer *lexer, char c,
                                             struct io_backend *io)
{
    if (isblank(c))
    {
        if (lexer->size == 0)
            return LEXER_OK;
        return LEXER_DELIMIT;
    }

    return rule_prev_word(lexer, c, io);
}

// Rule 6 - First OP char => delimit if buffer not empty
static enum lexer_status rule_first_op(struct lexer *lexer, char c,
                                       struct io_backend *io)
{
    if (is_first_op_char(c))
    {
        // Redirections
        if ((c == '<' || c == '>') && is_io_number(lexer))
            lexer->current_tok.type = TOKEN_IO_NUMBER;

        if (lexer->size > 0)
            return LEXER_DELIMIT;

        lexer->current_tok.type = TOKEN_OPERATOR;
        append_buffer(lexer, c);
        return LEXER_OK;
    }

    return rule_unquoted_blank(lexer, c, io);
}

// Rule 4 - Quoting
// Lex single quotes
enum lexer_status lex_single_quotes(struct lexer *lexer, struct io_backend *io)
{
    if (lexer->current_tok.type == TOKEN_OPERATOR)
        return LEXER_DELIMIT;

    // Put first ' char in buffer
    char c = io_peek(io);
    append_buffer(lexer, c);

    // Current token type is set to a WORD
    lexer->current_tok.type = TOKEN_WORD;

    // Get next char
    io_next(io);
    c = io_peek(io);

    // Feed into the buffer as is until we find the closing ' char
    while (c != EOF)
    {
        append_buffer(lexer, c);
        if (c == '\'')
        {
            io_next(io);
            return LEXER_OK;
        }

        io_next(io);
        c = io_peek(io);
    }

    return LEXER_ERROR;
}

// Lex backslash
enum lexer_status lex_backslash(struct lexer *lexer, struct io_backend *io)
{
    if (lexer->current_tok.type == TOKEN_OPERATOR)
        return LEXER_DELIMIT;

    // Get next char
    io_next(io);
    char c = io_peek(io);

    // Put the next char in the buffer regardless of what it is
    if (c != '\n')
    {
        // Put \ char in buffer
        append_buffer(lexer, '\\');
        if (c != EOF)
            append_buffer(lexer, c);

        lexer->current_tok.type = TOKEN_WORD;
    }

    io_next(io);
    c = io_peek(io);
    return LEXER_OK;
}

// Lex double quotes
enum lexer_status lex_double_quotes(struct lexer *lexer, struct io_backend *io)
{
    if (lexer->current_tok.type == TOKEN_OPERATOR)
        return LEXER_DELIMIT;

    // Put first " char in buffer
    char c = io_peek(io);
    append_buffer(lexer, c);

    // Current token type is set to a WORD
    lexer->current_tok.type = TOKEN_WORD;

    // Get next char
    io_next(io);
    c = io_peek(io);

    // Feed into the buffer as is until we find the closing " char
    while (c != EOF)
    {
        c = io_peek(io);
        if (c == '$')
        {
            if (lex_dollar(lexer, io) == LEXER_ERROR)
                return LEXER_ERROR;
            continue;
        }
        else if (c == '\\')
        {
            // Backslash within double quotes retains its special meaning if
            // followed by $, `, ", \, or \n

            // \n joining is a special case, prevent it from being added in the
            // = buffer as well as the backslash itself
            io_next(io);
            c = io_peek(io);
            if (c == '\n')
            {
                io_next(io);
                continue;
            }

            append_buffer(lexer, '\\');
            if (c == '$' || c == '`' || c == '"' || c == '\\' || c == '\n')
            {
                append_buffer(lexer, c);
                io_next(io);
                continue;
            }
        }

        append_buffer(lexer, c);
        // Found closing "
        if (c == '"')
        {
            io_next(io);
            return LEXER_OK;
        }

        io_next(io);
        c = io_peek(io);
    }

    return LEXER_ERROR;
}

// Rule 5 - Expansion
static enum lexer_status lex_dollar_bracket(struct lexer *lexer,
                                            struct io_backend *io)
{
    char c = io_peek(io);
    while (c != EOF)
    {
        if (c == '}')
        {
            append_buffer(lexer, c);
            io_next(io);
            return LEXER_OK;
        }

        append_buffer(lexer, c);
        io_next(io);
        c = io_peek(io);
    }

    // No closing bracket is an error
    return LEXER_ERROR;
}

static enum lexer_status lex_dollar_par(struct lexer *lexer,
                                        struct io_backend *io)
{
    char c = io_peek(io);

    while (c != EOF)
    {
        c = io_peek(io);
        enum lexer_status status = LEXER_OK;
        if (c == '\'')
            status = lex_single_quotes(lexer, io);
        else if (c == '"')
            status = lex_double_quotes(lexer, io);
        else if (c == '\\')
            status = lex_backslash(lexer, io);
        else if (c == '$')
            status = lex_dollar(lexer, io);
        else
        {
            append_buffer(lexer, c);

            if (c == ')')
            {
                io_next(io);
                return LEXER_OK;
            }

            io_next(io);
            c = io_peek(io);
        }

        if (status == LEXER_ERROR)
            return LEXER_ERROR;
    }

    // No closing parenthesis is an error
    return LEXER_ERROR;
}

static enum lexer_status lex_cmd_sub(struct lexer *lexer, struct io_backend *io)
{
    io_next(io);

    if (lex_dollar_par(lexer, io) == LEXER_ERROR)
        return LEXER_ERROR;
    return LEXER_OK;
}

enum lexer_status lex_dollar(struct lexer *lexer, struct io_backend *io)
{
    if (lexer->current_tok.type == TOKEN_OPERATOR)
        return LEXER_DELIMIT;

    // Put $ char in buffer
    char c = io_peek(io);
    append_buffer(lexer, c);

    // Current token type is set to a WORD
    lexer->current_tok.type = TOKEN_WORD;

    // Get next char
    io_next(io);
    c = io_peek(io);

    if (c == '(')
    {
        append_buffer(lexer, c);
        if (lex_cmd_sub(lexer, io) == LEXER_ERROR)
            return LEXER_ERROR;
        return LEXER_OK;
    }
    else if (c == '{')
    {
        if (lex_dollar_bracket(lexer, io) == LEXER_ERROR)
            return LEXER_ERROR;
        return LEXER_OK;
    }

    return LEXER_OK;
}

enum lexer_status lex_sub_backtick(struct lexer *lexer, struct io_backend *io)
{
    char c = io_peek(io);

    while (c != EOF)
    {
        c = io_peek(io);
        enum lexer_status status = LEXER_OK;
        if (c == '\'')
            status = lex_single_quotes(lexer, io);
        else if (c == '"')
            status = lex_double_quotes(lexer, io);
        else if (c == '\\')
        {
            append_buffer(lexer, c);

            io_next(io);
            c = io_peek(io);
            if (c != EOF)
            {
                if (c == '$' || c == '`' || c == '\\')
                {
                    append_buffer(lexer, c);
                    io_next(io);
                    continue;
                }
            }
        }
        else
        {
            append_buffer(lexer, c);

            if (c == '`')
            {
                io_next(io);
                return LEXER_OK;
            }

            io_next(io);
            c = io_peek(io);
        }

        if (status == LEXER_ERROR)
            return LEXER_ERROR;
    }

    // No closing backtick is an error
    return LEXER_ERROR;
}

enum lexer_status lex_backtick(struct lexer *lexer, struct io_backend *io)
{
    if (lexer->current_tok.type == TOKEN_OPERATOR)
        return LEXER_DELIMIT;

    // Put ` char in buffer
    char c = io_peek(io);
    append_buffer(lexer, c);

    // Current token type is set to a WORD
    lexer->current_tok.type = TOKEN_WORD;

    // Get next char
    io_next(io);
    c = io_peek(io);

    if (lex_sub_backtick(lexer, io) == LEXER_ERROR)
        return LEXER_ERROR;
    return LEXER_OK;
}

// Rule 2 and 3 - Prev OP + OP char
static enum lexer_status rule_prev_op(struct lexer *lexer, char c,
                                      struct io_backend *io)
{
    if (lexer->current_tok.type == TOKEN_OPERATOR)
    {
        // If char cannot be added to current OP, delimit (Rule 3)
        // else add to OP buffer (Rule 2)
        if (!is_operator(lexer, c))
            return LEXER_DELIMIT;

        append_buffer(lexer, c);
        return LEXER_OK;
    }

    return rule_first_op(lexer, c, io);
}

// Rule 1 - EOF
static enum lexer_status rule_eof(struct lexer *lexer, char c,
                                  struct io_backend *io)
{
    if (c == EOF)
    {
        if (lexer->size == 0)
            lexer->current_tok.type = TOKEN_EOF;
        return LEXER_DELIMIT;
    }
    return rule_prev_op(lexer, c, io);
}

enum lexer_status lexer_token_recognition(struct lexer *lexer, char c,
                                          struct io_backend *io)
{
    return rule_eof(lexer, c, io);
}
