lib_LIBRARIES = libutils.a

libutils_a_SOURCES = string_utils.c string_utils.h \
					 argv_utils.c argv_utils.h \
					 hash_map.c hash_map.h \
					 hash_map_func.c hash_map_func.h

libutils_a_CPPFLAGS = -I$(top_srcdir)/src

libutils_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libutils.a
