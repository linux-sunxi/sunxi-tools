libfex_SOURCES	:= $(wildcard libfex/*.c)
libfex_OBJS	:= $(patsubst %.c,$(OBJDIR)/%.o,$(libfex_SOURCES))
libfex_LIB	:= $(O)/libfex.a
libfex_SO	:= $(O)/libfex.so.1.0

SRCS	+= $(libfex_SOURCES)
OBJS	+= $(libfex_OBJS)

LIBS	+= $(libfex_LIB)
$(libfex_LIB): $(libfex_OBJS)

SOLIBS	+= $(libfex_SO)
$(libfex_SO): $(libfex_OBJS)
