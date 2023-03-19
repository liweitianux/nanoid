/*-
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2023 Aaron LI
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * API
 * ---
 * id = nanoid.generate(length?, alphabet?)
 *
 * Both <length> and <alphabet> are optional; specify them to use custom
 * length and/or alphabet.
 *
 * Returns the generated ID, or nil if error occurred.
 */

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "nanoid.h"


static int
l_generate(lua_State *L)
{
    const unsigned char *alphabet;
    char *buf;
    size_t length, alphacnt;

    length = (size_t)luaL_optinteger(L, 1, NANOID_SIZE);
    alphabet = (const unsigned char *)luaL_optlstring(L, 2, NULL, &alphacnt);

    buf = malloc(length);
    if (buf == NULL)
        return luaL_error(L, "out of memory");

    if (nanoid_generate_r(buf, length, alphabet, alphacnt) == NULL)
        lua_pushnil(L);
    else
        lua_pushlstring(L, buf, length);

    free(buf);

    return 1;
}


int
luaopen_nanoid(lua_State *L)
{
    static const struct luaL_Reg funcs[] = {
        { "generate", l_generate },
        { NULL, NULL },
    };
    /* NOTE: require Lua >=5.2 or LuaJIT >=2.1 */
    luaL_newlib(L, funcs);

    /* Constants */
    lua_pushinteger(L, NANOID_SIZE);
    lua_setfield(L, -2, "SIZE");

    return 1;
}
