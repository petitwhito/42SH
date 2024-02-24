lib_LIBRARIES = libexecution.a

libexecution_a_SOURCES = execution.c execution.h \
						 builtin/builtin.c builtin/builtin.h \
						 builtin/builtin_false.c \
						 builtin/builtin_true.c \
						 builtin/builtin_echo.c \
						 builtin/builtin_exit.c \
						 builtin/builtin_cd.c \
						 builtin/builtin_export.c \
						 builtin/builtin_unset.c \
						 builtin/builtin_break.c \
						 builtin/builtin_continue.c \
						 builtin/builtin_dot.c \
						 builtin/builtin_alias.c \
						 builtin/builtin_unalias.c \
						 execution_cmd.c \
						 execution_if.c \
						 execution_list.c \
						 execution_until.c \
						 execution_while.c \
						 execution_simple_cmd.c \
						 execution_and_or.c \
						 execution_negation.c \
						 execution_pipe.c \
						 execution_for.c \
						 execution_fundec.c \
						 execution_case.c

libexecution_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libexecution_a_CPPFLAGS = -I$(top_srcdir)/src

noinst_LIBRARIES = libexecution.a
