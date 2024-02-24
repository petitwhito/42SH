#include "var.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void free_var(struct var *var)
{
    if (var == NULL)
        return;

    struct var *nvar = var->next;
    while (var != NULL)
    {
        free(var->prev_value);
        free(var->key);
        free(var->value);
        free(var);
        var = nvar;
        if (var == NULL)
            return;
        nvar = var->next;
    }
}

static struct var *get_var_error(struct var *var)
{
    free_var(var);
    return NULL;
}

struct var *append_var(struct var *var, char *word)
{
    if (!var)
        return get_var(word);

    struct var *tmp = var;
    while (tmp->next)
        tmp = tmp->next;

    struct var *next = get_var(word);
    tmp->next = next;
    return var;
}

struct var *get_var(char *word)
{
    struct var *var = calloc(1, sizeof(struct var));
    if (!var)
        return NULL;

    size_t i = 0;
    var->key = malloc(1);
    var->next = NULL;
    var->prev_value = NULL;

    while (word[i] != '=' && word[i])
    {
        var->key[i] = word[i];
        var->key = realloc(var->key, i + 2);
        if (!var->key)
            return get_var_error(var);
        i++;
    }
    var->key[i] = '\0';
    if (word[i] != '=')
        return get_var_error(var);
    i++;
    size_t j = 0;
    var->value = malloc(1);
    while (word[i])
    {
        var->value[j] = word[i];
        var->value = realloc(var->value, j + 2);
        if (!var->value)
            return get_var_error(var);
        j++;
        i++;
    }
    var->value[j] = '\0';
    return var;
}

// Check if a string is a valid XBD name
int is_valid_name(char *word)
{
    if (!word || strlen(word) == 0 || isdigit(word[0]))
        return 0;

    for (size_t i = 0; word[i] != '\0'; ++i)
    {
        char c = word[i];
        if (c != '_' && !isdigit(c) && !isalpha(c))
            return 0;
    }

    return 1;
}

// Check if a string is a valid XBD alias name
int is_valid_alias(char *word)
{
    if (!word || strlen(word) == 0)
        return 0;

    for (size_t i = 0; word[i] != '\0'; ++i)
    {
        char c = word[i];
        if (c != '!' && c != '%' && c != ',' && c != '@' && c != '_'
            && !isdigit(c) && !isalpha(c))
            return 0;
    }

    return 1;
}
