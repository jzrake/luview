

#ifndef __LuaCppObject_Hpp__
#define __LuaCppObject_Hpp__

#include <iostream>
#include <string>
#include <sstream>
#include <map>

#ifdef __GNUC__
#include <cstdlib>
#include <cxxabi.h>
#endif // __GNUC__


extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#define __REGCXX "LuaCppObject_cxx"
#define __REGLUA "LuaCppObject_lua"

// ---------------------------------------------------------------------------
#define STACKDUMP {                                                     \
    for (int i=1; i<=lua_gettop(L); ++i) {                              \
      printf("%d: %s %s\n",i,luaL_typename(L,i),lua_tostring(L,i));     \
    }                                                                   \
  }                                                                     \
// ---------------------------------------------------------------------------
#define RETURN_ATTR_OR_CALL_SUPER(S) {                                  \
    AttributeMap::iterator m = attr.find(method_name);                  \
    return m == attr.end() ? S::__getattr__(method_name) : m->second;   \
  }                                                                     \
// ---------------------------------------------------------------------------


class LuaCppObject
{
 protected:
  typedef int (*LuaInstanceMethod)(lua_State *L);
  typedef std::map<std::string, LuaInstanceMethod> AttributeMap;

  // Used as the key into the symbol table (with weak values) of registered
  // objects at global key `LuaCppObject`.
  int __refid;
  lua_State *__lua_state;
  bool __is_cxx_only;

 public:

  // =======================================
  // P U B L I C   C L A S S   M E T H O D S
  // =======================================

  LuaCppObject() : __refid(LUA_NOREF),
		   __lua_state(NULL),
		   __is_cxx_only(false) { }

  // ---------------------------------------------------------------------------
  // This constructor is intended for C++ only methods, i.e. one which was
  // created and will be deleted by C++ code. These objects are never returned
  // to Lua, and are thus never entered as a user data or receive a metatable.
  // ---------------------------------------------------------------------------
  LuaCppObject(lua_State *L, int pos) : __refid(make_refid(L, pos, __REGCXX)),
					__lua_state(L),
					__is_cxx_only(true) { }

  virtual ~LuaCppObject()
  {
    if (__is_cxx_only) {
      unmake_refid(__lua_state, __refid, __REGCXX);
    }
  }

  static void Init(lua_State *L)
  // ---------------------------------------------------------------------------
  // Set up the global table for LuaCppObject to have weak values, so that its
  // entries are garbage collected. Call this once for each Lua instance.
  // ---------------------------------------------------------------------------
  {
    lua_newtable(L);
    lua_newtable(L);
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2);
    lua_setglobal(L, __REGLUA);

    lua_newtable(L);
    lua_setglobal(L, __REGCXX);
  }
  template <class T> static void Register(lua_State *L)
  // ---------------------------------------------------------------------------
  // Registers the constructor for a given class in the table at the top of the
  // stack.
  // ---------------------------------------------------------------------------
  {
    lua_pushcfunction(L, newobj<T>);
    lua_setfield(L, -2, demangle(typeid(T).name()).c_str());
  }
  template <class T> static void RegisterWithConstructor(lua_State *L)
  {
    lua_pushcfunction(L, T::__new__);
    lua_setfield(L, -2, demangle(typeid(T).name()).c_str());
  }

protected:

  // =================================
  // U T I L I T Y   F U N C T I O N S
  // =================================
  template <class T> static int newobj(lua_State *L)
  {
    return make_lua_obj(L, new T);
  }
  template <class T> static T *checkarg(lua_State *L, int pos)
  // ---------------------------------------------------------------------------
  // This function first ensures that the argument at position `pos` is a valid
  // user data. If so, it tries to dynamic_cast it to the template parameter
  // `T`. This cast will fail if the object does not inherit from `T`, causing a
  // graceful Lua error.
  // ---------------------------------------------------------------------------
  {
    void *object_p = lua_touserdata(L, pos);
    if (object_p == NULL) {
      luaL_error(L, "invalid type");
    }

    LuaCppObject *cpp_object = *static_cast<LuaCppObject**>(object_p);
    T *result = dynamic_cast<T*>(cpp_object);

    if (result == NULL) {
      luaL_error(L, "object of type '%s' is not a subtype of '%s'",
                 cpp_object->get_type().c_str(),
                 demangle(typeid(T).name()).c_str());
    }
    return result;
  }
  static int make_refid(lua_State *L, int pos, const char *reg)
  {
    pos = lua_absindex(L, pos);

    lua_getglobal(L, reg);
    lua_pushvalue(L, pos);
    int refid = luaL_ref(L, -2);
    lua_pop(L, 1);
    lua_remove(L, -2); // registry table
    return refid;
  }
  static void unmake_refid(lua_State *L, int refid, const char *reg)
  {
    lua_getglobal(L, reg);
    luaL_unref(L, -1, refid);
    lua_pop(L, 1);
  }
  static void push_lua_refid(lua_State *L, int refid, const char *reg)
  {
    lua_getglobal(L, reg);
    lua_rawgeti(L, -1, refid);
    lua_remove(L, -2);
  }
  static void push_lua_obj(lua_State *L, LuaCppObject *object, const char *reg)
  {
    push_lua_refid(L, object->__refid, reg);
  }
  static int make_lua_obj(lua_State *L, LuaCppObject *object)
  {
    LuaCppObject **place = (LuaCppObject**)
      lua_newuserdata(L, sizeof(LuaCppObject*));
    *place = object;

    lua_newtable(L);

    lua_pushcfunction(L, LuaCppObject::__index);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, LuaCppObject::__tostring);
    lua_setfield(L, -2, "__tostring");
    lua_pushcfunction(L, LuaCppObject::__gc);
    lua_setfield(L, -2, "__gc");

    lua_setmetatable(L, -2);

    // Register the object with a unique reference id for easy retrieval as a
    // Lua object.
    // -------------------------------------------------------------------------
    lua_getglobal(L, __REGLUA);
    lua_pushvalue(L, -2);
    object->__refid = luaL_ref(L, -2);
    object->__lua_state = L;
    lua_pop(L, 1);

    return 1;
  }

