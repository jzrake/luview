

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <algorithm>

#include "lua_object.hpp"

extern "C" {
#define LUNUM_API_NOCOMPLEX
#include "numarray.h"
#include "lunum.h"
#include "GL/glfw.h"
}



// ---------------------------------------------------------------------------
#define GETSET_TRAITS_D1(prop)                                          \
  static int _get_##prop##_(lua_State *L) {                             \
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);    \
    lua_remove(L, 1);                                                   \
    return __get_vec__(L, &self->prop, 1);                              \
  }                                                                     \
  static int _set_##prop##_(lua_State *L) {                             \
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);    \
    lua_remove(L, 1);                                                   \
    return __set_vec__(L, &self->prop, 1);                              \
  }                                                                     \
  // ---------------------------------------------------------------------------
#define GETSET_TRAITS_D3(prop)                                          \
  static int _get_##prop##_(lua_State *L) {                             \
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);    \
    lua_remove(L, 1);                                                   \
    return __get_vec__(L, self->prop, 3);                               \
  }                                                                     \
  static int _set_##prop##_(lua_State *L) {                             \
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);    \
    lua_remove(L, 1);                                                   \
    return __set_vec__(L, self->prop, 3);                               \
  }                                                                     \
  // ---------------------------------------------------------------------------



class CallbackFunction : public LuaCppObject
{
public:
  CallbackFunction(lua_State *L, int pos) : LuaCppObject(L, pos) { }

  std::vector<double> call(double u)
  {
    double x[1] = {u};
    return call_n(x, 1);
  }
  std::vector<double> call(double u, double v)
  {
    double x[2] = {u,v};
    return call_n(x, 2);
  }
  std::vector<double> call(double u, double v, double w)
  {
    double x[3] = {u,v,w};
    return call_n(x, 3);
  }
  std::vector<double> call_n(std::vector<double> X)
  {
    return call_n(&X[0], X.size());
  }
  std::vector<double> call_n(double *x, int narg)
  {
    lua_State *L = __lua_state;
    std::vector<double> res;

    push_lua_refid(L, __refid, __REGCXX);
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
} ;


class DataSource : public LuaCppObject
{
protected:
  DataSource *input;
  GLfloat *output;
  CallbackFunction *transform;

public:
  DataSource() : input(NULL), output(NULL), transform(NULL) { }

  virtual GLfloat *get_data() = 0;
  virtual int get_num_points(int d) { return 0; }
  virtual int get_size() { return 0; }
  virtual int get_num_components() { return 0; }

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name)
  {
    AttributeMap attr;
    attr["get_transform"] = _get_transform_;
    attr["set_transform"] = _set_transform_;
    attr["get_input"] = _get_input_;
    attr["set_input"] = _set_input_;
    RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
  }
  static int _get_transform_(lua_State *L)
  {
    DataSource *self = checkarg<DataSource>(L, 1);
    if (self->transform == NULL) {
      lua_pushnil(L);
    }
    else {
      self->push_lua_obj(L, self->transform, __REGCXX);
    }
    return 1;
  }

  static int _set_transform_(lua_State *L)
  {
    DataSource *self = checkarg<DataSource>(L, 1);
    if (lua_type(L, 2) == LUA_TFUNCTION) {
      if (self->transform) delete self->transform;
      self->transform = new CallbackFunction(L, 2);
    }
    else if (lua_type(L, 2) == LUA_TNIL) {
      if (self->transform) delete self->transform;
      self->transform = NULL;
    }
    else {
      luaL_error(L, "requires either function or nil");
    }
    return 0;
  }
  static int _get_input_(lua_State *L)
  {
    DataSource *self = checkarg<DataSource>(L, 1);

    if (self->input == NULL) {
      lua_pushnil(L);
    }
    else {
      self->push_lua_obj(L, self->input, __REGLUA); 
    }
    return 1;
  }
  static int _set_input_(lua_State *L)
  {
    DataSource *self = checkarg<DataSource>(L, 1);
    self->input = checkarg<DataSource>(L, 2);
    return 0;
  }
} ;


