local luabc = require("luabc")
local cmd   = luabc.cmd
local tool  = luabc.tool

local CC     = "gcc"
local CFLAGS = { "-Wall", "-Wextra", "-I./" }
local CSTD   = "-std=c11"
local TARGET = "bfjit"
local SRC    = tool.match_file_extension(".c", "src")
local CLEAR  = TARGET

local function build()
    local all = cmd:new()
    all:append(CC, CFLAGS, CSTD, "-o", TARGET, SRC)
    local debug = cmd:new("debug");
    debug:call(function () table.insert(CFLAGS, "-g") end)
    debug:link()

    luabc.build()
end
build()
