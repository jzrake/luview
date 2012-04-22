

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
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

  /*
  double call(double x)
  {
    lua_State *L = __lua_state;
    int top = lua_gettop(L);
    push_lua_refid(L, callback_refid);
    printf("the ref at index %d is %s\n", callback_refid, luaL_typename(L, -1));
    lua_pushnumber(L, x);
    lua_call(L, 1, 1);
    double res = lua_tonumber(L, -1);
    lua_settop(L, top);
    return res;
  }

  static int __new__(lua_State *L)
  {
    luaL_argcheck(L, lua_type(L, 1) == LUA_TFUNCTION, 1, "function expected");
    CallbackFunction *newobj = new CallbackFunction;
    newobj->callback_refid = make_refid(L, 1);
    return make_lua_obj(L, newobj);
  }
  */
} ;


class LuviewTraitedObject : public LuaCppObject
{
protected:
  double Position[3];
  double Orientation[3];
  double Color[3];
  double Scale[3];
  double LineWidth;
  std::map<std::string, CallbackFunction*> callbacks;

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
    attr["get_callback"] = _get_callback_;
    attr["set_callback"] = _set_callback_;
    RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
  }
  GETSET_TRAITS_D3(Position);
  GETSET_TRAITS_D3(Orientation);
  GETSET_TRAITS_D3(Color);
  GETSET_TRAITS_D3(Scale);
  GETSET_TRAITS_D1(LineWidth);

  static int _get_callback_(lua_State *L)
  {
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);
    const char *name = luaL_checkstring(L, 2);
    if (self->callbacks.find(name) == self->callbacks.end()) {
      lua_pushnil(L);
    }
    else {
      self->push_lua_obj(L, self->callbacks[name]);
    }
    return 1;
  }
  static int _set_callback_(lua_State *L)
  // ---------------------------------------------------------------------------
  // Arguments:
  //
  // (1) Lua function or nil
  //
  // ---------------------------------------------------------------------------
  {
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);
    const char *name = luaL_checkstring(L, 2);
    std::map<std::string, CallbackFunction*>::iterator val =
      self->callbacks.find(name);

    if (lua_type(L, 3) == LUA_TFUNCTION) {
      if (val == self->callbacks.end()) delete self->callbacks[name];
      self->callbacks[name] = new CallbackFunction(L, 3);
    }
    else if (lua_type(L, 3) == LUA_TNIL && (val == self->callbacks.end())) {
      delete self->callbacks[name];
      self->callbacks.erase(val);
    }
    else {
      luaL_error(L, "requires either function or nil");
    }
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

    if (callbacks.find("test_cb") != callbacks.end()) {
      printf("running test_cb\n");
      //      std::cout << callbacks["test_cb"]->call(3.14) <<std::endl;
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
    //    printf("received key: %d\n", key);
  }
  static void CharacterInput(int key, int state)
  {
    //    printf("received character: %\n", key);
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


class SurfaceRendering : public DrawableObject
{
protected:
  GLfloat *surfdata;
  GLfloat *colordata;
  double Lx0, Lx1, Ly0, Ly1;
  int Nx, Ny;

public:
  SurfaceRendering() : surfdata(NULL), colordata(NULL)
  {
    gl_modes.push_back(GL_MAP2_VERTEX_3);
    gl_modes.push_back(GL_AUTO_NORMAL);
    gl_modes.push_back(GL_NORMALIZE);

    Orientation[0] = -90.0;

    Lx0 = -0.5;
    Lx1 =  0.5;
    Ly0 = -0.5;
    Ly1 =  0.5;

    Nx = 0;
    Ny = 0;
  }
  virtual ~SurfaceRendering()
  {
    if (surfdata) free(surfdata);
  }
  virtual void draw_local() = 0;

  void set_data(const double *data, int nx, int ny)
  {
    Nx = nx;
    Ny = ny;

    const int sx = Ny;
    const int sy = 1;

    const double dx = (Lx1 - Lx0) / (Nx - 1);
    const double dy = (Ly1 - Ly0) / (Ny - 1);

    if (surfdata) free(surfdata);
    if (colordata) free(colordata);
    surfdata = (GLfloat*) malloc(Nx*Ny*3*sizeof(GLfloat));
    colordata = (GLfloat*) malloc(Nx*Ny*4*sizeof(GLfloat));

    for (int i=0; i<Nx; ++i) {
      for (int j=0; j<Ny; ++j) {

        const int m = i*sx + j*sy;

        const double x = Lx0 + i*dx;
        const double y = Ly0 + j*dy;
        const double z = data[m];

        surfdata[3*m + 0] = x;
        surfdata[3*m + 1] = y;
        surfdata[3*m + 2] = data[m];

	colordata[4*m + 0] = pow(sin(20*z), 2);
	colordata[4*m + 1] = pow(cos(20*z), 2);
	colordata[4*m + 2] = pow(sin(20*z+10), 4);
	colordata[4*m + 3] = 0.9;
      }
    }
  }

protected:
 virtual LuaInstanceMethod __getattr__(std::string &method_name)
  {
    AttributeMap attr;
    attr["set_data"] = _set_data_;
    RETURN_ATTR_OR_CALL_SUPER(DrawableObject);
  }
  static int _set_data_(lua_State *L)
  {
    SurfaceRendering *self = checkarg<SurfaceRendering>(L, 1);
    Array *A = lunum_checkarray1(L, 2);

    if (A->ndims != 2 || A->dtype != ARRAY_TYPE_DOUBLE) {
      luaL_error(L, "need a 2d array of doubles\n");
    }
    self->set_data((const double*)A->data, A->shape[0], A->shape[1]);

    return 0;
  }
} ;


class SurfaceNURBS : public SurfaceRendering
{
private:
  int order;
  GLUnurbsObj *theNurb;

public:
  SurfaceNURBS() : order(4)
  {
    gl_modes.push_back(GL_LIGHTING);
    gl_modes.push_back(GL_LIGHT0);
    gl_modes.push_back(GL_BLEND);
    gl_modes.push_back(GL_COLOR_MATERIAL);

    Orientation[0] = -90.0;

    theNurb = gluNewNurbsRenderer();
    gluNurbsProperty(theNurb, GLU_SAMPLING_METHOD, GLU_DOMAIN_DISTANCE);
    gluNurbsProperty(theNurb, GLU_U_STEP, 0.1);
    gluNurbsProperty(theNurb, GLU_V_STEP, 0.1);

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
    GLfloat *knots_x = (GLfloat*) malloc((Nx + order)*sizeof(GLfloat));
    GLfloat *knots_y = (GLfloat*) malloc((Ny + order)*sizeof(GLfloat));

    for (int i=0; i<Nx+order; ++i) knots_x[i] = i;
    for (int i=0; i<Ny+order; ++i) knots_y[i] = i;

    const int sx = Ny;
    const int sy = 1;

    GLfloat mat_diffuse[] = { 0.3, 0.6, 0.7, 0.8 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.8 };
    GLfloat mat_shininess[] = { 100.0 };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    gluBeginSurface(theNurb);
    gluNurbsSurface(theNurb,
                    Nx + order, knots_x, Ny + order, knots_y,
                    3*sx, 3*sy, surfdata,
                    order, order, GL_MAP2_VERTEX_3);
    gluNurbsSurface(theNurb,
                    Nx + order, knots_x, Ny + order, knots_y,
                    4*sx, 4*sy, colordata,
		    order, order, GL_MAP2_COLOR_4);

    gluEndSurface(theNurb);

    free(knots_x);
    free(knots_y);
  }

  static void nurbsError(GLenum errorCode)
  {
    fprintf(stderr, "Nurbs Error: %s\n", gluErrorString(errorCode));
    exit(0);
  }
} ;





extern "C" int luaopen_luview(lua_State *L)
{
  printf("loading luview...\n");

  lua_newtable(L);

  LuaCppObject::Init(L);
  LuaCppObject::Register<Window>(L);
  LuaCppObject::Register<BoundingBox>(L);
  LuaCppObject::Register<SurfaceNURBS>(L);
  //  LuaCppObject::RegisterWithConstructor<CallbackFunction>(L);

  return 1;
}