class FunctionDataSource : public DataSource
{
protected:
  CallbackFunction *callback;

public:
  FunctionDataSource() : callback(NULL) { }
  ~FunctionDataSource()
  {
    if (callback) delete callback;
  }

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name)
  {
    AttributeMap attr;
    attr["get_function"] = _get_function_;
    attr["set_function"] = _set_function_;
    RETURN_ATTR_OR_CALL_SUPER(DataSource);
  }
  static int _get_function_(lua_State *L)
  {
    FunctionDataSource *self = checkarg<FunctionDataSource>(L, 1);
    if (self->callback == NULL) {
      lua_pushnil(L);
    }
    else {
      self->push_lua_obj(L, self->callback, __REGCXX);
    }
    return 1;
  }
  static int _set_function_(lua_State *L)
  // ---------------------------------------------------------------------------
  // Arguments:
  //
  // (1) Lua function or nil
  //
  // ---------------------------------------------------------------------------
  {
    FunctionDataSource *self = checkarg<FunctionDataSource>(L, 1);
    if (lua_type(L, 2) == LUA_TFUNCTION) {
      if (self->callback) delete self->callback;
      self->callback = new CallbackFunction(L, 2);
    }
    else if (lua_type(L, 2) == LUA_TNIL) {
      delete self->callback;
      self->callback = NULL;
    }
    else {
      luaL_error(L, "requires either function or nil");
    }
    return 0;
  }
} ;


class ArrayDataSource : public DataSource
{
protected:
  Array *A;
public:
  ArrayDataSource() : A(NULL) { }

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name)
  {
    AttributeMap attr;
    attr["get_array"] = _get_array_;
    attr["set_array"] = _set_array_;
    RETURN_ATTR_OR_CALL_SUPER(DataSource);
  }
  static int _get_array_(lua_State *L)
  {
    ArrayDataSource *self = checkarg<ArrayDataSource>(L, 1);
    Array B = array_new_copy(self->A, self->A->dtype);
    lunum_pusharray1(L, &B);
    return 1;
  }
  static int _set_array_(lua_State *L)
  {
    ArrayDataSource *self = checkarg<ArrayDataSource>(L, 1);
    self->A = lunum_checkarray1(L, 2);
    return 0;
  }
} ;



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
  LuviewTraitedObject()
  {
    Position[0] = 0.0;
    Position[1] = 0.0;
    Position[2] = 0.0;

    Orientation[0] = 0.0;
    Orientation[1] = 0.0;
    Orientation[2] = 0.0;

    Color[0] = 1.0;
    Color[1] = 1.0;
    Color[2] = 1.0;

    Scale[0] = 1.0;
    Scale[1] = 1.0;
    Scale[2] = 1.0;

    Alpha = 0.9;
    LineWidth = 1.0;
  }
  void set_position(double x, double y, double z);
  void set_orientation(double x, double y, double z);
  void set_color(double x, double y, double z);
  void set_scale(double x, double y, double z);
  void set_linewidth(double w);

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name)
  {
    AttributeMap attr;
    attr["get_position"] = _get_Position_;
    attr["set_position"] = _set_Position_;
    attr["get_orientation"] = _get_Orientation_;
    attr["set_orientation"] = _set_Orientation_;
    attr["get_color"] = _get_Color_;
    attr["set_color"] = _set_Color_;
    attr["get_scale"] = _get_Scale_;
    attr["set_scale"] = _set_Scale_;
    attr["get_linewidth"] = _get_LineWidth_;
    attr["set_linewidth"] = _set_LineWidth_;
    attr["get_alpha"] = _get_Alpha_;
    attr["set_alpha"] = _set_Alpha_;
    attr["get_callback"] = _get_Callback_;
    attr["set_callback"] = _set_Callback_;
    attr["get_data"] = _get_DataSource_;
    attr["set_data"] = _set_DataSource_;
    RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
  }
  GETSET_TRAITS_D3(Position);
  GETSET_TRAITS_D3(Orientation);
  GETSET_TRAITS_D3(Color);
  GETSET_TRAITS_D3(Scale);
  GETSET_TRAITS_D1(LineWidth);
  GETSET_TRAITS_D1(Alpha);

  static int _get_Callback_(lua_State *L)
  {
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);
    const char *name = luaL_checkstring(L, 2);
    EntryCB val = self->Callbacks.find(name);

    if (val == self->Callbacks.end()) {
      lua_pushnil(L);
    }
    else {
      self->push_lua_obj(L, val->second, __REGCXX);
    }
    return 1;
  }
  static int _set_Callback_(lua_State *L)
  // ---------------------------------------------------------------------------
  // Arguments:
  //
  // (1) Lua function or nil
  //
  // ---------------------------------------------------------------------------
  {
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);
    const char *name = luaL_checkstring(L, 2);
    EntryCB val = self->Callbacks.find(name);

    if (lua_type(L, 3) == LUA_TFUNCTION) {
      if (val == self->Callbacks.end()) delete self->Callbacks[name];
      self->Callbacks[name] = new CallbackFunction(L, 3);
    }
    else if (lua_type(L, 3) == LUA_TNIL && (val == self->Callbacks.end())) {
      delete self->Callbacks[name];
      self->Callbacks.erase(val);
    }
    else {
      luaL_error(L, "requires either function or nil");
    }
    return 0;
  }
  static int _get_DataSource_(lua_State *L)
  {
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);
    const char *name = luaL_checkstring(L, 2);

    EntryDS val = self->DataSources.find(name);

    if (val == self->DataSources.end()) {
      lua_pushnil(L);
    }
    else {
      self->push_lua_obj(L, val->second, __REGLUA); 
    }
    return 1;
  }
  static int _set_DataSource_(lua_State *L)
  {
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);
    const char *name = luaL_checkstring(L, 2);
    self->DataSources[name] = checkarg<DataSource>(L, 3);
    return 0;
  }

  static int __get_vec__(lua_State *L, double *v, int n)
  {
    for (int i=0; i<n; ++i) lua_pushnumber(L, v[i]);
    return n;
  }
  static int __set_vec__(lua_State *L, double *v, int n)
  {
    for (int i=0; i<n; ++i) v[i] = luaL_checknumber(L, i+1);
    return 0;
  }
} ;



