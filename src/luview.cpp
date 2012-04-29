

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <algorithm>

#include "luview.hpp"

extern "C" {
#define LUNUM_API_NOCOMPLEX
#include "numarray.h"
#include "lunum.h"
#include "GL/glfw.h"
}




CallbackFunction::CallbackFunction(lua_State *L, int pos) :
  LuaCppObject(L, pos) { }

std::vector<double> CallbackFunction::call(double u)
{
  double x[1] = {u};
  return call_n(x, 1);
}
std::vector<double> CallbackFunction::call(double u, double v)
{
  double x[2] = {u,v};
  return call_n(x, 2);
}
std::vector<double> CallbackFunction::call(double u, double v, double w)
{
  double x[3] = {u,v,w};
  return call_n(x, 3);
}
std::vector<double> CallbackFunction::call_n(std::vector<double> X,
                                             LuaCppObject *caller)
{
  return call_n(&X[0], X.size(), caller);
}
std::vector<double> CallbackFunction::call_n(double *x, int narg,
                                             LuaCppObject *caller)
{
  lua_State *L = __lua_state;
  std::vector<double> res;
  push_lua_refid(L, __refid, __REGCXX);
  for (int i=0; i<narg; ++i) {
    lua_pushnumber(L, x[i]);
  }
  // ---------------------------------------------------------------------------
  // Append the caller as a Lua object to the argument list if it's given
  // ---------------------------------------------------------------------------
  if (caller != NULL) {
    push_lua_obj(__lua_state, caller, __REGLUA);
    ++narg;
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




DataSource::DataSource() : input(NULL), output(NULL), transform(NULL) { }
DataSource::~DataSource()
{
  if (output) free(output);
}

DataSource::LuaInstanceMethod DataSource::__getattr__(std::string &method_name)
{
  AttributeMap attr;
  attr["get_transform"] = _get_transform_;
  attr["set_transform"] = _set_transform_;
  attr["get_input"] = _get_input_;
  attr["set_input"] = _set_input_;
  RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
}
int DataSource::_get_transform_(lua_State *L)
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
int DataSource::_set_transform_(lua_State *L)
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
int DataSource::_get_input_(lua_State *L)
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
int DataSource::_set_input_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  self->input = checkarg<DataSource>(L, 2);
  return 0;
}



LuviewTraitedObject::LuviewTraitedObject()
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
LuviewTraitedObject::LuaInstanceMethod LuviewTraitedObject::__getattr__
(std::string &method_name)
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

int LuviewTraitedObject::_get_Callback_(lua_State *L)
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
int LuviewTraitedObject::_set_Callback_(lua_State *L)
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
int LuviewTraitedObject::_get_DataSource_(lua_State *L)
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
int LuviewTraitedObject::_set_DataSource_(lua_State *L)
{
  LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);
  const char *name = luaL_checkstring(L, 2);
  self->DataSources[name] = checkarg<DataSource>(L, 3);
  return 0;
}

int LuviewTraitedObject::__get_vec__(lua_State *L, double *v, int n)
{
  for (int i=0; i<n; ++i) lua_pushnumber(L, v[i]);
  return n;
}
int LuviewTraitedObject::__set_vec__(lua_State *L, double *v, int n)
{
  for (int i=0; i<n; ++i) v[i] = luaL_checknumber(L, i+1);
  return 0;
}



DrawableObject::DrawableObject()
{
  gl_modes.push_back(GL_DEPTH_TEST);
}
void DrawableObject::draw()
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

  glColor4d(Color[0], Color[1], Color[2], Alpha);
  glLineWidth(LineWidth);

  this->draw_local();

  glPopMatrix();

  for (unsigned int i=0; i<gl_modes.size(); ++i) {
    glDisable(gl_modes[i]);
  }
}



