lib_LIBRARIES = libparser.a

libparser_a_SOURCES = parser.h \
					  parser_and_or.c \
					  parser_command.c \
					  parser_element.c \
					  parser_input.c \
					  parser_pipeline.c \
					  parser_simple_command.c \
					  parser_list.c \
					  parser_rule_if.c \
					  parser_else_clause.c \
					  parser_compound_list.c \
					  parser_prefix.c \
					  parser_redirection.c \
					  parser_utils.c \
					  parser_rule_while.c \
					  parser_rule_until.c \
					  parser_shell_command.c \
					  parser_rule_for.c \
					  parser_fundec.c \
					  parser_rule_case.c

libparser_a_CPPFLAGS = -I$(top_srcdir)/src

libparser_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libparser.a
