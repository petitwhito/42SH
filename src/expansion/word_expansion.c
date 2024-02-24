#define _POSIX_C_SOURCE 200809L

#include "word_expansion.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "command_substitution.h"
#include "environment/arguments.h"
#include "environment/environment.h"
#include "environment/shell_instance.h"
#include "environment/special_var.h"
#include "environment/var.h"
#include "error_handler/error_handler.h"
#include "execution/execution.h"
#include "expansion.h"
#include "io_backend/io.h"
#include "utils/argv_utils.h"
#include "utils/hash_map.h"
#include "utils/string_utils.h"

static enum expansion_status expand_dollar_par(struct expansion_data *data,
                                               char **cmd, size_t *cmd_len);
static enum expansion_status expand_cmd_sub_dollar(struct expansion_data *data,
                                                   char **sub_value);
static enum expansion_status expand_double_quotes(struct expansion_data *data,
                                                  char **buffer,
                                                  size_t *buf_len,
                                                  int in_cmd_sub);

static void append_buffer(struct expansion_data *data, char c)
{
    data->buffer = realloc(data->buffer, data->buf_len + 2);
    data->buffer[data->buf_len] = c;
    data->buffer[data->buf_len + 1] = '\0';
    data->buf_len++;
}

// Use this function in case the expansion needs to spawn a new word (ex: $@)
// This function will add the current buffer in the new_words array and will
// reset the buffer to an empty one to begin a new word
static void shift_word(struct expansion_data *data)
{
    data->word_count++;
    data->new_words =
        realloc(data->new_words, sizeof(char *) * (data->word_count + 1));

    // Copy and append the data->buffer in the new_words count
    data->new_words[data->word_count - 1] = strdup(data->buffer);
    // NULL-terminate the new_words array for future execvp
    data->new_words[data->word_count] = NULL;

    // Reset the buffer
    data->buffer = realloc(data->buffer, 1);
    data->buf_len = 0;
    data->buffer[0] = '\0';
}

static int is_first_char_special(char c)
{
    return isdigit(c) || c == '@' || c == '*' || c == '?' || c == '$'
        || c == '#';
}

static int is_valid_full_var(char *name, size_t len)
{
    int res = 0;
    if (len == 1 && is_first_char_special(name[0]))
        res = 1;
    else if (is_valid_name(name))
        res = 1;
    else
        res = is_number(name);

    return res;
}

// Temporarily appends the c char in the name str and see if it is still valid
static int is_valid_var(char *name, size_t len, char c)
{
    char *tmp_name = calloc(len + 2, 1);
    strcat(tmp_name, name);
    tmp_name[len] = c;
    int res = is_valid_full_var(tmp_name, len + 1);
    free(tmp_name);
    return res;
}

static int is_special_var(char *word, size_t ind)
{
    if (ind != 0)
    {
        char c = word[ind];
        return c == '@' || c == '*' || c == '?' || c == '$' || c == '#'
            || isdigit(c);
    }

    char *spec_var[] = { "@",      "*",   "?",      "$",   "#",
                         "RANDOM", "UID", "OLDPWD", "PWD", "IFS" };
    size_t spec_len = sizeof(spec_var) / sizeof(spec_var[0]);
    for (size_t i = 0; i < spec_len; ++i)
    {
        if (strcmp(word, spec_var[i]) == 0)
            return 1;
    }

    return is_number(word);
}

static char *handle_special_variables(char *word)
{
    // These variables have special meaning
    if (strcmp(word, "*") == 0)
        return argv_star_sv();
    if (strcmp(word, "?") == 0)
        return int_to_str(get_last_val());
    if (strcmp(word, "$") == 0)
        return int_to_str(getpid());
    if (is_number(word))
        return argv_sv(atoi(word));
    if (strcmp(word, "#") == 0)
        return int_to_str(get_argc());

    // Check if RANDOM has been unset or not
    char *rdm = hash_map_get(get_hash_map(), "RANDOM");
    if (rdm && strcmp(word, "RANDOM") == 0)
        return int_to_str(rand() % 32768);

    if (strcmp(word, "UID") == 0)
        return int_to_str(getuid());
    if (strcmp(word, "OLDPWD") == 0)
        return old_pwd_sv();
    if (strcmp(word, "PWD") == 0)
        return pwd_sv();
    if (strcmp(word, "IFS") == 0)
        return ifs_sv();
    return NULL;
}

