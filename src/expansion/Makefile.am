lib_LIBRARIES = libexpansion.a

libexpansion_a_SOURCES = expansion.c expansion.h \
						 word_expansion.c word_expansion.h \
						 command_substitution.c command_substitution.h

libexpansion_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libexpansion_a_CPPFLAGS = -I$(top_srcdir)/src

noinst_LIBRARIES = libexpansion.a
