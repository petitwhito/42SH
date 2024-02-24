lib_LIBRARIES = libenv.a

libenv_a_SOURCES = special_var.c special_var.h \
				   environment.c environment.h environment_func.c\
				   var.c var.h \
				   loops.c loops.h \
				   shell_instance.c shell_instance.h \
				   arguments.c arguments.h

libenv_a_CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

libenv_a_CPPFLAGS = -I$(top_srcdir)/src

noinst_LIBRARIES = libenv.a
