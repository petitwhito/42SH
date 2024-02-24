#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

struct lexer;

void init_error_handler(struct lexer *lex);

struct lexer *get_lexer(void);

void free_shell_data(void);

void exit_print(int exit_status, const char *err_msg);

void exit_noprint(int exit_status);

void set_future_exit_code(int exit);

#endif /* ! ERROR_HANDLER_H */