static char *get_variable(char *word)
{
    // Check if the variable is a special one
    if (get_random_state() && is_special_var(word, 0))
        return handle_special_variables(word);

    // Else, attempt to retrieve it in the environment
    char *env = getenv(word);
    if (env)
        return strdup(env);

    // If not in environment, check hash map
    char *var = hash_map_get(get_hash_map(), word);
    // If the variable is unset in the hash map, return NULL
    if (!var)
        return NULL;
    return strdup(var);
}

static void handle_at_sv(struct expansion_data *data, char **argv, size_t argc)
{
    // Append argument 0 at end of current buffer and add it to data->new_words
    string_insert_substr(&data->buffer, argv[0], data->buf_len);
    data->buf_len += strlen(argv[0]);
    shift_word(data);

    // Add to data->new_words arguments 1 to n-1
    for (size_t i = 1; i < argc - 1; i++)
    {
        string_insert_substr(&data->buffer, argv[i], 0);
        data->buf_len += strlen(argv[i]);
        shift_word(data);
    }

    // Prepend argument n to beginning of buffer
    string_insert_substr(&data->buffer, argv[argc - 1], 0);
    data->buf_len += strlen(argv[argc - 1]);
    free_argv(argv);
}

static void variable_substitution(struct expansion_data *data, char *var_name)
{
    char *val = NULL;

    // Check for $@ special case
    if (strcmp(var_name, "@") == 0)
    {
        char **argv = argv_at_sv();
        int argc = argv_len(argv);

        // Simple case of $@, only one argument means it is a simple
        // substitution
        if (argc == 1)
            val = strdup(argv[0]);

        // We need to  : append the first arg of $@ with the current
        // data->buffer, add every args until the last (excluded) in the
        // data->new_words, and finally fill the last arg in data->buffer
        // (prepend it)
        else if (argc > 1)
        {
            handle_at_sv(data, argv, argc);
            return;
        }

        free_argv(argv);
    }
    else
        val = get_variable(var_name);
    // a NULL val means the variable is not set <=> nothing to insert
    if (val)
    {
        string_insert_substr(&data->buffer, val, data->buf_len);
        data->buf_len += strlen(val);
    }

    free(val);
}

static enum expansion_status expand_single_quotes(struct expansion_data *data,
                                                  char **buffer,
                                                  size_t *buf_len)
{
    size_t old_len = *buf_len;
    data->i++;

    // Feed into the buffer as is until we find the closing ' char
    while (data->i < data->len)
    {
        if (data->word[data->i] == '\'')
        {
            data->i++;
            if (old_len == *buf_len)
                data->empty_quotes_in_word = 1;
            return EXPANSION_OK;
        }

        string_append_char(buffer, (*buf_len)++, data->word[data->i++]);
    }
    return EXPANSION_ERROR;
}

static enum expansion_status expand_backslash(struct expansion_data *data,
                                              char **buffer, size_t *buf_len)
{
    data->i++;

    // Put the next char in the buffer regardless of what it is
    if (data->i < data->len)
        string_append_char(buffer, (*buf_len)++, data->word[data->i++]);

    return EXPANSION_OK;
}

static enum expansion_status expand_dollar_bracket(struct expansion_data *data,
                                                   char **name,
                                                   size_t *name_len)
{
    while (data->i < data->len)
    {
        if (data->word[data->i] == '}')
        {
            // Variable name MUST be valid if between brackets
            if (*name_len == 0 || !is_valid_full_var(*name, *name_len))
                return EXPANSION_ERROR;
            return EXPANSION_OK;
        }

        string_append_char(name, (*name_len)++, data->word[data->i++]);
    }

    return EXPANSION_ERROR;
}

