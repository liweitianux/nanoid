all: nanoid lib

CFLAGS=	-g3 -O3 -std=c99 -pedantic -Wall -Wextra
CFLAGS+=-Wshadow -Wundef -Wformat=2 -Wformat-truncation=2 -Wconversion
CFLAGS+=-fno-common
CFLAGS+=-DNDEBUG -D_POSIX_C_SOURCE=200112L

ifneq ($(DEBUG),)
CFLAGS+=-ggdb3 -O0 -UNDEBUG -DDEBUG
endif

LDFLAGS=-lm

nanoid: nanoid_main.c nanoid.c nanoid.h
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

lib: libnanoid.so
libnanoid.so: nanoid.c nanoid.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

lualib: nanoid.so
nanoid.so: nanoid_lua.c nanoid.c nanoid.h
	$(CC) $(CFLAGS) -fPIC -shared -I$(LUA_INCDIR) -o $@ $^

clean:
	rm -f nanoid libnanoid.so nanoid.so
