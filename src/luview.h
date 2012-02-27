


#ifndef __LuviewHeader_HEADER__
#define __LuviewHeader_HEADER__

#include "lualib.h"
#include "lauxlib.h"

struct LuviewTraits
{
  double Position[3];
  double Orientation[3];
  double Color[3];
  double Scale;
  double LineWidth;
  int HasFocus, IsVisible;
} ;

typedef struct LuviewTraits LuviewTraits;

LuviewTraits *luview_totraits(lua_State *L, int pos);
double       *luview_tovec3(lua_State *L, int pos);
void          luview_pushvec3(lua_State *L, double *v);

int lv_register_types(lua_State *L);



#define GETSET1(type, prop)						\
  static int (set_##type##_##prop(lua_State *L))			\
  {                                                                     \
    luaL_getmetafield(L, 1, "priv");                                    \
    type *T = (type*) lua_touserdata(L, -1);				\
    lua_pop(L, 1);                                                      \
    T->prop = lua_tonumber(L, 2);                                       \
    return 0;                                                           \
  }                                                                     \
  static int get_##type##_##prop(lua_State *L)				\
  {                                                                     \
    luaL_getmetafield(L, 1, "priv");                                    \
    type *T = (type*) lua_touserdata(L, -1);				\
    lua_pop(L, 1);                                                      \
    lua_pushnumber(L, T->prop);                                         \
    return 1;                                                           \
  }                                                                     \

#define GETSETBOOL(type, prop)						\
  static int (set_##type##_##prop(lua_State *L))			\
  {                                                                     \
    luaL_getmetafield(L, 1, "priv");                                    \
    type *T = (type*) lua_touserdata(L, -1);				\
    lua_pop(L, 1);                                                      \
    T->prop = lua_toboolean(L, 2);					\
    return 0;                                                           \
  }                                                                     \
  static int get_##type##_##prop(lua_State *L)				\
  {                                                                     \
    luaL_getmetafield(L, 1, "priv");                                    \
    type *T = (type*) lua_touserdata(L, -1);				\
    lua_pop(L, 1);                                                      \
    lua_pushboolean(L, T->prop);					\
    return 1;                                                           \
  }                                                                     \

#define GETSET3(type, prop)						\
  static int set_##type##_##prop(lua_State *L)				\
  {                                                                     \
    luaL_getmetafield(L, 1, "priv");                                    \
    type *T = (type*) lua_touserdata(L, -1);				\
    lua_pop(L, 1);                                                      \
    double *v = T->prop;                                                \
    lua_rawgeti(L, 2, 1); v[0] = lua_tonumber(L, -1); lua_pop(L, 1);    \
    lua_rawgeti(L, 2, 2); v[1] = lua_tonumber(L, -1); lua_pop(L, 1);    \
    lua_rawgeti(L, 2, 3); v[2] = lua_tonumber(L, -1); lua_pop(L, 1);    \
    return 0;                                                           \
  }                                                                     \
  static int get_##type##_##prop(lua_State *L)				\
  {                                                                     \
    luaL_getmetafield(L, 1, "priv");                                    \
    type *T = (type*) lua_touserdata(L, -1);				\
    lua_pop(L, 1);                                                      \
    luview_pushvec3(L, T->prop);					\
    return 1;                                                           \
  }                                                                     \



#endif // __LuviewHeader_HEADER__
