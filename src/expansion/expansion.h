#ifndef EXPANSION_H
#define EXPANSION_H

enum expansion_status
{
    EXPANSION_OK = 0,
    EXPANSION_ERROR,
    EXPANSION_EXIT,
};

struct expansion_res
{
    char **new_words;
    enum expansion_status status;
};

struct var;

enum expansion_status words_expansion(char ***words);

enum expansion_status var_assignation(struct var *begin);

#endif /* ! EXPANSION_H */
