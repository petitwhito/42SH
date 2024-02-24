bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

42sh_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDADD = error_handler/liberr.a \
			 parser/libparser.a \
			 lexer/liblexer.a \
			 io_backend/libio_backend.a \
			 ast/libast.a \
			 execution/libexecution.a \
			 expansion/libexpansion.a \
			 redirection/libredirection.a \
			 environment/libenv.a \
			 utils/libutils.a

SUBDIRS = error_handler utils io_backend lexer ast parser execution redirection expansion environment
