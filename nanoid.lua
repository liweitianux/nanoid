--[[
-- SPDX-License-Identifier: MIT
--
-- Copyright (c) 2023 Aaron LI
--
-- Permission is hereby granted, free of charge, to any person obtaining
-- a copy of this software and associated documentation files (the
-- "Software"), to deal in the Software without restriction, including
-- without limitation the rights to use, copy, modify, merge, publish,
-- distribute, sublicense, and/or sell copies of the Software, and to
-- permit persons to whom the Software is furnished to do so, subject
-- to the following conditions:
--
-- The above copyright notice and this permission notice shall be
-- included in all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
-- EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
-- MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
-- IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
-- CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
-- TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
-- SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--]]

--[[
Nano ID LuaJIT FFI interface

API
---
id = nanoid.generate(length?, alphabet?)

Both <length> and <alphabet> are optional; specify them to use custom
length and/or alphabet.

Returns the generated ID, or nil if error occurred.
--]]

local ffi = require("ffi")


-- Load library by searching 'package.cpath'.
-- Credit: https://github.com/api7/lua-resty-radixtree (lib/resty/radixtree.lua)
local function load_shared_lib(libname)
    local cpath = package.cpath
    local tried_paths = {}
    local i = 1

    for path in string.gmatch(cpath, "[^;]+") do
        local fpath = string.match(path, "(.*/)")
        fpath = fpath .. libname
        -- The only way to know if a file exist is trying to open it.
        local f = io.open(fpath)
        if f then
            io.close(f)
            return ffi.load(fpath)
        end
        tried_paths[i] = fpath
        i = i + 1
    end

    return nil, tried_paths
end

local libname = "libnanoid.so"
local nanoid, tried_paths = load_shared_lib(libname)
if not nanoid then
    error("cannot find library [" .. libname .. "] at paths:\n" ..
          table.concat(tried_paths, "\n"))
end


ffi.cdef[[
// #define NANOID_SIZE     21
static const int NANOID_SIZE = 21;

void *nanoid_generate_r(void *buf, size_t buflen,
                        const unsigned char *alphabet, size_t alphacnt);
]]


local get_buffer
do
    local _buf_type = ffi.typeof("unsigned char[?]")
    local _buf_size = 0
    local _buf

    function get_buffer(size)
        if size > _buf_size then
            _buf_size = size
            _buf = ffi.new(_buf_type, _buf_size)
        end
        return _buf
    end
end


local function generate(length, alphabet)
    length = length or nanoid.NANOID_SIZE
    local alphacnt = alphabet and #alphabet or 0

    local buf = get_buffer(length)
    local id = nanoid.nanoid_generate_r(buf, length, alphabet, alphacnt)
    if id == nil then
        return nil
    else
        return ffi.string(buf, length)
    end
end


return {
    SIZE = nanoid.NANOID_SIZE,
    generate = generate,
}
