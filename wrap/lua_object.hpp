

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

#define __CXX_INSTANCE_HELD_OBJECTS   "__CXX_INSTANCE_HELD_OBJECTS"
#define __CXX_INSTANCE_ATTRIB         "__CXX_INSTANCE_ATTRIB"
#define __CXX_OBJECT_LOOKUP           "__CXX_OBJECT_LOOKUP"
#define __LDEBUG 1

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
#define META_NOT_IMPLEMENTED(m) {					\
    luaL_error(__lua_state,						\
	       "no operator '%s' on %s", m, __type_name.c_str());	\
    return 0;								\
  }									\
// ---------------------------------------------------------------------------
#define META_STATIC(method) {						\
    LuaCppObject *self = checkarg<LuaCppObject>(L, 1);			\
    return self->method();						\
  }									\
// ---------------------------------------------------------------------------
#define META_STATIC_BINARY(method) {					\
    LuaCppObject *A = testarg<LuaCppObject>(L, 1);			\
    LuaCppObject *B = testarg<LuaCppObject>(L, 2);			\
    if (A) return A->method();						\
    if (B) return B->method();						\
    return 0;								\
  }									\
// ---------------------------------------------------------------------------



class LuaCppObject
{
 protected:
  typedef int (*LuaInstanceMethod)(lua_State *L);
  typedef std::map<std::string, LuaInstanceMethod> AttributeMap;

