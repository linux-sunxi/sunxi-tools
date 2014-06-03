libfex_SOURCES	:= $(wildcard libfex/*.c)
libfex_OBJS	:= $(patsubst %.c,$(OBJDIR)/%.o,$(libfex_SOURCES))
libfex_LIB	:= $(O)/libfex.a

SRCS	+= $(libfex_SOURCES)
OBJS	+= $(libfex_OBJS)

libs: $(libfex_LIB)

$(libfex_LIB): $(libfex_OBJS)
