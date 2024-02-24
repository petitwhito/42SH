#ifndef IO_H
#define IO_H

#include <stdio.h>

struct io_backend
{
    FILE *stream;
};

struct io_backend *new_io_stdin(void);

struct io_backend *new_io_file(char *path);

struct io_backend *new_io_string(char *string);

void free_backend(struct io_backend *io);

char io_next(struct io_backend *io);

char io_peek(struct io_backend *io);

#endif /* ! IO_H */
