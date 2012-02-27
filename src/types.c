

#include <stdlib.h>
#include <stdio.h>
#include "luview.h"


static int LuviewTraits_new(lua_State *L);
static int LuviewTraits_update(lua_State *L);

static int LuviewVec3_new(lua_State *L);
static int LuviewVec3_index(lua_State *L);
static int LuviewVec3_newindex(lua_State *L);
static int LuviewVec3_tostring(lua_State *L);

GETSET3(LuviewTraits, Position);
GETSET3(LuviewTraits, Orientation);
GETSET3(LuviewTraits, Color);
GETSET1(LuviewTraits, Scale);
GETSET1(LuviewTraits, LineWidth);
GETSETBOOL(LuviewTraits, HasFocus);
GETSETBOOL(LuviewTraits, IsVisible);

static const luaL_Reg LuviewTraits_getters[] = {
  { "Position"   , get_LuviewTraits_Position },
  { "Orientation", get_LuviewTraits_Orientation },
  { "Color"      , get_LuviewTraits_Color },
  { "Scale"      , get_LuviewTraits_Scale },
  { "LineWidth"  , get_LuviewTraits_LineWidth },
  { "HasFocus"   , get_LuviewTraits_HasFocus },
  { "IsVisible"  , get_LuviewTraits_IsVisible },
  { NULL, NULL} };

static const luaL_Reg LuviewTraits_setters[] = {
  { "Position"   , set_LuviewTraits_Position },
  { "Orientation", set_LuviewTraits_Orientation },
  { "Color"      , set_LuviewTraits_Color },
  { "Scale"      , set_LuviewTraits_Scale },
  { "LineWidth"  , set_LuviewTraits_LineWidth },
  { "HasFocus"   , set_LuviewTraits_HasFocus },
  { "IsVisible"  , set_LuviewTraits_IsVisible },
  { NULL, NULL} };

int lv_register_types(lua_State *L)
{
  const int luview = lua_gettop(L);
  {
    const luaL_Reg class_methods[] = {
      { "new"     , LuviewTraits_new },
      { "update"  , LuviewTraits_update },
      { NULL, NULL} };

    lua_newtable(L); // class
    luaL_setfuncs(L, class_methods, 0);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index"); // Traits.__index = Traits
    lua_setfield(L, luview, "Traits");
  }

  {
    const luaL_Reg class_methods[] = {
      { "new"            , LuviewVec3_new },
      { "__index"        , LuviewVec3_index },
      { "__newindex"     , LuviewVec3_newindex },
      { "__tostring"     , LuviewVec3_tostring },
      { NULL, NULL} };

    lua_newtable(L); // class
    luaL_setfuncs(L, class_methods, 0);
    lua_setfield(L, luview, "Vec3");
  }

  return 0;
}



int LuviewVec3_new(lua_State *L)
{
#include "proxy.lc" // leaves the make_proxy function on top of stack

  lua_pushvalue(L, 1); // Vec3 class

  double *v = (double*) lua_newuserdata(L, 3*sizeof(double));
  lua_rawgeti(L, 2, 1); v[0] = lua_tonumber(L, -1); lua_pop(L, 1);
  lua_rawgeti(L, 2, 2); v[1] = lua_tonumber(L, -1); lua_pop(L, 1);
  lua_rawgeti(L, 2, 3); v[2] = lua_tonumber(L, -1); lua_pop(L, 1);

  lua_pushnil(L); // getters
  lua_pushnil(L); // setters

  lua_pushvalue(L, 1);
  lua_setmetatable(L, -4);
  lua_pcall(L, 4, 1, 0); // make_proxy(class, priv, getters, setters)

  return 1;
}
int LuviewVec3_index(lua_State *L)
{
  luaL_getmetafield(L, 1, "priv");
  double *v = (double*) lua_touserdata(L, -1); lua_pop(L, 1);
  int n = luaL_checkinteger(L,2);
  if (0 > n || n > 3) luaL_error(L, "index %d out of range", n);
  lua_pushnumber(L, v[n-1]);
  return 1;
}
int LuviewVec3_newindex(lua_State *L)
{
  luaL_getmetafield(L, 1, "priv");
  double *v = (double*) lua_touserdata(L, -1); lua_pop(L, 1);
  int n = luaL_checkinteger(L,2);
  if (0 > n || n > 3) luaL_error(L, "index %d out of range", n);
  v[n-1] = luaL_checknumber(L, 3);
  return 0;
}
int LuviewVec3_tostring(lua_State *L)
{
  luaL_getmetafield(L, 1, "priv");
  double *v = (double*) lua_touserdata(L, -1); lua_pop(L, 1);
  lua_pushfstring(L, "(%f, %f, %f)", v[0], v[1], v[2]);
  return 1;
}



int LuviewTraits_update(lua_State *L)
// stack: (1) Traits instance, (2) update table
{
  lua_pushnil(L);
  while (lua_next(L, 2) != 0) lua_setfield(L, 1, lua_tostring(L, -2));
  return 0;
}

int LuviewTraits_new(lua_State *L)
{
#include "proxy.lc" // leaves the make_proxy function on top of stack

  lua_pushvalue(L, 1); // Traits class
  LuviewTraits *T = (LuviewTraits*) lua_newuserdata(L, sizeof(LuviewTraits)); // priv
  lua_newtable(L); luaL_setfuncs(L, LuviewTraits_getters, 0); // getters
  lua_newtable(L); luaL_setfuncs(L, LuviewTraits_setters, 0); // setters

  lua_pushvalue(L, 1);
  lua_setmetatable(L, -4);
  lua_pcall(L, 4, 1, 0); // make_proxy(class, priv, getters, setters)

  LuviewTraits U = {
    { 0.0, 0.0, 0.0 }, // Position
    { 0.0, 0.0, 0.0 }, // Orientation
    { 0.5, 0.5, 0.5 }, // Color
    1.0,               // Scale
    1.0,               // LineWidth
    0, 1               // HasFocus, IsVisible
  } ;
  *T = U;

  lua_getfield(L, -1, "update");
  lua_pushvalue(L, -2); // Traits instance
  lua_pushvalue(L, 2);  // update table
  lua_pcall(L, 2, 0, 0);

  return 1;
}

LuviewTraits *luview_totraits(lua_State *L, int pos)
{
  luaL_getmetafield(L, pos, "priv");
  LuviewTraits *T = (LuviewTraits*) lua_touserdata(L, -1);
  lua_pop(L, 1);
  return T;
}

double *luview_tovec3(lua_State *L, int pos)
{
  luaL_getmetafield(L, pos, "priv");
  double *v = (double*) lua_touserdata(L, -1);
  lua_pop(L, 1);
  return v;
}

void luview_pushvec3(lua_State *L, double *v)
{
  lua_getglobal(L, "luview");

#include "proxy.lc" // leaves the make_proxy function on top of stack

  lua_getfield(L, -2, "Vec3"); // Vec3 class
  lua_pushlightuserdata(L, v); // priv
  lua_pushnil(L); // getters
  lua_pushnil(L); // setters

  lua_pushvalue(L, 1);
  lua_setmetatable(L, -4);
  lua_pcall(L, 4, 1, 0); // make_proxy(class, priv, getters, setters)

  lua_remove(L, -2); // luview
}