class Window : public LuviewTraitedObject
{
private:
  double WindowWidth, WindowHeight;
  int character_input;
  static Window *CurrentWindow;

public:
  Window() : WindowWidth(1024),
             WindowHeight(768), character_input(0)
  {
    Orientation[0] = 9.0;
    Position[2] = -2.0;
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
    character_input = ' ';
    CurrentWindow = this;

    glClearColor(Color[0], Color[1], Color[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslated(Position[0], Position[1], Position[2]);
    glRotated(Orientation[0], 1, 0, 0);
    glRotated(Orientation[1], 0, 1, 0);
    glScaled(Scale[0], Scale[1], Scale[2]);

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
    double *Position = CurrentWindow->Position;
    double *Scale = CurrentWindow->Scale;

    switch (key) {
    case 'a': Position[0] += 0.1; break;
    case 'd': Position[0] -= 0.1; break;

    case 'f': Position[1] += 0.1; break;
    case 'r': Position[1] -= 0.1; break;

    case 'w': Position[2] += 0.1; break;
    case 's': Position[2] -= 0.1; break;

    case 'Z': for (int i=0; i<3; ++i) Scale[i] *= 1.05; break;
    case 'z': for (int i=0; i<3; ++i) Scale[i] /= 1.05; break;
    }
    CurrentWindow->character_input = key;
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
    lua_pushlstring(L, (char*)&self->character_input, 1);
    return 2;
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
  virtual int get_size() { return Nu*Nv; }
  virtual int get_num_components() { return 2; }
  virtual int get_num_dimensions() { return 2; }

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


PointsSource::PointsSource() : output(NULL)
{
  Np = 0;
  Nc = 0;
}
PointsSource::~PointsSource()
{
  if (output) free(output);
}
GLfloat *PointsSource::get_data() { return output; }
int PointsSource::get_num_points(int d)
{
  switch (d) {
  case 0: return Np;
  default: return 0;
  }
}
int PointsSource::get_size() { return Np; }
int PointsSource::get_num_components() { return Nc; }
int PointsSource::get_num_dimensions() { return 1; }

PointsSource::LuaInstanceMethod PointsSource::__getattr__
(std::string &method_name)
{
  AttributeMap attr;
  attr["set_points"] = _set_points_;
  RETURN_ATTR_OR_CALL_SUPER(DataSource);
}
int PointsSource::_set_points_(lua_State *L)
{
  PointsSource *self = checkarg<PointsSource>(L, 1);
  Array *A = lunum_checkarray1(L, 2);

  if (A->ndims != 2) {
    luaL_error(L, "array must have dimension 2");
  }
  else if (A->dtype != ARRAY_TYPE_DOUBLE) {
    luaL_error(L, "array must have type 'double'");
  }

  int Nt = A->shape[0]*A->shape[1];
  self->Np = A->shape[0];
  self->Nc = A->shape[1];
  self->output = (GLfloat*) realloc(self->output, Nt*sizeof(double));
  for (int i=0; i<Nt; ++i) self->output[i] = ((double*)A->data)[i];

  return 0;
}




int FunctionMapping::get_num_points(int d)
{
  return input ? input->get_num_points(d) : 0;
}
int FunctionMapping::get_size()
{
  return input ? input->get_size() : 0;
}
int FunctionMapping::get_num_components()
{
  return (transform && input) ? transform->call_n
    (std::vector<double>(input->get_num_components(), 0.0)).size() : 0;
}
int FunctionMapping::get_num_dimensions()
{
  return input ? input->get_num_dimensions() : 0;
}

GLfloat *FunctionMapping::get_data()
// -----------------------------------------------------------------------------
// Refresh the output buffer with callback results
// -----------------------------------------------------------------------------
{
  if (input == NULL) {
    luaL_error(__lua_state, "broken pipeline: missing data source");
  }
  if (transform == NULL) {
    luaL_error(__lua_state, "broken pipeline: missing transform");
  }

  int Nd_domain = input->get_num_components();
  int Nd_range = this->get_num_components();
  int nval_output = Nd_range * input->get_size();

  output = (GLfloat*) realloc(output, nval_output*sizeof(GLfloat));
  GLfloat *domain = input->get_data();

  // ---------------------------------------------------------------------------
  // Set up info dictionary
  // ---------------------------------------------------------------------------
  for (int d=0; d<Nd_domain; ++d) {
    double &xmax = info[K("max", d)];
    double &xmin = info[K("min", d)];
    xmin = +1e16;
    xmax = -1e16;
    for (int n=0; n<input->get_size(); ++n) {
      const GLfloat x = domain[Nd_domain*n + d];
      if (x > xmax) xmax = x;
      if (x < xmin) xmin = x;
    }
  }

  for (int n=0; n<input->get_size(); ++n) {

    // Here we're loading data from the domain data into the argument vector
    std::vector<double> X(domain + Nd_domain*n, domain + Nd_domain*(n+1));
    std::vector<double> Y = transform->call_n(X, this);

    for (int d=0; d<Nd_range; ++d) {
      output[Nd_range*n + d] = Y[d];
    }
  }
  return output;
}
std::string FunctionMapping::K(const char *s, int d)
{
  char res[256];
  sprintf(res, "%s%d", s, d);
  return res;
}

FunctionMapping::LuaInstanceMethod FunctionMapping::__getattr__
(std::string &method_name)
{
  AttributeMap attr;
  attr["get_info"] = _get_info_;
  RETURN_ATTR_OR_CALL_SUPER(DataSource);
}
int FunctionMapping::_get_info_(lua_State *L)
{
  FunctionMapping *self = checkarg<FunctionMapping>(L, 1);
  std::string key = luaL_checkstring(L, 2);
  std::map<std::string, double>::iterator val = self->info.find(key);

  if (val != self->info.end()) {
    lua_pushnumber(L, val->second);
  }
  else {
    lua_pushnil(L);
  }

  return 1;
}





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
    gl_modes.push_back(GL_NORMALIZE);

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
    EntryDS cm = DataSources.find("colors");

    if (cp == DataSources.end()) {
      return;
    }

    GLfloat *surfdata = cp->second->get_data();
    GLfloat *colordata = NULL;


    if (cp->second->get_num_dimensions() != 2) {
      luaL_error(__lua_state,
                 "data source 'control_points' must have dimension 2");
    }
    if (cp->second->get_num_components() != 3) {
      luaL_error(__lua_state,
                 "data source 'control_points' must provide 3 components "
                 "(x,y,z)");
    }

    if (cm != DataSources.end()) {
      colordata = cm->second->get_data();
      if (cm->second->get_num_components() != 4) {
        luaL_error(__lua_state,
                   "data source 'colors' must provide 4 components (r,g,b,a)");
      }
      if (cm->second->get_num_dimensions() != 2) {
        luaL_error(__lua_state,
                   "data source 'colors' must be a 2d array of colors");
      }
    }

    int Nu = cp->second->get_num_points(0);
    int Nv = cp->second->get_num_points(1);

    GLfloat *knots_u = (GLfloat*) malloc((Nu + order)*sizeof(GLfloat));
    GLfloat *knots_v = (GLfloat*) malloc((Nv + order)*sizeof(GLfloat));

    for (int i=0; i<Nu+order; ++i) knots_u[i] = i;
    for (int i=0; i<Nv+order; ++i) knots_v[i] = i;

    const int su = Nv;
    const int sv = 1;

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

    if (colordata) {
      gluNurbsSurface(theNurb,
                      Nu + order, knots_u, Nv + order, knots_v,
                      4*su, 4*sv, colordata,
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



class PointsEnsemble : public DrawableObject
{
private:
  GLuint texture;

public:
  PointsEnsemble()
  {
    gl_modes.push_back(GL_BLEND);
    gl_modes.push_back(GL_POINT_SPRITE);
    gl_modes.push_back(GL_TEXTURE_2D);

    Orientation[0] = -90.0;
    glGenTextures(1, &texture);
  }
  ~PointsEnsemble()
  {
    glDeleteTextures(1, &texture);
  }

private:
  void load_tex_data()
  {
    int N = 64;
    double del = 2.0 / N;
    unsigned char *data = new unsigned char[N*N];

    for (int i=0; i<N; ++i) {
      for (int j=0; j<N; ++j) {
        int offs = i + j*N;
        double x = (i - N/2) * del;
        double y = (j - N/2) * del;
        double a = 1.0 - hypot(x, y);
        data[offs] = 255 * Alpha * (a > 0.0 ? a : 0.0);
      }
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, N, N, 0, GL_ALPHA,
                 GL_UNSIGNED_BYTE, data);

    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    delete [] data;
  }
  void draw_local()
  {
    EntryDS cp = DataSources.find("control_points");
    EntryDS cm = DataSources.find("colors");

    if (cp == DataSources.end()) {
      return;
    }

    GLfloat *pointdata = cp->second->get_data();
    GLfloat *colordata = NULL;

    if (cp->second->get_num_dimensions() != 1) {
      luaL_error(__lua_state,
                 "data source 'control_points' must be a list of points");
    }
    if (cp->second->get_num_components() != 3) {
      luaL_error(__lua_state,
                 "data source 'control_points' must provide 3 components "
                 "(x,y,z)");
    }

    if (cm != DataSources.end()) {
      colordata = cm->second->get_data();
      if (cm->second->get_num_components() != 4) {
        luaL_error(__lua_state,
                   "data source 'colors' must provide 4 components (r,g,b,a)");
      }
      if (cm->second->get_num_dimensions() != 1) {
        luaL_error(__lua_state,
                   "data source 'colors' must be a list of colors");
      }
    }

    int Np = cp->second->get_num_points(0);
    GLuint *ind = (GLuint*) malloc(Np*sizeof(GLuint));
    for (int n=0; n<Np; ++n) ind[n] = n;

    load_tex_data();
    glPointSize(LineWidth);
    glEnableClientState(GL_VERTEX_ARRAY);
    if (colordata) glEnableClientState(GL_COLOR_ARRAY);
    if (colordata) glColorPointer(4, GL_FLOAT, 4*sizeof(GLfloat), colordata);
    glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), pointdata);
    glDrawElements(GL_POINTS, Np, GL_UNSIGNED_INT, ind);
    glDisableClientState(GL_VERTEX_ARRAY);
    if (colordata) glDisableClientState(GL_COLOR_ARRAY);

    free(ind);
  }
} ;





extern "C" int luaopen_luview(lua_State *L)
{
  lua_newtable(L);

  LuaCppObject::Init(L);
  LuaCppObject::Register<Window>(L);
  LuaCppObject::Register<BoundingBox>(L);
  LuaCppObject::Register<SurfaceNURBS>(L);
  LuaCppObject::Register<PointsEnsemble>(L);

  LuaCppObject::Register<GridSource2D>(L);
  LuaCppObject::Register<PointsSource>(L);
  LuaCppObject::Register<FunctionMapping>(L);
  LuaCppObject::Register<Tesselation3D>(L);
  LuaCppObject::Register<ShaderExample>(L);

  return 1;
}

