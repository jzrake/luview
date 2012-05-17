

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

#define __LDEBUG 0
#define __REGLUA "LuaCppObject"

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

 public:

  // =======================================
  // P U B L I C   C L A S S   M E T H O D S
  // =======================================

  LuaCppObject() : __refid(LUA_NOREF),
		   __lua_state(NULL) { }
  virtual ~LuaCppObject() { }

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
  }
  template <class T> static void Register(lua_State *L)
  // ---------------------------------------------------------------------------
  // Registers the constructor for a given class in the table at the top of the
  // stack.
  // ---------------------------------------------------------------------------
  {
    lua_pushcfunction(L, __new__<T>);
    lua_setfield(L, -2, demangle(typeid(T).name()).c_str());
  }

protected:

  // =================================
  // U T I L I T Y   F U N C T I O N S
  // =================================
  template <class T> static int __new__(lua_State *L)
  // ---------------------------------------------------------------------------
  // This is the constructor which gets called from Lua code.
  // ---------------------------------------------------------------------------
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
                 cpp_object->_get_type().c_str(),
		 demangle(typeid(T).name()).c_str());
    }
    return result;
  }
  static int make_lua_obj(lua_State *L, LuaCppObject *object)
  // ---------------------------------------------------------------------------
  // This function is responsible for creating a Lua userdata out of a C++
  // object. It is invoked either indirectly by Lua code through object
  // constructors, or indirectly by C++ code through create. It returns 1,
  // leaving the new userdata on the stack to be picked up by Lua.
  // ---------------------------------------------------------------------------
  {
    LuaCppObject **place = (LuaCppObject**)
      lua_newuserdata(L, sizeof(LuaCppObject*));
    *place = object;

    lua_newtable(L);

    lua_pushcfunction(L, LuaCppObject::__call);
    lua_setfield(L, -2, "__call");
    lua_pushcfunction(L, LuaCppObject::__index);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, LuaCppObject::__newindex);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, LuaCppObject::__tostring);
    lua_setfield(L, -2, "__tostring");
    lua_pushcfunction(L, LuaCppObject::__gc);
    lua_setfield(L, -2, "__gc");
    lua_newtable(L);
    lua_setfield(L, -2, "held_objects");
    lua_newtable(L);
    lua_setfield(L, -2, "lua_attributes");

    lua_setmetatable(L, -2);

    // Register the object with a unique reference id for easy retrieval as a
    // Lua object.
    // -------------------------------------------------------------------------
    lua_getglobal(L, __REGLUA);
    lua_pushvalue(L, -2);
    object->__refid = luaL_ref(L, -2);
    object->__lua_state = L;
    lua_pop(L, 1);

    if (__LDEBUG) {
      printf("created object with refid %d\n", object->__refid);
    }

    return 1;
  }
  static int make_cpp_handle(lua_State *L, int pos)
  // ---------------------------------------------------------------------------
  // This function is responsible for creating a C++ object out of the Lua
  // object at stack index `pos`. It is only invoked by C++ code indirectly
  // through LuaCppObject::LuaCppObject(lua_State *L, int pos). Its main purpose
  // is to facilitate the creation of C++ callback functions which invoke Lua
  // functions, but it could also be used, for example, to wrap a Lua table with
  // a C++ class. The function returns a refid (key into __REGLUA) whose value
  // is the Lua object at stack index `pos`.
  // ---------------------------------------------------------------------------
  {
    pos = lua_absindex(L, pos);
    lua_getglobal(L, __REGLUA);
    lua_pushvalue(L, pos);

    int refid = luaL_ref(L, -2);
    lua_pop(L, 2);

    if (__LDEBUG) {
      printf("created lua C++ handle with refid %d\n", refid);
    }
    return refid;
  }
  template <class T> static T *create(lua_State *L)
  {
    T *thing = new T;
    make_lua_obj(L, thing);
    lua_pop(L, 1); // make_lua_obj left `thing` on top of the stack
    return thing;
  }

