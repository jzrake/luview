

#include "lua_object.hpp"

class Animal : public LuaCppObject
{
private:
  std::string given_name;

public:
  Animal() : given_name("noname") { }
  virtual ~Animal() { }

  virtual void speak() = 0;
  virtual void eat(int number) = 0;

  void set_name(const char *name) {
    this->given_name = name;
  }
  std::string get_name() {
    return this->given_name;
  }


protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name)
  {
    AttributeMap attr;
    attr["speak"] = _speak_;
    attr["eat"] = _eat_;
    attr["get_name"] = _get_name_;
    attr["set_name"] = _set_name_;
    RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
  }
  static int _speak_(lua_State *L)
  {
    Animal *self = checkarg<Animal>(L, 1);
    self->speak();
    return 0;
  }
  static int _eat_(lua_State *L)
  {
    Animal *self = checkarg<Animal>(L, 1);
    int n = luaL_checkinteger(L, 2);
    self->eat(n);
    return 0;
  }
  static int _get_name_(lua_State *L)
  {
    Animal *self = checkarg<Animal>(L, 1);
    lua_pushstring(L, self->get_name().c_str());
    return 1;
  }
  static int _set_name_(lua_State *L)
  {
    Animal *self = checkarg<Animal>(L, 1);
    const char *name = luaL_checkstring(L, 2);
    self->set_name(name);
    return 0;
  }
} ;


class Cat : public Animal
{
public:
  void speak()
  {
    printf("meow!\n");
  }
  void eat(int number)
  {
    printf("eating %d fishes...\n", number);
  }
  void dig()
  {
    printf("digging\n");
  }


protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name)
  {
    AttributeMap attr;
    attr["dig"] = _dig_;
    RETURN_ATTR_OR_CALL_SUPER(Animal);
  }
  static int _dig_(lua_State *L)
  {
    Cat *self = checkarg<Cat>(L, 1);
    self->dig();
    return 0;
  }

public:
  static int new_lua_obj(lua_State *L)
  {
    return make_lua_obj(L, new Cat);
  }
} ;


class Dog : public Animal
{
public:
  void speak()
  {
    printf("bark!\n");
  }
  void eat(int number)
  {
    printf("eating %d rabbits...\n", number);
  }
  static int new_lua_obj(lua_State *L)
  {
    return make_lua_obj(L, new Dog);
  }
} ;


class Poodle : public Dog
{
public:
  void speak()
  {
    printf("bark!\n");
  }
  void eat(int number)
  {
    printf("eating %d rabbits (with a cute haircut)...\n", number);
  }
  static int new_lua_obj(lua_State *L)
  {
    return make_lua_obj(L, new Poodle);
  }
} ;



class PetOwner : public LuaCppObject
{
private:
  Dog *dog;
  Cat *cat;

public:
  void set_dog(Dog *_dog)
  {
    dog = _dog;
  }
  void set_cat(Cat *_cat)
  {
    cat = _cat;
  }

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name)
  {
    AttributeMap attr;
    attr["set_cat"] = _set_cat_;
    attr["set_dog"] = _set_dog_;
    attr["get_cat"] = _get_cat_;
    attr["get_dog"] = _get_dog_;
    RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
  }
  static int _set_dog_(lua_State *L) {
    PetOwner *self = checkarg<PetOwner>(L, 1);
    self->set_dog(checkarg<Dog>(L, 2));
    return 0;
  }
  static int _set_cat_(lua_State *L) {
    PetOwner *self = checkarg<PetOwner>(L, 1);
    self->set_cat(checkarg<Cat>(L, 2));
    return 0;
  }
  static int _get_dog_(lua_State *L) {
    PetOwner *self = checkarg<PetOwner>(L, 1);
    self->push_lua_obj(L, self->dog, __REGLUA);
    return 1;
  }
  static int _get_cat_(lua_State *L) {
    PetOwner *self = checkarg<PetOwner>(L, 1);
    self->push_lua_obj(L, self->cat,  __REGLUA);
    return 1;
  }

public:
  static int new_lua_obj(lua_State *L)
  {
    return make_lua_obj(L, new PetOwner);
  }
} ;


int main()
{

  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  LuaCppObject::Init(L);

  lua_newtable(L);

  LuaCppObject::Register<Cat>(L);
  LuaCppObject::Register<Dog>(L);
  LuaCppObject::Register<Poodle>(L);
  LuaCppObject::Register<PetOwner>(L);
  lua_setglobal(L, "tests");

  if (luaL_dofile(L, "run.lua")) {
    printf("%s\n", lua_tostring(L, -1));
  }

  lua_close(L);
  return 0;
}