static enum expansion_status expand_variables(struct expansion_data *data,
                                              char **buffer, size_t *buf_len,
                                              int in_cmd_sub)
{
    // Backup current data->i in case it is not a special $
    size_t start = data->i;
    data->i++;

    // Potential variable name ahead, store in a temporary external buffer
    // instead of data->buffer for future verification
    // '$' is not stored in name itself
    char *name = calloc(1, 1);
    size_t name_len = 0;
    name[0] = '\0';

    // Get potential name
    // Variable declaration can be enclosed in brackets
    if (!in_cmd_sub && data->word[data->i] == '{')
    {
        data->i++;
        enum expansion_status status =
            expand_dollar_bracket(data, &name, &name_len);
        if (status != EXPANSION_OK)
        {
            free(name);
            return status;
        }
        data->i++;
    }
    // If no brackets fill name buffer until it is no longer a valid variable
    // name
    else if (!in_cmd_sub)
    {
        while (data->i < data->len
               && is_valid_var(name, name_len, data->word[data->i]))
        {
            string_append_char(&name, (name_len)++, data->word[data->i++]);
        }
    }
    // Not a variable, $ must be taken alone as a simple char,
    // restore data->i position and return to main loop
    if (name_len == 0)
    {
        data->i = start;
        string_append_char(buffer, (*buf_len)++, data->word[data->i++]);
        free(name);
        return EXPANSION_OK;
    }

    // Proceed to substitution for the found variable
    variable_substitution(data, name);
    free(name);
    return EXPANSION_OK;
}

static enum expansion_status expand_dollar_par(struct expansion_data *data,
                                               char **cmd, size_t *cmd_len)
{
    enum expansion_status status;
    while (data->i < data->len)
    {
        // Quoting characters should still be interpreted as such to avoid
        // 'fake' $( or )
        if (data->word[data->i] == '\'')
        {
            string_append_char(cmd, (*cmd_len)++, data->word[data->i]);
            if ((status = expand_single_quotes(data, cmd, cmd_len))
                != EXPANSION_OK)
                return status;
            string_append_char(cmd, (*cmd_len)++, '\'');
        }
        else if (data->word[data->i] == '"')
        {
            string_append_char(cmd, (*cmd_len)++, data->word[data->i]);
            if ((status = expand_double_quotes(data, cmd, cmd_len, 1))
                != EXPANSION_OK)
                return status;
            string_append_char(cmd, (*cmd_len)++, '"');
        }
        else if (data->word[data->i] == '\\')
        {
            string_append_char(cmd, (*cmd_len)++, data->word[data->i]);
            if ((status = expand_backslash(data, cmd, cmd_len)) != EXPANSION_OK)
                return status;
        }
        else
        {
            // Closing parenthesis means end of dollar command substitution
            if (data->word[data->i] == ')')
                return EXPANSION_OK;

            // If we come accross a nested $() command substitution, don't
            // process it, it will be processed in a further shell instance
            // Simply get all its characters until the end
            else if (data->word[data->i] == '$'
                     && data->word[data->i + 1] == '(')
            {
                // Add '$(' before calling recursively
                string_append_char(cmd, (*cmd_len)++, data->word[data->i++]);
                string_append_char(cmd, (*cmd_len)++, data->word[data->i++]);
                if ((status = expand_dollar_par(data, cmd, cmd_len))
                    != EXPANSION_OK)
                    return status;

                if (data->word[data->i] != ')')
                    return EXPANSION_ERROR;

                // Don't forget to append the ending ')' as well
                string_append_char(cmd, (*cmd_len)++, data->word[data->i]);
            }
            else
                string_append_char(cmd, (*cmd_len)++, data->word[data->i]);

            data->i++;
        }
    }

    return EXPANSION_ERROR;
}

