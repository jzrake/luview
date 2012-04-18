

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "numarray.h"
#include "lunum.h"
#include "GL/glfw.h"


static int luaC_Init(lua_State *L)
{
  printf("init!\n");
  return 0;
}

int luaopen_luview(lua_State *L)
{
  printf("loading luview...\n");

  luaL_Reg luview_api[] = { { "Init", luaC_Init }, {NULL, NULL} };

  lua_newtable(L);
  luaL_setfuncs(L, luview_api, 0);

  lua_setglobal(L, "luview");
  lua_getglobal(L, "luview");

  return 1;
}
