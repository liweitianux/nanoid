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

local clock = os.clock
local floor = math.floor
local format = string.format

function benchmark(count, burnin, f, ...)
    burnin = burnin or floor(count / 10)
    local tstart, tend

    print(format("Burning in ... (n=%d)", burnin))
    for i = 1, burnin do
        f(...)
    end

    print(format("Running test ... (n=%d)", count))
    tstart = clock()
    for i = 1, count do
        f(...)
    end
    tend = clock()

    local v1 = floor((tend - tstart) * 1e9 / count)
    local v2 = floor(count / (tend - tstart))
    print(format("Speed: %d ns/id, %d id/s", v1, v2))
end

print(">>> Lua C Interface")
local nanoid = require("nanoid")
benchmark(2000000, nil, nanoid.generate)

if _G.jit then
    print(">>> LuaJIT FFI Interface")
    local nanoid = require("nanoid.ffi")
    benchmark(2000000, nil, nanoid.generate)
end