class DrawableObject : public LuviewTraitedObject
{
protected:
  std::vector<int> gl_modes;

public:
  DrawableObject()
  {
    gl_modes.push_back(GL_DEPTH_TEST);
  }
  virtual void draw()
  {
    for (unsigned int i=0; i<gl_modes.size(); ++i) {
      glEnable(gl_modes[i]);
    }

    glPushMatrix();

    glTranslated(Position[0], Position[1], Position[2]);
    glRotated(Orientation[0], 1, 0, 0);
    glRotated(Orientation[1], 0, 1, 0);
    glRotated(Orientation[2], 0, 0, 1);
    glScaled(Scale[0], Scale[1], Scale[2]);

    glColor3dv(Color);
    glLineWidth(LineWidth);

    this->draw_local();

    glPopMatrix();

    for (unsigned int i=0; i<gl_modes.size(); ++i) {
      glDisable(gl_modes[i]);
    }
  }

protected:
  virtual void draw_local() = 0;
} ;


class Window : public LuviewTraitedObject
{
private:
  double WindowWidth, WindowHeight;
  static Window *CurrentWindow;

public:
  Window() : WindowWidth(1024),
             WindowHeight(768)
  {
    Orientation[0] = 9.0;
    this->start_window();
  }
  virtual ~Window() { }

private:
  void start_window()
  {
    glfwInit();
    glfwOpenWindow(WindowWidth, WindowHeight, 0,0,0,0,0,0, GLFW_WINDOW);

    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float) WindowWidth / WindowHeight, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    glfwSetKeyCallback(KeyboardInput);
    glfwSetCharCallback(CharacterInput);
    glfwEnable(GLFW_STICKY_KEYS);
    glfwEnable(GLFW_KEY_REPEAT);
  }

  const char *render_scene(std::vector<DrawableObject*> &actors)
  {
    CurrentWindow = this;
    glClearColor(Color[0], Color[1], Color[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslated(Position[0], Position[1], Position[2]);
    glRotated(Orientation[0], 1, 0, 0);
    glRotated(Orientation[1], 0, 1, 0);

    for (std::vector<DrawableObject*>::iterator a=actors.begin();
         a!=actors.end(); ++a) {
      DrawableObject *actor = *a;
      actor->draw();
    }

    glFlush();
    glfwSwapBuffers();

    if (glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED)) {
      glfwCloseWindow();
      return "terminate";
    }
    return "continue";
  }

private:
  static void KeyboardInput(int key, int state)
  {
    if (state != GLFW_PRESS) return;
    double *Orientation = CurrentWindow->Orientation;
    switch (key) {
    case GLFW_KEY_RIGHT : Orientation[1] += 3; break;
    case GLFW_KEY_LEFT  : Orientation[1] -= 3; break;

    case GLFW_KEY_DOWN  : Orientation[0] += 3; break;
    case GLFW_KEY_UP    : Orientation[0] -= 3; break;
    }
  }
  static void CharacterInput(int key, int state)
  {

  }

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name)
  {
    AttributeMap attr;
    attr["render_scene"] = _render_scene_;
    RETURN_ATTR_OR_CALL_SUPER(LuviewTraitedObject);
  }
  static int _render_scene_(lua_State *L)
  {
    Window *self = checkarg<Window>(L, 1);
    luaL_argcheck(L, lua_type(L, 2) == LUA_TTABLE, 2, "table expected");
    std::vector<DrawableObject*> actors;

    for (unsigned int i=1; i<=lua_rawlen(L, 2); ++i) {
      lua_rawgeti(L, 2, i);
      actors.push_back(checkarg<DrawableObject>(L, -1));
      lua_pop(L, 1);
    }

    lua_pushstring(L, self->render_scene(actors));
    return 1;
  }
} ;
Window *Window::CurrentWindow;


