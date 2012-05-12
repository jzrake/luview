

#include "lua_object.hpp"


class PetOwner;

class Animal : public LuaCppObject
{
private:
  std::string given_name;
  PetOwner *owner;

public:
  Animal() : given_name("noname"), owner(NULL) { }
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
    attr["set_owner"] = _set_owner_;
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
  static int _set_owner_(lua_State *L);
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
} ;



class PetOwner : public LuaCppObject
{
private:
  Dog *dog;
  Cat *cat;

public:
  PetOwner() : dog(NULL), cat(NULL) { }
  void set_dog(Dog *_dog)
  {
    if (dog) drop(dog);
    hold(dog = _dog);
  }
  void set_cat(Cat *_cat)
  {
    if (cat) drop(cat);
    hold(cat = _cat);
  }
  void auto_cat()
  {
    cat = create_and_hold<Cat>();
    cat->set_name("cleo-kitty");
  }

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name)
  {
    AttributeMap attr;
    attr["set_cat"] = _set_cat_;
    attr["set_dog"] = _set_dog_;
    attr["get_cat"] = _get_cat_;
    attr["get_dog"] = _get_dog_;
    attr["auto_cat"] = _auto_cat_;
    RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
  }
  static int _set_dog_(lua_State *L) {
    PetOwner *self = checkarg<PetOwner>(L, 1);
    Dog *dog = checkarg<Dog>(L, 2);
    self->set_dog(dog);
    return 0;
  }
  static int _set_cat_(lua_State *L) {
    PetOwner *self = checkarg<PetOwner>(L, 1);
    Cat *cat = checkarg<Cat>(L, 2);
    self->set_cat(cat);
    return 0;
  }
  static int _get_dog_(lua_State *L) {
    PetOwner *self = checkarg<PetOwner>(L, 1);
    self->push_lua_obj(L, self->dog);
    return 1;
  }
  static int _get_cat_(lua_State *L) {
    PetOwner *self = checkarg<PetOwner>(L, 1);
    self->push_lua_obj(L, self->cat);
    return 1;
  }
  static int _auto_cat_(lua_State *L) {
    PetOwner *self = checkarg<PetOwner>(L, 1);
    self->auto_cat();
    return 0;
  }
} ;


int Animal::_set_owner_(lua_State *L)
{
  Animal *self = checkarg<Animal>(L, 1);
  PetOwner *owner = checkarg<PetOwner>(L, 2);
  if (self->owner) self->drop(self->owner);
  self->hold(self->owner = owner);
  return 0;
}


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