protected:
  void retrieve(LuaCppObject *object)
  // ---------------------------------------------------------------------------
  // Pushes the LuaCppObject associated with `object` on the stack
  // ---------------------------------------------------------------------------
  {
    lua_State *L = __lua_state;
    if (object == NULL) {
      lua_pushnil(L);
      return;
    }
    lua_getglobal(L, __REGLUA);
    lua_rawgeti(L, -1, object->__refid);
    lua_remove(L, -2);
  }
  void retrieve(const char *key)
  // ---------------------------------------------------------------------------
  // Pushes the pure Lua object this->held_objects[key] onto the stack
  // ---------------------------------------------------------------------------
  {
    lua_State *L = __lua_state;
    retrieve(this);
    luaL_getmetafield(L, -1, "held_objects");
    lua_remove(L, -2); // done with `this`
    lua_getfield(L, -1, key);
    lua_remove(L, -2); // done with this->held_objects
  }
  // ---------------------------------------------------------------------------
  // Holds a C++ object, or a Lua object respectively.
  // ---------------------------------------------------------------------------
  void hold(LuaCppObject *obj)
  {
    retrieve(obj);
    _hold_or_drop('h', obj->__refid, NULL);
  }
  void hold(int pos, const char *key)
  {
    lua_pushvalue(__lua_state, pos);
    _hold_or_drop('h', LUA_NOREF, key);
  }
  // ---------------------------------------------------------------------------
  // Drops a C++ object, or a Lua object respectively
  // ---------------------------------------------------------------------------
  void drop(LuaCppObject *obj)
  {
    _hold_or_drop('d', obj->__refid, NULL);
  }
  void drop(const char *key)
  {
    _hold_or_drop('d', LUA_NOREF, key);
  }
  template <class T> T *replace(T *oldobj, T *newobj)
  // ---------------------------------------------------------------------------
  // Convenience function: assumes that if `oldobj` is not NULL, then it
  // previously belongs to `this`, and is be dropped and replaced by `newobj`.
  // ---------------------------------------------------------------------------
  {
    if (oldobj != NULL) drop(oldobj);
    hold(newobj);
    return newobj;
  }
  template <class T> T *replace(T *oldobj, int pos)
  // ---------------------------------------------------------------------------
  // Convenience function: assumes that if `oldobj` is not NULL, then it
  // previously belongs to `this`, and is be dropped and replaced by the object
  // at stack index `pos`.
  // ---------------------------------------------------------------------------
  {
    pos = lua_absindex(__lua_state, pos);
    if (oldobj != NULL) drop(oldobj);
    T *newobj = checkarg<T>(__lua_state, pos);
    hold(newobj);
    return newobj;
  }
private:
  void _hold_or_drop(char op, int refid, const char *key)
  {
    lua_State *L = __lua_state;
    const int pos = lua_absindex(L, -1);
    retrieve(this);
    luaL_getmetafield(L, -1, "held_objects");
    lua_remove(L, -2); // removes `this`
    if (key == NULL) {
      lua_pushnumber(L, refid); // key is obj's refid (for LuaCppObject's)
    }
    else {
      lua_pushstring(L, key); // key is the string `key` (for pure Lua objects)
    }
    if (op == 'h') { // hold the Lua (or C++) object at stack index `pos`
      lua_pushvalue(L, pos);
    }
    else if (op == 'd') {
      lua_pushnil(L); // held_objects[obj->__refid] = nil
    }
    lua_settable(L, -3); // pops the key and value
    lua_pop(L, 1); // removes this->metatable->held_objects, back to normal
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
  virtual std::string _get_type()
  // ---------------------------------------------------------------------------
  // May be over-ridden by derived classes in case a different type name is
  // desired. This function is called by the default `tostring` metamethod.
  // ---------------------------------------------------------------------------
  {
    return demangle(typeid(*this).name());
  }
  virtual int _call()
  {
    luaL_error(__lua_state, "object does not respond to function calls");
    return 0;
  }
  virtual std::string _tostring()
  {
    std::stringstream ss;
    ss<<"<"<<this->_get_type()<<" instance at "<<this<<">";
    return ss.str();
  }
  virtual int _newindex()
  {
    lua_State *L = __lua_state;
    luaL_getmetafield(L, 1, "lua_attributes");
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_settable(L, -3);

    return 0;
  }
  virtual int _index()
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
    lua_State *L = __lua_state;
    LuaCppObject *object = *static_cast<LuaCppObject**>(lua_touserdata(L, 1));
    std::string method_name = lua_tostring(L, 2);

    luaL_getmetafield(L, 1, "lua_attributes");
    lua_getfield(L, -1, method_name.c_str());

    if (!lua_isnil(L, -1)) {
      lua_remove(L, -2);
      return 1;
    }
    else {
      lua_pop(L, 2);
    }

    LuaInstanceMethod m = object->__getattr__(method_name);

    if (m == NULL) {
      luaL_error(L, "'%s' has no attribute '%s'", object->_get_type().c_str(),
                 method_name.c_str());
    }

    lua_pushcfunction(L, m);
    return 1;
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
    lua_pushstring(L, self->_get_type().c_str());
    return 1;
  }

  // =====================
  // M E T A M E T H O D S
  // =====================

  static int __call(lua_State *L)
  {
    LuaCppObject *self = *static_cast<LuaCppObject**>(lua_touserdata(L, 1));
    lua_remove(L, 1); // leave all remain arguments on the stack
    return self->_call();
  }
  static int __newindex(lua_State *L)
  {
    LuaCppObject *self = *static_cast<LuaCppObject**>(lua_touserdata(L, 1));
    return self->_newindex();
  }
  static int __index(lua_State *L)
  {
    LuaCppObject *self = *static_cast<LuaCppObject**>(lua_touserdata(L, 1));
    return self->_index();
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

    lua_getglobal(L, __REGLUA);
    luaL_unref(L, -1, object->__refid);
    lua_pop(L, 1);

    if (__LDEBUG) {
      printf("killing object with refid %d...\n", object->__refid);
    }

    delete object;
    return 0;
  }
  static int __tostring(lua_State *L)
  {
    LuaCppObject *object = *((LuaCppObject**) lua_touserdata(L, 1));
    lua_pushstring(L, object->_tostring().c_str());
    return 1;
  }
} ;

#endif // __LuaCppObject_Hpp__