static enum expansion_status expand_cmd_sub_dollar(struct expansion_data *data,
                                                   char **sub_value)
{
    data->i += 2;

    char *cmd = calloc(1, 1);
    size_t cmd_len = 0;
    cmd[0] = '\0';

    enum expansion_status status = expand_dollar_par(data, &cmd, &cmd_len);
    if (status != EXPANSION_OK)
    {
        free(cmd);
        return status;
    }

    // Execute substitution then remove trailing newlines
    struct sub_res res = execute_substitution(cmd);
    if (res.status != EXPANSION_OK)
    {
        free(cmd);
        return res.status;
    }
    size_t buf_len = strlen(res.buffer);
    string_remove_trailing_newline(&res.buffer, &buf_len);
    *sub_value = strdup(res.buffer);

    free(res.buffer);
    free(cmd);
    return EXPANSION_OK;
}

static enum expansion_status expand_dollar(struct expansion_data *data,
                                           char **buffer, size_t *buf_len,
                                           int in_cmd_sub)
{
    if (data->word[data->i + 1] == '(')
    {
        char *sub_value = NULL;
        enum expansion_status status = expand_cmd_sub_dollar(data, &sub_value);
        if (status != EXPANSION_OK)
            return status;
        string_insert_substr(buffer, sub_value, *buf_len);
        *buf_len += strlen(sub_value);
        data->i++;
        free(sub_value);
        return EXPANSION_OK;
    }

    return expand_variables(data, buffer, buf_len, in_cmd_sub);
}

static enum expansion_status substitute_backtick(struct expansion_data *data,
                                                 char **cmd, size_t *cmd_len)
{
    while (data->i < data->len)
    {
        if (data->word[data->i] == '\\')
        {
            char c = data->word[data->i + 1];

            // Backslash keeps its special meaning in a backtick behind a
            // '$', '`' or '\'

            // Append the backslash to the cmd buffer
            // (literal meaning)
            if (c != '$' && c != '`' && c != '\\')
                string_append_char(cmd, (*cmd_len)++, data->word[data->i]);

            // Don't append the backslash to buffer if '$', '`', or '\' behind
            // (special meaning)
            enum expansion_status status = expand_backslash(data, cmd, cmd_len);
            if (status != EXPANSION_OK)
                return status;

            continue;
        }

        // Non-escaped backtick means end of command substitution
        if (data->word[data->i] == '`')
            return EXPANSION_OK;
        else
            string_append_char(cmd, (*cmd_len)++, data->word[data->i]);

        data->i++;
    }

    return EXPANSION_ERROR;
}

static enum expansion_status
expand_cmd_sub_backtick(struct expansion_data *data, char **sub_value)
{
    data->i++;

    // Initialize sub buffer that will hold the command substitution itself
    char *cmd = calloc(1, 1);
    size_t cmd_len = 0;
    cmd[0] = '\0';

    // Retrieve the backtick command substitution sub string
    enum expansion_status status = substitute_backtick(data, &cmd, &cmd_len);
    if (status != EXPANSION_OK)
    {
        free(cmd);
        return status;
    }

    // Execute substitution then remove trailing newlines and add it to main
    // buffer
    // char *buffer = execute_substitution(cmd);
    struct sub_res res = execute_substitution(cmd);
    if (res.status != EXPANSION_OK)
    {
        free(cmd);
        return res.status;
    }
    size_t buf_len = strlen(res.buffer);
    string_remove_trailing_newline(&res.buffer, &buf_len);
    *sub_value = strdup(res.buffer);

    free(res.buffer);
    free(cmd);
    return EXPANSION_OK;
}

static enum expansion_status expand_backtick(struct expansion_data *data)
{
    // Start the backtick command substitution
    // sub_value will contain the final result after execution of the
    // substitution
    char *sub_value = NULL;

