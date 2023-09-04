all: nanoid libnanoid.so

CFLAGS=	-g3 -O3 -std=c99 -pedantic -fPIC -fno-common -Wall -Wextra
CFLAGS+=-Wshadow -Wundef -Wformat=2 -Wformat-truncation=2 -Wconversion
CFLAGS+=-DNDEBUG

ifeq ($(shell uname -s),Linux)
CFLAGS+=-D_POSIX_C_SOURCE=200112L -D_DEFAULT_SOURCE
endif

ifneq ($(DEBUG),)
CFLAGS+=-ggdb3 -O0 -UNDEBUG -DDEBUG
endif

LDFLAGS=-lm

nanoid: nanoid_main.o nanoid.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

libnanoid.so: nanoid.o
	$(CC) $(CFLAGS) -shared -o $@ $^

lualib: nanoid.so
nanoid.so: nanoid_lua.o nanoid.o
	$(CC) $(CFLAGS) -shared -o $@ $^

nanoid.o: nanoid.c nanoid.h nanoid_rand.h
nanoid_main.o: nanoid_main.c nanoid.h

nanoid_lua.o: nanoid_lua.c nanoid.h
	$(CC) $(CFLAGS) -I$(LUA_INCDIR) -o $@ -c $<

clean:
	rm -f nanoid libnanoid.so nanoid.so *.o *.gch
