all: nanoid lib
lua: lib lualib

CFLAGS=	-g -O3 -std=c99 -pedantic -Wall -Wextra -DNDEBUG
CFLAGS+=-D_POSIX_C_SOURCE=200112L

ifneq ($(DEBUG),)
CFLAGS+=-ggdb3 -O0 -UNDEBUG -DDEBUG
endif

nanoid: nanoid_main.c nanoid.c nanoid.h
	$(CC) $(CFLAGS) -o $@ $^

lib: libnanoid.so
libnanoid.so: nanoid.c nanoid.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

lualib: nanoid.so
nanoid.so: nanoid_lua.c nanoid.c nanoid.h
	$(CC) $(CFLAGS) -fPIC -shared -I$(LUA_INCLUDE) -o $@ $^

clean:
	rm -f nanoid libnanoid.so nanoid.so

install-lua: lua
	@test -n "$(PREFIX)"
	install -d $(DESTDIR)$(PREFIX)/nanoid
	install -m 0555 nanoid.so $(DESTDIR)$(PREFIX)/nanoid.so
	install -m 0555 libnanoid.so $(DESTDIR)$(PREFIX)/libnanoid.so
	install -m 0444 nanoid.lua $(DESTDIR)$(PREFIX)/nanoid/ffi.lua
