lib_LIBRARIES = libast.a

libast_a_SOURCES = ast.h ast.c \
				   ast_cmd.c ast_cmd.h \
				   ast_if.c ast_if.h \
				   ast_list.c ast_list.h \
				   ast_until.c ast_until.h \
				   ast_while.c ast_while.h \
				   ast_redir.c ast_redir.h \
				   ast_simple_cmd.c ast_simple_cmd.h \
				   ast_and_or.c ast_and_or.h \
				   ast_negation.h ast_negation.c \
				   ast_pipe.h ast_pipe.c \
				   ast_for.h ast_for.c \
				   ast_fundec.h ast_fundec.c \
				   ast_case.h ast_case.c

libast_a_CPPFLAGS = -I$(top_srcdir)/src

libast_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libast.a
