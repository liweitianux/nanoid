package = "nanoid"
version = "master-1"

description = {
    summary = "A tiny, secure, URL-friendly, unique string ID generator",
    homepage = "https://github.com/liweitianux/nanoid",
    license = "MIT",
}

dependencies = {}

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