class BoundingBox : public DrawableObject
{
public:
  void draw_local()
  {
    glBegin(GL_LINES);
    // x-edges
    glVertex3f(-0.5, -0.5, -0.5); glVertex3f(+0.5, -0.5, -0.5);
    glVertex3f(-0.5, -0.5, +0.5); glVertex3f(+0.5, -0.5, +0.5);
    glVertex3f(-0.5, +0.5, -0.5); glVertex3f(+0.5, +0.5, -0.5);
    glVertex3f(-0.5, +0.5, +0.5); glVertex3f(+0.5, +0.5, +0.5);

    // y-edges
    glVertex3f(-0.5, -0.5, -0.5); glVertex3f(-0.5, +0.5, -0.5);
    glVertex3f(+0.5, -0.5, -0.5); glVertex3f(+0.5, +0.5, -0.5);
    glVertex3f(-0.5, -0.5, +0.5); glVertex3f(-0.5, +0.5, +0.5);
    glVertex3f(+0.5, -0.5, +0.5); glVertex3f(+0.5, +0.5, +0.5);

    // z-edges
    glVertex3f(-0.5, -0.5, -0.5); glVertex3f(-0.5, -0.5, +0.5);
    glVertex3f(-0.5, +0.5, -0.5); glVertex3f(-0.5, +0.5, +0.5);
    glVertex3f(+0.5, -0.5, -0.5); glVertex3f(+0.5, -0.5, +0.5);
    glVertex3f(+0.5, +0.5, -0.5); glVertex3f(+0.5, +0.5, +0.5);

    glEnd();
  }
} ;



class ParametricArrayDataSource : public ArrayDataSource
{
protected:
  GLfloat *ctrlpoint;
  double u0, u1, v0, v1;
  int Nu, Nv;

public:
  int get_Nu() { return Nu; }
  int get_Nv() { return Nv; }