  int __refid;
  lua_State *__lua_state;
  std::string __type_name;

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
    lua_setfield(L, LUA_REGISTRYINDEX, __CXX_OBJECT_LOOKUP);
  }
  template <class T> static void Register(lua_State *L)
  // ---------------------------------------------------------------------------
  // Registers the constructor for a given class in the table at the top of the
  // stack.
  // ---------------------------------------------------------------------------
  {
    std::string m = demangle(typeid(T).name());
    lua_pushcfunction(L, __new__<T>);
    lua_setfield(L, -2, m.c_str());
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
  template <class T> static T *testarg(lua_State *L, int pos)
  // ---------------------------------------------------------------------------
  // This function first ensures that the argument at position `pos` is a valid
  // user data. If so, it tries to dynamic_cast it to the template parameter
  // `T`. This cast will fail if the object does not inherit from `T`, and the
  // method returns NULL.
  // ---------------------------------------------------------------------------
  {
    void *object_p = lua_touserdata(L, pos);
    if (object_p == NULL) return NULL;
    LuaCppObject *cpp_object = *static_cast<LuaCppObject**>(object_p);
    return dynamic_cast<T*>(cpp_object);
  }
  template <class T> static T *checkarg(lua_State *L, int pos)
  // ---------------------------------------------------------------------------
  // Convenience function, calls testarg and throws a Lua error if the result is
  // NULL.
  // ---------------------------------------------------------------------------
  {
    T *object_p = testarg<T>(L, pos);
    if (object_p == NULL) {
      std::string tn = demangle(typeid(T).name());
      luaL_error(L, "object of type '%s' at %d is not a subtype of '%s'",
		 luaL_typename(L, pos), pos, tn.c_str());
    }
    return object_p;
  }
  static int make_lua_obj(lua_State *L, LuaCppObject *object)
  // ---------------------------------------------------------------------------
  // This function is responsible for creating a Lua userdata out of a C++
  // object. It is invoked either indirectly by Lua code through object
  // constructors, or indirectly by C++ code through create. It returns 1,
  // leaving the new userdata on the stack to be picked up by Lua.
  // ---------------------------------------------------------------------------
  {
    const size_t sz = sizeof(LuaCppObject*);
    LuaCppObject **place = (LuaCppObject**) lua_newuserdata(L, sz);
    *place = object;

    static luaL_Reg meta[] =
      {{ "__add", LuaCppObject::_add},
       { "__sub", LuaCppObject::_sub},
       { "__mul", LuaCppObject::_mul},
       { "__div", LuaCppObject::_div},
       { "__pow", LuaCppObject::_pow},
       { "__mod", LuaCppObject::_mod},
       { "__unm", LuaCppObject::_unm},
       { "__concat", LuaCppObject::_concat},
       { "__len", LuaCppObject::_len},
       { "__eq", LuaCppObject::_eq},
       { "__lt", LuaCppObject::_lt},
       { "__le", LuaCppObject::_le},
       { "__index", LuaCppObject::_index},
       { "__newindex", LuaCppObject::_newindex},
       { "__call", LuaCppObject::_call},
       { "__tostring", LuaCppObject::_tostring},
       { "__gc", LuaCppObject::_gc},
       {NULL, NULL}};

    lua_newtable(L);
    luaL_setfuncs(L, meta, 0);
    lua_newtable(L); lua_setfield(L, -2, __CXX_INSTANCE_HELD_OBJECTS);
    lua_newtable(L); lua_setfield(L, -2, __CXX_INSTANCE_ATTRIB);
    lua_setmetatable(L, -2);

    // Register the object with a unique reference id for easy retrieval as a
    // Lua object.
    // -------------------------------------------------------------------------
    lua_getfield(L, LUA_REGISTRYINDEX, __CXX_OBJECT_LOOKUP);
    lua_pushvalue(L, -2);
    object->__refid = luaL_ref(L, -2);
    object->__lua_state = L;
    object->__type_name = demangle(typeid(*object).name());
    object->__init_lua_objects();
    lua_pop(L, 1);

    if (__LDEBUG) {
      printf("created object with refid %d\n", object->__refid);
    }
    return 1;
  }

public:
  // ---------------------------------------------------------------------------
  // The methods below have public access so that non-class members may create
  // objects
  // ---------------------------------------------------------------------------
  template <class T> static T *create(lua_State *L)
  {
    T *thing = new T;
    make_lua_obj(L, thing);
    lua_pop(L, 1); // make_lua_obj left `thing` on top of the stack
    return thing;
  }
  static void retrieve(lua_State *L, LuaCppObject *object)
  // ---------------------------------------------------------------------------
  // Pushes the LuaCppObject associated with `object` on the stack
  // ---------------------------------------------------------------------------
  {
    if (object == NULL) {
      lua_pushnil(L);
      return;
    }
    lua_getfield(L, LUA_REGISTRYINDEX, __CXX_OBJECT_LOOKUP);
    lua_rawgeti(L, -1, object->__refid);
    lua_remove(L, -2);
  }
protected:
  void retrieve(LuaCppObject *object)
  {
    retrieve(__lua_state, object);
  }
  void retrieve(const char *key)
  // ---------------------------------------------------------------------------
  // Pushes the pure Lua object this->held_objects[key] onto the stack
  // ---------------------------------------------------------------------------
  {
    lua_State *L = __lua_state;
    retrieve(this);
    luaL_getmetafield(L, -1, __CXX_INSTANCE_HELD_OBJECTS);
    lua_remove(L, -2); // all finished with `this`
    lua_getfield(L, -1, key);
    lua_remove(L, -2); // all finished with this->held_objects
  }
  // ---------------------------------------------------------------------------
  // Holds a C++ object, or a Lua object respectively
  // ---------------------------------------------------------------------------
  void hold(LuaCppObject *obj)
  {
    retrieve(obj);
    _hold_or_drop('h', obj->__refid, NULL);
    lua_pop(__lua_state, 1);
  }
  void hold(int pos, const char *key)
  {
    lua_pushvalue(__lua_state, pos);
    _hold_or_drop('h', LUA_NOREF, key);
    lua_pop(__lua_state, 1);
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
    if (newobj) hold(newobj);
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
    if (lua_isnil(__lua_state, pos)) {
      return NULL;
    }
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
    luaL_getmetafield(L, -1, __CXX_INSTANCE_HELD_OBJECTS);
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
  virtual void __init_lua_objects()
  // ---------------------------------------------------------------------------
  // Objects needing to instantiate their own Lua objects should do so here.
  // ---------------------------------------------------------------------------
  { }
  virtual std::string _get_type()
  // ---------------------------------------------------------------------------
  // May be over-ridden by derived classes in case a different type name is
  // desired. This function is called by the default `tostring` metamethod.
  // ---------------------------------------------------------------------------
  {
    return __type_name;
  }
  virtual int _call()
  {
    META_NOT_IMPLEMENTED("()");
  }
  virtual std::string __tostring()
  {
    std::stringstream ss;
    ss<<"<"<<this->_get_type()<<" instance at "<<this<<">";
    return ss.str();
  }
  virtual int _newindex()
  {
    lua_State *L = __lua_state;
    luaL_getmetafield(L, 1, __CXX_INSTANCE_ATTRIB);
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_settable(L, -3);
    return 0;
  }
  virtual int _index()
  {
    lua_State *L = __lua_state;
    std::string method_name = lua_tostring(L, 2);
    luaL_getmetafield(L, 1, __CXX_INSTANCE_ATTRIB);
    lua_getfield(L, -1, method_name.c_str());
    if (!lua_isnil(L, -1)) {
      lua_remove(L, -2);
      return 1;
    }
    else {
      lua_pop(L, 2);
    }
    LuaInstanceMethod m = __getattr__(method_name);
    if (m == NULL) return 0;
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
    lua_pushstring(L, self->__type_name.c_str());
    return 1;
  }

  // =====================
  // M E T A M E T H O D S
  // =====================
  virtual int __add() { META_NOT_IMPLEMENTED("+"); }
  virtual int __sub() { META_NOT_IMPLEMENTED("-"); }
  virtual int __mul() { META_NOT_IMPLEMENTED("*"); }
  virtual int __div() { META_NOT_IMPLEMENTED("/"); }
  virtual int __pow() { META_NOT_IMPLEMENTED("^"); }
  virtual int __mod() { META_NOT_IMPLEMENTED("%"); }
  virtual int __unm() { META_NOT_IMPLEMENTED("-"); }
  virtual int __concat() { META_NOT_IMPLEMENTED(".."); }
  virtual int __len() { META_NOT_IMPLEMENTED("#"); }
  virtual int __eq() { META_NOT_IMPLEMENTED("=="); }
  virtual int __lt() { META_NOT_IMPLEMENTED("<"); }
  virtual int __le() { META_NOT_IMPLEMENTED("<="); }

  static int _add(lua_State *L) { META_STATIC_BINARY(__add); }
  static int _sub(lua_State *L) { META_STATIC_BINARY(__sub); }
  static int _mul(lua_State *L) { META_STATIC_BINARY(__mul); }
  static int _div(lua_State *L) { META_STATIC_BINARY(__div); }
  static int _pow(lua_State *L) { META_STATIC_BINARY(__pow); }
  static int _mod(lua_State *L) { META_STATIC_BINARY(__mod); }
  static int _unm(lua_State *L) { META_STATIC_BINARY(__unm); }
  static int _concat(lua_State *L) { META_STATIC_BINARY(__concat); }
  static int _len(lua_State *L) { META_STATIC(__len); }
  static int _eq(lua_State *L) { META_STATIC_BINARY(__eq); }
  static int _lt(lua_State *L) { META_STATIC_BINARY(__lt); }
  static int _le(lua_State *L) { META_STATIC_BINARY(__le); }
  static int _newindex(lua_State *L)
  {
    LuaCppObject *self = checkarg<LuaCppObject>(L, 1);
    return self->_newindex();
  }
  static int _index(lua_State *L)
  {
    LuaCppObject *self = checkarg<LuaCppObject>(L, 1);
    return self->_index();
  }
  static int _call(lua_State *L)
  {
    LuaCppObject *self = checkarg<LuaCppObject>(L, 1);
    lua_remove(L, 1); // leave all remaining arguments on the stack
    return self->_call();
  }
  static int _tostring(lua_State *L)
  {
    LuaCppObject *object = *((LuaCppObject**) lua_touserdata(L, 1));
    std::string str = object->__tostring();
    lua_pushstring(L, str.c_str());
    return 1;
  }
  static int _gc(lua_State *L)
  // ---------------------------------------------------------------------------
  // Arguments:
  //
  // (1) object: a user data pointing to a LuaCppObject
  //
  // Returns: nothing
  // ---------------------------------------------------------------------------
  {
    LuaCppObject *object = checkarg<LuaCppObject>(L, 1);

    lua_getfield(L, LUA_REGISTRYINDEX, __CXX_OBJECT_LOOKUP);
    luaL_unref(L, -1, object->__refid);
    lua_pop(L, 1);

    if (__LDEBUG) {
      printf("killing object with refid %d...\n", object->__refid);
    }

    delete object;
    return 0;
  }
} ;

#endif // __LuaCppObject_Hpp__
