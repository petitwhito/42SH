#ifndef ARGV_UTILS_H
#define ARGV_UTILS_H

#include <stddef.h>

size_t argv_len(char **argv);

char *argv_to_word(char **argv);

char **argv_copy(char **argv);

int argv_append(char ***argv, char *str);

int argv_remove(char ***argv, size_t pos);

int argv_insert(char ***argv, char *str, size_t pos);

void argv_replace(char **argv, char *str, size_t pos);

void merge_argv(char ***argv1, char **argv2, size_t pos);

void free_argv(char **argv);

#endif /* ! ARGV_UTILS_H */
