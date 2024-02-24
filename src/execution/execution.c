#define _POSIX_C_SOURCE 200809L

#include "execution.h"

#include "ast/ast.h"

struct exit_status execution_ast(struct ast_node *ast)
{
    if (!ast)
        return (struct exit_status){ 0, EXIT_OK };
    struct exit_status res = (ast->ftable->run)(ast);
    return res;
}
