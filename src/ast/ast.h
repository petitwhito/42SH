#ifndef AST_H
#define AST_H

enum ast_type
{
    AST_CMD = 0,
    AST_SIMPLE_CMD,
    AST_IF,
    AST_LIST,
    AST_WHILE,
    AST_UNTIL,
    AST_REDIR,
    AST_AND_OR,
    AST_NEGATION,
    AST_PIPE,
    AST_FOR,
    AST_FUNDEC,
    AST_CASE,
};

typedef struct ast_node *(*init_func)(void);

struct ast_ftable;

struct ast_node
{
    enum ast_type type;
    struct ast_ftable *ftable;
    int ref_counter;
};

struct ast_ftable
{
    struct exit_status (*run)(struct ast_node *ast);
    void (*free)(struct ast_node *ast);
};

void reduce_ref_count(struct ast_node *ast);

struct ast_node *init_ast(enum ast_type type);

void free_ast(struct ast_node *ast);

#endif /* ! AST_H */
