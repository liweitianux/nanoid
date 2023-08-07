-- Package 'nanoid' already exists, so change to use 'lua-nanoid'.
package = "lua-nanoid"
version = "master-1"

description = {
    summary = "A tiny, secure, URL-friendly, unique string ID generator for Lua/LuaJIT",
    homepage = "https://github.com/liweitianux/nanoid",
    license = "MIT",
}

dependencies = {
    "lua >= 5.1",
}

source = {
    url = "git://github.com/liweitianux/nanoid",
    branch = "master",
}

local _defines = { "NDEBUG" }

build = {
    type = "builtin",
    modules = {
        ["nanoid.ffi"] = "nanoid.lua",
        ["libnanoid"] = {
            sources = { "nanoid.c" },
            defines = _defines,
        },
        ["nanoid"] = {
            sources = { "nanoid_lua.c", "nanoid.c" },
            defines = _defines,
        },
    },
}
