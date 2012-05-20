#!/bin/bash

lua="."
luabin=$(pwd)/bin

PATH=${PATH/${luabin}:/""} # Remove lua/bin from path if it exists

export PATH=${luabin}:$PATH
export LUA_CPATH=$(pwd)/lib/?.so
export LUA_PATH=$(pwd)/lib/lua/5.2/?.lua
export LUA_HOME=$(pwd)
export LUVIEW_HOME=$(pwd)

echo "*** run with 'source' to export these environment variables for build: ***"
echo "PATH=${PATH}"
echo "LUA_CPATH=${LUA_CPATH}"
echo "LUA_PATH=${LUA_PATH}"
echo "LUA_HOME=${LUA_HOME}"