    enum expansion_status status = expand_cmd_sub_backtick(data, &sub_value);
    if (status != EXPANSION_OK)
        return status;

    // Insert the sub_value result in the main data->buffer
    string_insert_substr(&data->buffer, sub_value, data->buf_len);
    data->buf_len += strlen(sub_value);
    data->i++;
    free(sub_value);
    return EXPANSION_OK;
}

static enum expansion_status expand_double_quotes(struct expansion_data *data,
                                                  char **buffer,
                                                  size_t *buf_len,
                                                  int in_cmd_sub)
{
    size_t old_len = *buf_len;
    data->i++;

    // Feed into the buffer as is until we find the closing " char
    while (data->i < data->len)
    {
        // $ special meaning (parameter expansion or command substitution)
        if (data->word[data->i] == '$')
        {
            enum expansion_status status =
                expand_dollar(data, buffer, buf_len, in_cmd_sub);
            if (status != EXPANSION_OK)
                return status;
            continue;
        }
        // ` special meaning (command substitution)
        else if (data->word[data->i] == '`')
        {
            enum expansion_status status = expand_backtick(data);
            if (status != EXPANSION_OK)
                return status;
            continue;
        }
        else if (data->word[data->i] == '\\')
        {
            // Backslash within double quotes retains its special meaning if
            // followed by $, `, ", \, or \n

            // If next character is not one of these, add the backslash itself
            // (literal meaning)
            char c = data->word[data->i + 1];
            if (in_cmd_sub
                || (c != '$' && c != '`' && c != '"' && c != '\\' && c != '\n'))
            {
                string_append_char(buffer, (*buf_len)++, data->word[data->i]);
            }

            enum expansion_status status =
                expand_backslash(data, buffer, buf_len);
            if (status != EXPANSION_OK)
                return status;
            continue;
        }

        // Found closing "
        if (data->word[data->i] == '"')
        {
            data->i++;
            if (old_len == *buf_len)
                data->empty_quotes_in_word = 1;
            return EXPANSION_OK;
        }

        string_append_char(buffer, (*buf_len)++, data->word[data->i++]);
    }

    return EXPANSION_ERROR;
}

static void free_data(struct expansion_data *data)
{
    free_argv(data->new_words);
    free(data->buffer);
}

// Expand a single word into a new_words array NULL-terminated
struct expansion_res expand_word(char *word)
{
    // Init data structure to store expansion buffer informations
    struct expansion_data data = {
        word, strlen(word), 0, calloc(1, 1), 0, calloc(1, sizeof(char *)), 0, 0
    };

    // Main loop, if a special char is found call the corresponding function
    // else just add it to the data->buffer
    enum expansion_status status = EXPANSION_OK;
    while (status == EXPANSION_OK && data.i < data.len)
    {
        if (data.word[data.i] == '\'')
            status = expand_single_quotes(&data, &data.buffer, &data.buf_len);
        else if (data.word[data.i] == '"')
            status =
                expand_double_quotes(&data, &data.buffer, &data.buf_len, 0);
        else if (data.word[data.i] == '\\')
            status = expand_backslash(&data, &data.buffer, &data.buf_len);
        else if (data.word[data.i] == '$')
            status = expand_dollar(&data, &data.buffer, &data.buf_len, 0);
        else if (data.word[data.i] == '`')
            status = expand_backtick(&data);
        else
        {
            append_buffer(&data, data.word[data.i]);
            data.i++;
        }
    }

    // Check if error happened or need to exit, free and return NULL
    if (status != EXPANSION_OK)
    {
        free_data(&data);
        return (struct expansion_res){ NULL, status };
    }

    // Return NULL if word empty
    if (!data.empty_quotes_in_word && data.buf_len == 0)
    {
        free_data(&data);
        return (struct expansion_res){ NULL, status };
    }

    // shift_word adds the finalized word in the data->new_words array
    shift_word(&data);
    free(data.buffer);
    return (struct expansion_res){ data.new_words, EXPANSION_OK };
}
