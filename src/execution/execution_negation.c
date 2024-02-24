#include <stdlib.h>

#include "ast/ast.h"
#include "ast/ast_if.h"
#include "ast/ast_negation.h"
#include "execution.h"

struct exit_status execution_negation(struct ast_node *ast)
{
    struct ast_negation *ast_negation = (struct ast_negation *)ast;
    struct exit_status res = execution_ast(ast_negation->child);
    res.exit_code = !res.exit_code;
    return res;
}
