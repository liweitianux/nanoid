Nano ID
=======

A tiny, secure, URL-friendly, unique string ID generator for C and Lua
(both C API and LuaJIT FFI API).

For the Nano ID description and algorithm, please refer to
[ai's nanoid](https://github.com/ai/nanoid).

C Interface
-----------
### Usage
- Bundle the `nanoid.c` and `nanoid.h` source files with your project;
- Or, link your program to the `libnanoid.so` library.

### API
```c
void *
nanoid_generate_r(void *buf, size_t buflen, const unsigned char *alphabet,
                  size_t alphacnt);
```

Generates an ID of length `buflen` and stores into `buf`, using the given
alphabet `alphabet` of size `alphacnt`.  If `alphabet` is `NULL`, then the
default alphabet is used; `alphacnt` is also ignored in this case.

Returns a pointer to `buf` on success, or `NULL` on error.

This function is reentrantable (i.e., thread-safe), as indicated by the `_r`
suffix.

```c
const char *
nanoid_generate(const unsigned char *alphabet, size_t alphacnt);
```

Generates an ID of the default length (i.e., 21 as defined by `NANOID_SIZE`),
using the alphabet `alphabet` of size `alphacnt` if it isn't `NULL`.

This function uses an *internal static buffer* to store the ID and returns a
pointer to the internal buffer on success, or `NULL` on error.

Lua C Interface
---------------
### Usage
```lua
local nanoid = require("nanoid")
```

### API
```lua
id = nanoid.generate(length?, alphabet?)
```

Generates an ID.  Both `length` and `alphabet` are optional; specify them to
use custom length and/or alphabet.

Returns the generated ID, or nil if error occurred.

LuaJIT FFI Interface
--------------------
### Usage
```lua
local nanoid = require("nanoid.ffi")
```

### API
Same as the above Lua C interface.

CLI Utility
-----------
The `nanoid` CLI utility can be used to generate IDs, perform speed tests,
and test for uniformity.

```
% ./nanoid -h
Nano ID command utility.

Generate ID:
>>> ./nanoid [-a alphabet] [-l length]
    -a: specify the custom alphabet
    -l: specify the custom ID length

Speed test:
>>> ./nanoid speed [-b burnin] [-c count] [-l length]
    -b: specify the burn-in iterations (default: count/10)
    -c: specify the test iterations (default: 1000000)
    -l: specify the custom ID length

Distribution uniformity test:
>>> ./nanoid test
```

Credits
-------
* [ai/nanoid](https://github.com/ai/nanoid)
* [mcmikecreations/nanoid\_cpp](https://github.com/mcmikecreations/nanoid_cpp)
* [leosbotelho/nanoid-c](https://github.com/leosbotelho/nanoid-c)

License
-------
The MIT License.
