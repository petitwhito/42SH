#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast_for.h"
#include "environment/loops.h"
#include "environment/var.h"
#include "error_handler/error_handler.h"
#include "execution.h"
#include "expansion/expansion.h"
#include "utils/argv_utils.h"
#include "utils/hash_map.h"

static void restore_words(struct ast_for *ast, char **backup)
{
    free_argv(ast->words);
    ast->words = backup;
}

static enum expansion_status expansion(char ***words)
{
    // Words expansion
    if (words_expansion(words) == EXPANSION_ERROR)
        return EXPANSION_ERROR;

    return EXPANSION_OK;
}

struct exit_status execution_for(struct ast_node *ast)
{
    struct ast_for *ast_for = (struct ast_for *)ast;
    struct exit_status status = { 0, EXIT_OK };
    loop_append_level();

    // Expand words and check for bad substitution
    char **copy_words = argv_copy(ast_for->words);
    if (expansion(&ast_for->words) == EXPANSION_ERROR)
    {
        free_argv(copy_words);
        fprintf(stderr, "bad substitution");
        return (struct exit_status){ 2, EXIT_ERROR };
    }

    if (!is_valid_name(ast_for->var))
    {
        free_argv(copy_words);
        fprintf(stderr, "invalid variable identifier: %s\n", ast_for->var);
        return (struct exit_status){ 2, EXIT_ERROR };
    }

    struct hash_map *hash_map = get_hash_map();

    for (int i = 0; ast_for->words[i]; i++)
    {
        int b = 1;
        hash_map_insert(hash_map, strdup(ast_for->var),
                        strdup(ast_for->words[i]), &b);

        status = execution_ast(ast_for->todo);
        if (status.exit_action != EXIT_OK)
            break;

        struct loop_info *info = get_loop_info();
        if (info->action == LOOP_BREAK)
        {
            info->loop_remaining--;
            if (info->loop_remaining == 0)
                loop_reset_action();
            break;
        }

        else if (info->action == LOOP_CONTINUE)
        {
            info->loop_remaining--;
            if (info->loop_remaining == 0)
                loop_reset_action();
        }
    }

    restore_words(ast_for, copy_words);

    loop_reduce_level();
    return status;
}