  ParametricArrayDataSource()
  {
    u0 = -0.5;
    u1 =  0.5;
    v0 = -0.5;
    v1 =  0.5;

    Nu = 16;
    Nv = 16;

    ctrlpoint = (GLfloat*) malloc(3*Nu*Nv*sizeof(GLfloat));

    const int su = Nv;
    const int sv = 1;
    const double du = (u1 - u0) / (Nu - 1);
    const double dv = (v1 - v0) / (Nv - 1);

    // -------------------------------------------------------------------------
    // Initialize to a uniform, flat grid
    // -------------------------------------------------------------------------
    for (int i=0; i<Nu; ++i) {
      for (int j=0; j<Nv; ++j) {
        const int m = i*su + j*sv;
        const double x = u0 + i*du;
        const double y = v0 + j*dv;
        const double z = 0.0;
        ctrlpoint[3*m + 0] = x;
        ctrlpoint[3*m + 1] = y;
        ctrlpoint[3*m + 2] = z;
      }
    }
  }
  ~ParametricArrayDataSource()
  {
    free(ctrlpoint);
  }

  GLfloat *get_data()
  // ---------------------------------------------------------------------------
  // Attempts to use the lunum array belonging to parent ArrayDataSource as a
  // source for 3d control points.
  // ---------------------------------------------------------------------------
  {
    if (A->dtype != ARRAY_TYPE_DOUBLE) {
      luaL_error(__lua_state, "data source 'control_locations' must "
                 "have type 'double'");
    }
    else if (A->ndims == 2) {
      Nu = A->shape[0];
      Nv = A->shape[1];

      const int su = Nv;
      const int sv = 1;
      const double du = (u1 - u0) / (Nu - 1);
      const double dv = (v1 - v0) / (Nv - 1);

      ctrlpoint = (GLfloat*) realloc(ctrlpoint, 3*Nu*Nv*sizeof(GLfloat));
      for (int i=0; i<Nu; ++i) {
	for (int j=0; j<Nv; ++j) {
	  const int m = i*su + j*sv;
	  ctrlpoint[3*m + 0] = u0 + i*du;
	  ctrlpoint[3*m + 1] = v0 + j*dv;
	  ctrlpoint[3*m + 2] = ((double*) A->data)[m];
	}
      }
    }
    else if (A->ndims == 3) {
      if (A->shape[2] == 3) {
	Nu = A->shape[0];
	Nv = A->shape[1];
	ctrlpoint = (GLfloat*) realloc(ctrlpoint, 3*Nu*Nv*sizeof(GLfloat));
	for (int m=0; m<3*Nu*Nv; ++m) {
	  ctrlpoint[m] = ((double*) A->data)[m];
	}
      }
      else {
	luaL_error(__lua_state, "data source 'control_locations' must "
		   "have 3rd dimension of size 3");
      }
    }
    else {
      luaL_error(__lua_state, "data source 'control_locations' must "
                 "have dimension 2 or 3");
    }
    return ctrlpoint;
  }
} ;


class ParametricFunctionDataSource : public FunctionDataSource
{
protected:
  GLfloat *ctrlpoint;
  double u0, u1, v0, v1;
  int Nu, Nv;

public:
  int get_Nu() { return Nu; }
  int get_Nv() { return Nv; }

  ParametricFunctionDataSource()
  {
    u0 = -0.5;
    u1 =  0.5;
    v0 = -0.5;
    v1 =  0.5;

    Nu = 16;
    Nv = 16;

    ctrlpoint = (GLfloat*) malloc(3*Nu*Nv*sizeof(GLfloat));

    const int su = Nv;
    const int sv = 1;
    const double du = (u1 - u0) / (Nu - 1);
    const double dv = (v1 - v0) / (Nv - 1);

    // -------------------------------------------------------------------------
    // Initialize to a uniform, flat grid
    // -------------------------------------------------------------------------
    for (int i=0; i<Nu; ++i) {
      for (int j=0; j<Nv; ++j) {
        const int m = i*su + j*sv;
        const double x = u0 + i*du;
        const double y = v0 + j*dv;
        const double z = 0.0;
        ctrlpoint[3*m + 0] = x;
        ctrlpoint[3*m + 1] = y;
        ctrlpoint[3*m + 2] = z;
      }
    }
  }
  ~ParametricFunctionDataSource()
  {
    free(ctrlpoint);
  }

