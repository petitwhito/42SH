#ifndef VAR_H
#define VAR_H

struct var
{
    char *key;
    char *value;
    char *prev_value;
    struct var *next;
};

void free_var(struct var *var);

struct var *append_var(struct var *var, char *word);

struct var *get_var(char *word);

int is_valid_name(char *word);

int is_valid_alias(char *word);

#endif /* ! VAR_H */
