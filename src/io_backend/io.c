#define _POSIX_C_SOURCE 200809L

#include "io.h"

#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "error_handler/error_handler.h"

struct io_backend *new_io_stdin(void)
{
    struct io_backend *res = malloc(sizeof(struct io_backend));
    if (!res)
        exit_print(2, "new_io_stdin: error allocating memory for io_backend");

    res->stream = stdin;
    return res;
}

struct io_backend *new_io_file(char *path)
{
    struct io_backend *res = malloc(sizeof(struct io_backend));
    if (!res)
        exit_print(2, "new_io_file: error allocating memory for io_backend");

    FILE *file = fopen(path, "r");

    // File does not exist
    if (errno == ENOENT)
    {
        free(res);
        errx(127, "new_io_file : cannot read file %s", path);
    }

    // File doesn't have permissions
    if (errno == EACCES)
    {
        free(res);
        errx(126, "new_io_file : permissions denied %s", path);
    }

    if (!file)
    {
        free(res);
        exit_print(127, "new_io_file: error opening the file");
    }

    res->stream = file;
    return res;
}

// string argument should be argv[2] with -c
struct io_backend *new_io_string(char *string)
{
    struct io_backend *res = malloc(sizeof(struct io_backend));
    if (!res)
        exit_print(2, "io_backend: error allocating memory for io_backend");

    // Open the string as a read-only stream
    FILE *file = fmemopen(string, strlen(string), "r");
    if (!file)
    {
        free(res);
        exit_print(127, "new_io_string: error opening the string as a stream");
    }

    res->stream = file;
    return res;
}

void free_backend(struct io_backend *io)
{
    fclose(io->stream);
    free(io);
}

char io_next(struct io_backend *io)
{
    if (!io)
        exit_print(127, "io_next: NULL pointer given as io_backend");

    char buf[2] = { [1] = '\0' };
    if (fread(&buf, 1, 1, io->stream) < 1)
        return EOF;

    char c = buf[0];
    return c;
}

// Return char at current position in the stream and put it back in the stream
// with ungetc
char io_peek(struct io_backend *io)
{
    if (!io)
        exit_print(127, "io_peek: NULL pointer given as io_backend");

    char buf[2] = { [1] = '\0' };
    if (fread(&buf, 1, 1, io->stream) < 1)
        return EOF;
    char c = buf[0];
    ungetc(c, io->stream);

    return c;
}