  GLfloat *get_data()
  {
    const int su = Nv;
    const int sv = 1;
    const double du = (u1 - u0) / (Nu - 1);
    const double dv = (v1 - v0) / (Nv - 1);

    // -----------------------------------------------------------------------
    // Refresh the ctrlpoint buffer with callback values
    // -----------------------------------------------------------------------
    ctrlpoint = (GLfloat*) realloc(ctrlpoint, 3*Nu*Nv*sizeof(GLfloat));
    for (int i=0; i<Nu; ++i) {
      for (int j=0; j<Nv; ++j) {
        const int m = i*su + j*sv;
        const double u = u0 + i*du;
        const double v = v0 + j*dv;

        std::vector<double> V = callback->call(u, v);
        ctrlpoint[3*m + 0] = V[0];
        ctrlpoint[3*m + 1] = V[1];
        ctrlpoint[3*m + 2] = V[2];
      }
    }

    return ctrlpoint;
  }
} ;


class GridSource2D :  public DataSource
{
private:
  GLfloat *output;
  int Nu, Nv;
  double u0, u1, v0, v1;

public:
  GridSource2D() : output(NULL)
  {
    u0 = -0.5;
    u1 =  0.5;
    v0 = -0.5;
    v1 =  0.5;

    Nu = 16;
    Nv = 16;

    init_grid();
  }
  ~GridSource2D()
  {
    free(output);
  }
  GLfloat *get_data() { return output; }
  virtual int get_num_points(int d)
  {
    switch (d) {
    case 0: return Nu;
    case 1: return Nv;
    default: return 0;
    }
  }
  virtual int get_size()
  {
    return Nu*Nv;
  }
  virtual int get_num_components()
  {
    return 2;
  }

private:
  void init_grid()
  // ---------------------------------------------------------------------------
  // Creates to a uniform cartesian grid
  // ---------------------------------------------------------------------------
  {
    output = (GLfloat*) realloc(output, 2*Nu*Nv*sizeof(GLfloat));

    const int su = Nv;
    const int sv = 1;
    const double du = (u1 - u0) / (Nu - 1);
    const double dv = (v1 - v0) / (Nv - 1);

    for (int i=0; i<Nu; ++i) {
      for (int j=0; j<Nv; ++j) {
        const int m = i*su + j*sv;
        output[2*m + 0] = u0 + i*du;
        output[2*m + 1] = v0 + j*dv;
      }
    }
  }
} ;


class FunctionMapping : public DataSource
{
public:
  ~FunctionMapping()
  {
    if (output) free(output);
  }
  virtual int get_num_points(int d)
  {
    return input ? input->get_num_points(d) : 0;
  }
  virtual int get_size()
  {
    return input ? input->get_size() : 0;
  }
  virtual int get_num_components()
  {
    return transform ? transform->call_n
      (std::vector<double>(input->get_num_components(), 0.0)).size() : 0;
  }

  GLfloat *get_data()
  // ---------------------------------------------------------------------------
  // Refresh the ctrlpoint buffer with callback values
  // ---------------------------------------------------------------------------
  {
    int Nd_domain = input->get_num_components();
    int Nd_range = this->get_num_components();
    int nval_output = Nd_range * input->get_size();

    output = (GLfloat*) realloc(output, nval_output*sizeof(GLfloat));

    GLfloat *domain = input->get_data();

    for (int n=0; n<input->get_size(); ++n) {

      // Here we're loading data from the domain data into the argument vector
      std::vector<double> X(domain + Nd_domain*n, domain + Nd_domain*(n+1));
      std::vector<double> Y = transform->call_n(X);

      for (int d=0; d<Nd_range; ++d) {
	output[Nd_range*n + d] = Y[d];
      }
    }

    return output;
  }
} ;