#ifdef __GNUC__
  // ---------------------------------------------------------------------------
  // Demangling names on gcc works like this:
  // ---------------------------------------------------------------------------
  static std::string demangle(const char *mname)
  {
    static int status;
    char *realname = abi::__cxa_demangle(mname, 0, 0, &status);
    std::string ret = realname;
    free(realname);
    return ret;
  }
#else
  static std::string demangle(const char *mname)
  {
    return std::string(mname);
  }
#endif // __GNUC__


 protected:
  virtual std::string get_type()
  // ---------------------------------------------------------------------------
  // May be over-ridden by derived classes in case a different type name is
  // desired. This function is called by the default `tostring` metamethod.
  // ---------------------------------------------------------------------------
  {
    return demangle(typeid(*this).name());
  }
  virtual std::string tostring()
  {
    std::stringstream ss;
    ss<<"<"<<this->get_type()<<" instance at "<<this<<">";
    return ss.str();
  }


  // ===============================
  // I N S T A N C E   M E T H O D S
  // ===============================

  virtual LuaInstanceMethod __getattr__(std::string &method_name)
  // ---------------------------------------------------------------------------
  // The attributes below are inherited by all LuaCppObject's. If an attribute
  // does not belong to a particular class instance, the super is invoked until
  // we reach this function, at which point NULL is returned.
  // ---------------------------------------------------------------------------
  {
    AttributeMap attr;
    attr["get_refid"] = _get_refid_;
    attr["get_type"] = _get_type_;
    AttributeMap::iterator m = attr.find(method_name);
    return m == attr.end() ? NULL : m->second;
  }
  static int _get_refid_(lua_State *L)
  {
    LuaCppObject *self = checkarg<LuaCppObject>(L, 1);
    lua_pushnumber(L, self->__refid);
    return 1;
  }
  static int _get_type_(lua_State *L)
  {
    LuaCppObject *self = checkarg<LuaCppObject>(L, 1);
    lua_pushstring(L, self->get_type().c_str());
    return 1;
  }

  // =====================
  // M E T A M E T H O D S
  // =====================

  static int __index(lua_State *L)
  // ---------------------------------------------------------------------------
  // Arguments:
  //
  // (1) object: a user data pointing to a LuaCppObject
  // (2) method_name: a string
  //
  // Returns: a static c-function which wraps the instance method
  //
  // ---------------------------------------------------------------------------
  {
    LuaCppObject *object = *static_cast<LuaCppObject**>(lua_touserdata(L, 1));
    std::string method_name = lua_tostring(L, 2);

    LuaInstanceMethod m = object->__getattr__(method_name);

    if (m == NULL) {
      luaL_error(L, "'%s' has no attribute '%s'", object->get_type().c_str(),
                 method_name.c_str());
    }

    lua_pushcfunction(L, m);
    return 1;
  }

  static int __gc(lua_State *L)
  // ---------------------------------------------------------------------------
  // Arguments:
  //
  // (1) object: a user data pointing to a LuaCppObject
  //
  // Returns: nothing
  // ---------------------------------------------------------------------------
  {
    LuaCppObject *object = *static_cast<LuaCppObject**>(lua_touserdata(L, 1));
    unmake_refid(L, object->__refid, __REGLUA);

    //    printf("killing object with refid %d...\n", object->__refid);

    delete object;
    return 0;
  }
  static int __tostring(lua_State *L)
  {
    LuaCppObject *object = *((LuaCppObject**) lua_touserdata(L, 1));
    lua_pushstring(L, object->tostring().c_str());
    return 1;
  }
} ;

#endif __LuaCppObject_Hpp__
