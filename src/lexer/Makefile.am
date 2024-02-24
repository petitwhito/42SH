lib_LIBRARIES = liblexer.a

liblexer_a_SOURCES =  lexer.c lexer.h \
					  token.h \
					  token_recognition.c token_recognition.h

liblexer_a_CPPFLAGS = -I$(top_srcdir)/src

liblexer_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

liblexer_a_LIBADD = ../utils/libutils.a

noinst_LIBRARIES = liblexer.a