// -----------------------------------------------------------------------------
//
// [0,Nu] x [0,Nv] -> R^3 (parametric data source)
// R^3 -> R (get_scalars from it)
// R -> R^4 (map scalars through color map)
//
// -----------------------------------------------------------------------------
class SurfaceNURBS : public DrawableObject
{
private:
  int order;
  GLUnurbsObj *theNurb;

public:
  SurfaceNURBS() : order(3)
  {
    gl_modes.push_back(GL_LIGHTING);
    gl_modes.push_back(GL_LIGHT0);
    gl_modes.push_back(GL_BLEND);
    gl_modes.push_back(GL_COLOR_MATERIAL);
    gl_modes.push_back(GL_AUTO_NORMAL);

    Orientation[0] = -90.0;

    theNurb = gluNewNurbsRenderer();
    gluNurbsProperty(theNurb, GLU_SAMPLING_METHOD, GLU_DOMAIN_DISTANCE);
    gluNurbsProperty(theNurb, GLU_U_STEP, 10.0);
    gluNurbsProperty(theNurb, GLU_V_STEP, 10.0);

    gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);//GLU_OUTLINE_PATCH);
    gluNurbsCallback(theNurb, GLU_ERROR, (GLvoid (*)()) nurbsError);
  }
  ~SurfaceNURBS()
  {
    gluDeleteNurbsRenderer(theNurb);
  }

private:
  void draw_local()
  {
    EntryDS cp = DataSources.find("control_points");
    EntryDS cm = DataSources.find("color_data");

    if (cp == DataSources.end()) {
      return;
    }
    if (cp->second->get_num_components() != 3) {
      printf("need 3-component input data for control_points\n");
      return;
    }

    int Nu = cp->second->get_num_points(0);
    int Nv = cp->second->get_num_points(1);

    GLfloat *knots_u = (GLfloat*) malloc((Nu + order)*sizeof(GLfloat));
    GLfloat *knots_v = (GLfloat*) malloc((Nv + order)*sizeof(GLfloat));

    for (int i=0; i<Nu+order; ++i) knots_u[i] = i;
    for (int i=0; i<Nv+order; ++i) knots_v[i] = i;

    const int su = Nv;
    const int sv = 1;
    GLfloat *surfdata = cp->second->get_data();

    GLfloat mat_diffuse[] = { 0.3, 0.6, 0.7, 0.8 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.8 };
    GLfloat mat_shininess[] = { 100.0 };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    gluBeginSurface(theNurb);
    gluNurbsSurface(theNurb,
                    Nu + order, knots_u, Nv + order, knots_v,
                    3*su, 3*sv, surfdata,
                    order, order, GL_MAP2_VERTEX_3);

    if (cm != DataSources.end()) {
      gluNurbsSurface(theNurb,
		      Nu + order, knots_u, Nv + order, knots_v,
		      4*su, 4*sv, cm->second->get_data(),
		      order, order, GL_MAP2_COLOR_4);
    }

    gluEndSurface(theNurb);
    free(knots_u);
    free(knots_v);
  }

  static void nurbsError(GLenum errorCode)
  {
    fprintf(stderr, "Nurbs Error: %s\n", gluErrorString(errorCode));
    exit(0);
  }
} ;





extern "C" int luaopen_luview(lua_State *L)
{
  lua_newtable(L);

  LuaCppObject::Init(L);
  LuaCppObject::Register<Window>(L);
  LuaCppObject::Register<BoundingBox>(L);
  LuaCppObject::Register<SurfaceNURBS>(L);
  LuaCppObject::Register<ParametricFunctionDataSource>(L);
  LuaCppObject::Register<ParametricArrayDataSource>(L);
  
  LuaCppObject::Register<FunctionMapping>(L);
  LuaCppObject::Register<GridSource2D>(L);

  return 1;
}
