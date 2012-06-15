

#include <vector>
#include "lua_object.hpp"


class CallbackFunction : public LuaCppObject
{
public:
  std::vector<double> call();
  std::vector<double> call(double u);
  std::vector<double> call(double u, double v);
  std::vector<double> call(double u, double v, double w);
  std::vector<double> call(std::vector<double> X);
  static CallbackFunction *CallbackFunction::create_from_stack(lua_State *L, int pos);
private:
  virtual std::vector<double> call_priv(double *x, int narg) = 0;
} ;

class LuaFunction : public CallbackFunction
{
private:
  virtual std::vector<double> call_priv(double *x, int narg);
} ;

CallbackFunction *CallbackFunction::create_from_stack(lua_State *L, int pos)
{
  if (lua_type(L, pos) == LUA_TUSERDATA) {
    return checkarg<CallbackFunction>(L, pos);
  }
  else {
    LuaFunction *f = create<LuaFunction>(L);
    f->hold(2, "lua_callback");
    return f;
  }
}

class CppFunction : public CallbackFunction
{
private:
  virtual std::vector<double> call_priv(double *x, int narg)
  {
    printf("calling the CppFunction!\n");
    return std::vector<double>();
  }
} ;

class PetOwner;
class Animal : public LuaCppObject
{
private:
  std::string given_name;
  PetOwner *owner;
  CallbackFunction *play_func;

public:
  Animal() : given_name("noname"), owner(NULL), play_func(NULL) { }
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
    attr["teach_play"] = _teach_play_;
    attr["play"] = _play_;
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
  static int _teach_play_(lua_State *L);
  static int _play_(lua_State *L);
} ;


class Cat : public Animal
{
public:
  virtual ~Cat() { }
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
  virtual ~Dog() { }
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
  virtual ~Poodle() { }
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
  virtual ~PetOwner() { }
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
    cat = create<Cat>(__lua_state);
    hold(cat);
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
    self->retrieve(self->dog);
    return 1;
  }
  static int _get_cat_(lua_State *L) {
    PetOwner *self = checkarg<PetOwner>(L, 1);
    self->retrieve(self->cat);
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

int Animal::_teach_play_(lua_State *L)
{
  Animal *self = checkarg<Animal>(L, 1);
  if (self->play_func != NULL) self->drop(self->play_func);
  self->hold(self->play_func = CallbackFunction::create_from_stack(L, 2));
  return 0;
}

int Animal::_play_(lua_State *L)
{
  Animal *self = checkarg<Animal>(L, 1);
  if (self->play_func == NULL) {
    printf("don't know how ;(\n");
  }
  else {
    self->play_func->call();
  }
  return 0;
}




std::vector<double> CallbackFunction::call()
{
  return call_priv(NULL, 0);
}
std::vector<double> CallbackFunction::call(double u)
{
  double x[1] = {u};
  return call_priv(x, 1);
}
std::vector<double> CallbackFunction::call(double u, double v)
{
  double x[2] = {u,v};
  return call_priv(x, 2);
}
std::vector<double> CallbackFunction::call(double u, double v, double w)
{
  double x[3] = {u,v,w};
  return call_priv(x, 3);
}
std::vector<double> CallbackFunction::call(std::vector<double> X)
{
  return call_priv(&X[0], X.size());
}

std::vector<double> LuaFunction::call_priv(double *x, int narg)
{
  lua_State *L = __lua_state;
  std::vector<double> res;
  retrieve("lua_callback");
  for (int i=0; i<narg; ++i) {
    lua_pushnumber(L, x[i]);
  }
  if (lua_pcall(L, narg, LUA_MULTRET, 0) != 0) {
    luaL_error(L, lua_tostring(L, -1));
  }
  int nret = lua_gettop(L) - 2;
  for (int i=0; i<nret; ++i) {
    res.push_back(lua_tonumber(L, -1));
    lua_pop(L, 1);
  }
  reverse(res.begin(), res.end());
  return res;
}

#include <complex>
class LuaComplexDouble : public LuaCppObject
{
public:
  std::complex<double> z;
  LuaComplexDouble() : z(0,0) { }
protected:
  virtual int __add() {
    lua_State *L = __lua_state;
    LuaComplexDouble *a, *b;

    if (lua_isnumber(L, 1)) {
      a = create<LuaComplexDouble>(L);
      a->z = lua_tonumber(L, 1);
    }
    else {
      a = checkarg<LuaComplexDouble>(L, 1);
    }
    if (lua_isnumber(L, 2)) {
      b = create<LuaComplexDouble>(L);
      b->z = lua_tonumber(L, 2);
    }
    else {
      b = checkarg<LuaComplexDouble>(L, 2);
    }

    LuaComplexDouble *ret = create<LuaComplexDouble>(L);
    ret->z = a->z + b->z;
    retrieve(L, ret);

    return 1;
  }
  virtual std::string __tostring()
  {
    std::stringstream ss;
    ss<<this->z;
    return ss.str();
  }
  virtual LuaInstanceMethod __getattr__(std::string &method_name)
  {
    AttributeMap attr;
    attr["conj"] = _conj_;
    attr["norm"] = _norm_;
    RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
  }
  static int _conj_(lua_State *L) {
    LuaComplexDouble *self = checkarg<LuaComplexDouble>(L, 1);
    LuaComplexDouble *ret = create<LuaComplexDouble>(L);
    ret->z = conj(self->z);
    self->retrieve(ret);
    return 1;
  }
  static int _norm_(lua_State *L) {
    LuaComplexDouble *self = checkarg<LuaComplexDouble>(L, 1);
    LuaComplexDouble *ret = create<LuaComplexDouble>(L);
    ret->z = norm(self->z);
    self->retrieve(ret);
    return 0;
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
  LuaCppObject::Register<CppFunction>(L);

  LuaComplexDouble *J = LuaCppObject::create<LuaComplexDouble>(L);
  J->z = std::complex<double>(0,1);
  LuaCppObject::retrieve(L, J);
  lua_setfield(L, -2, "j");

  lua_setglobal(L, "tests");

  if (luaL_dofile(L, "run.lua")) {
    printf("%s\n", lua_tostring(L, -1));
  }

  lua_close(L);
  return 0;
}
