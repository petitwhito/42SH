#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast/ast_case.h"
#include "execution.h"
#include "expansion/word_expansion.h"
#include "utils/argv_utils.h"

static int execution_pair_case(struct pair_case *pair_case, char *var,
                               struct exit_status *exit_status)
{
    int i = 0;
    while (pair_case->condition[i])
    {
        struct expansion_res exp_res = expand_word(pair_case->condition[i]);
        if (exp_res.status != EXPANSION_OK)
        {
            fprintf(stderr, "case var: cannot expand %s\n",
                    pair_case->condition[i]);
            free_argv(exp_res.new_words);
            *exit_status = (struct exit_status){ 1, EXIT_ERROR };
            return 1;
        }

        char *val = argv_to_word(exp_res.new_words);

        if (!fnmatch(val, var, 0))
        {
            free(val);
            free_argv(exp_res.new_words);

            *exit_status = execution_ast(pair_case->todo);
            return 1;
        }
        free(val);
        free_argv(exp_res.new_words);
        i++;
    }
    return 0;
}

struct exit_status execution_case(struct ast_node *ast)
{
    struct ast_case *ast_case = (struct ast_case *)ast;
    struct exit_status exit_status;
    struct pair_case *pair_case = ast_case->pair_case;
    struct expansion_res exp_res = expand_word(ast_case->var);
    if (exp_res.status != EXPANSION_OK)
    {
        fprintf(stderr, "case var: cannot expand %s\n", ast_case->var);
        free_argv(exp_res.new_words);
        return (struct exit_status){ 1, EXIT_ERROR };
    }
    char *val = argv_to_word(exp_res.new_words);

    while (pair_case)
    {
        if (execution_pair_case(pair_case, val, &exit_status))
        {
            free(val);
            free_argv(exp_res.new_words);
            return exit_status;
        }
        pair_case = pair_case->next;
    }

    exit_status.exit_code = 0;
    exit_status.exit_action = EXIT_OK;
    free(val);
    free_argv(exp_res.new_words);
    return exit_status;
}
