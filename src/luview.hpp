
#ifndef __LuviewObjects_HEADER__
#define __LuviewObjects_HEADER__


#include <vector>
#include "lua_object.hpp"

extern "C" {
#include "GL/glfw.h"
}


class CallbackFunction : public LuaCppObject
{
public:
  CallbackFunction(lua_State *L, int pos);
  std::vector<double> call(double u);
  std::vector<double> call(double u, double v);
  std::vector<double> call(double u, double v, double w);
  std::vector<double> call_n(std::vector<double> X, LuaCppObject *caller=NULL);
  std::vector<double> call_n(double *x, int narg, LuaCppObject *caller=NULL);
} ;


class DataSource : public LuaCppObject
{
protected:
  DataSource *input;
  GLfloat *output;
  CallbackFunction *transform;

public:
  DataSource();
  ~DataSource();
  virtual GLfloat *get_data() = 0;
  virtual int get_num_points(int d) = 0;
  virtual int get_size() = 0;
  virtual int get_num_components() = 0;
  virtual int get_num_dimensions() = 0;

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _get_transform_(lua_State *L);
  static int _set_transform_(lua_State *L);
  static int _get_input_(lua_State *L);
  static int _set_input_(lua_State *L);
} ;


class FunctionMapping : public DataSource
{
private:
  std::map<std::string, double> info;

public:
  int get_num_points(int d);
  int get_size();
  int get_num_components();
  int get_num_dimensions();
  GLfloat *get_data();

private:
  std::string K(const char *s, int d);

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _get_info_(lua_State *L);
} ;


class PointsSource :  public DataSource
{
private:
  GLfloat *output;
  int Np, Nc;

public:
  PointsSource();
  ~PointsSource();

  GLfloat *get_data();

  int get_num_points(int d);
  int get_size();
  int get_num_components();
  int get_num_dimensions();

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _set_points_(lua_State *L);
} ;




// ---------------------------------------------------------------------------
#define GETSET_TRAITS_D1(prop)						\
  static int _get_##prop##_(lua_State *L) {				\
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);    \
    lua_remove(L, 1);                                                   \
    return __get_vec__(L, &self->prop, 1);                              \
  }                                                                     \
  static int _set_##prop##_(lua_State *L) {				\
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);    \
    lua_remove(L, 1);                                                   \
    return __set_vec__(L, &self->prop, 1);                              \
  }                                                                     \
  // ---------------------------------------------------------------------------
#define GETSET_TRAITS_D3(prop)						\
  static int _get_##prop##_(lua_State *L) {				\
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);    \
    lua_remove(L, 1);                                                   \
    return __get_vec__(L, self->prop, 3);                               \
  }                                                                     \
  static int _set_##prop##_(lua_State *L) {				\
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);    \
    lua_remove(L, 1);                                                   \
    return __set_vec__(L, self->prop, 3);                               \
  }                                                                     \
  // ---------------------------------------------------------------------------

class LuviewTraitedObject : public LuaCppObject
{
protected:
  double Position[3];
  double Orientation[3];
  double Color[3];
  double Scale[3];
  double LineWidth;
  double Alpha;
  std::map<std::string, CallbackFunction*> Callbacks;
  std::map<std::string, DataSource*> DataSources;
  typedef std::map<std::string, CallbackFunction*>::iterator EntryCB;
  typedef std::map<std::string, DataSource*>::iterator EntryDS;

public:
  LuviewTraitedObject();

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);

  GETSET_TRAITS_D3(Position);
  GETSET_TRAITS_D3(Orientation);
  GETSET_TRAITS_D3(Color);
  GETSET_TRAITS_D3(Scale);
  GETSET_TRAITS_D1(LineWidth);
  GETSET_TRAITS_D1(Alpha);

  static int _get_Callback_(lua_State *L);
  static int _set_Callback_(lua_State *L);
  static int _get_DataSource_(lua_State *L);
  static int _set_DataSource_(lua_State *L);
  static int __get_vec__(lua_State *L, double *v, int n);
  static int __set_vec__(lua_State *L, double *v, int n);
} ;


class DrawableObject : public LuviewTraitedObject
{
protected:
  std::vector<int> gl_modes;

public:
  DrawableObject();
  virtual void draw();

protected:
  virtual void draw_local() = 0;
} ;


class Tesselation3D : public DrawableObject
{
protected:
  void draw_local();
} ;


#endif // __LuviewObjects_HEADER__
