#ifndef ARGUMENTS_H
#define ARGUMENTS_H

/**
 * \brief Initialize the main_argv static variable containing the
 *        original arguments from the argv of the 42sh main function
 * \param argv: argv from the main function of 42sh
 *        offset: offset value to add to the argv to get to the arguments
 */
void init_main_argv(char **argv, int offset);

/**
 * \brief Get the current arguments
 * \param *argv Pointer to **words of the AST_CMD
 * \return the current argv containing $1..$n
 */
char **get_current_args(void);

/**
 * \brief Update the current arguments with the ones provided
 * \param new_args: the new argv to set as the current arguments
 */
void set_current_args(char **new_args);

/**
 * \brief Get the argument count
 * \return an integer that contains the current argc (argument count)
 */
int get_argc(void);

#endif /* ! ARGUMENTS_H */
