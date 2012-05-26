

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

#include "luview.hpp"
#include "pyplotcm.h"

extern "C" {
#define LUNUM_API_NOCOMPLEX
#include "numarray.h"
#include "lunum.h"
#include "GL/glfw.h"
}
#include "glInfo.hpp"



GpuInformation::LuaInstanceMethod
GpuInformation::__getattr__(std::string &method_name)
{
  AttributeMap attr;
  attr["print"] = _print_;
  attr["ext_supported"] = _ext_supported_;
  RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
}

int GpuInformation::_ext_supported_(lua_State *L)
{
  const char *ext = luaL_checkstring(L, 2);
  glInfo info;
  info.getInfo();
  lua_pushboolean(L, info.isExtensionSupported(ext));
  return 1;
}
int GpuInformation::_print_(lua_State *L)
{
  glInfo info;
  info.getInfo();
  info.printSelf();
  return 0;
}


CallbackFunction *CallbackFunction::create_from_stack(lua_State *L, int pos)
{
  if (lua_type(L, pos) == LUA_TUSERDATA) {
    return checkarg<CallbackFunction>(L, pos);
  }
  else if (lua_type(L, pos) == LUA_TNIL) {
    return NULL;
  }
  else {
    LuaFunction *f = create<LuaFunction>(L);
    f->hold(pos, "lua_callback");
    return f;
  }
}
int CallbackFunction::_call()
{
  lua_State *L = __lua_state;
  int narg = lua_gettop(L);
  std::vector<double> args;
  for (int n=1; n<=narg; ++n) {
    args.push_back(lua_tonumber(L, n));
  }
  std::vector<double> res = call(args);
  for (unsigned int n=0; n<res.size(); ++n) {
    lua_pushnumber(L, res[n]);
  }
  return res.size();
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
  int nstart = lua_gettop(L);
  retrieve("lua_callback");


  for (int i=0; i<narg; ++i) {
    lua_pushnumber(L, x[i]);
  }
  if (lua_pcall(L, narg, LUA_MULTRET, 0) != 0) {
    luaL_error(L, lua_tostring(L, -1));
  }
  int nret = lua_gettop(L) - nstart;
  for (int i=0; i<nret; ++i) {
    res.push_back(lua_tonumber(L, -1));
    lua_pop(L, 1);
  }
  reverse(res.begin(), res.end());
  return res;
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

  Alpha = 1.0;
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
    self->retrieve(val->second);
  }
  return 1;
}
int LuviewTraitedObject::_set_Callback_(lua_State *L)
{
  LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);
  const char *name = luaL_checkstring(L, 2);
  CallbackFunction *newcb = CallbackFunction::create_from_stack(L, 3);
  const char *help = luaL_optstring(L, 4, "");
  self->Callbacks[name] = self->replace(self->Callbacks[name], newcb);
  if (self->Callbacks[name] == NULL) {
    self->Callbacks.erase(self->Callbacks.find(name));
  }
  else {
    self->Callbacks[name]->set_message(help);
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
    self->retrieve(val->second);
  }
  return 1;
}
int LuviewTraitedObject::_set_DataSource_(lua_State *L)
{
  LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);
  const char *name = luaL_checkstring(L, 2);
  self->DataSources[name] = self->replace(self->DataSources[name], 3);
  if (self->DataSources[name] == NULL) {
    self->DataSources.erase(self->DataSources.find(name));
  }
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



DrawableObject::DrawableObject() : shader(NULL)
{

}
void DrawableObject::draw()
{
  for (unsigned int i=0; i<gl_modes.size(); ++i) {
    glEnable(gl_modes[i]);
  }
  glPushMatrix();
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
  if (shader) shader->activate();

  glTranslated(Position[0], Position[1], Position[2]);
  glScaled(Scale[0], Scale[1], Scale[2]);
  glRotated(Orientation[0], 1, 0, 0);
  glRotated(Orientation[1], 0, 1, 0);
  glRotated(Orientation[2], 0, 0, 1);

  glColor4d(Color[0], Color[1], Color[2], Alpha);
  glLineWidth(LineWidth);

  GLfloat mat_diff[] = { 0.9, 0.9, 0.9, Alpha };
  GLfloat mat_spec[] = { 1.0, 1.0, 1.0, Alpha };
  GLfloat mat_shin[] = { 128.0 };

  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diff);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shin);

  draw_local();
  if (shader) shader->deactivate();

  glPopClientAttrib();
  glPopAttrib();
  glPopMatrix();

  for (unsigned int i=0; i<gl_modes.size(); ++i) {
    glDisable(gl_modes[i]);
  }
}



DrawableObject::LuaInstanceMethod DrawableObject::__getattr__
(std::string &method_name)
{
  AttributeMap attr;
  attr["get_shader"] = _get_shader_;
  attr["set_shader"] = _set_shader_;
  RETURN_ATTR_OR_CALL_SUPER(LuviewTraitedObject);
}
int DrawableObject::_get_shader_(lua_State *L)
{
  DrawableObject *self = checkarg<DrawableObject>(L, 1);
  self->retrieve(self->shader);
  return 1;
}
int DrawableObject::_set_shader_(lua_State *L)
{
  DrawableObject *self = checkarg<DrawableObject>(L, 1);
  self->shader = self->replace(self->shader, 2);
  return 0;
}


class Window : public LuviewTraitedObject
{
private:
  int WindowWidth, WindowHeight;
  int character_input;
  static Window *CurrentWindow;
  bool first_frame;

public:
  Window() : WindowWidth(1024),
             WindowHeight(768), character_input(0), first_frame(true)
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
    glfwOpenWindow(WindowWidth, WindowHeight, 5, 6, 5, 0, 8, 0, GLFW_WINDOW);
    glfwSetWindowTitle("Mythos science visualizer");

    glClearDepth(1.0);
    glDepthFunc(GL_LESS);

    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)WindowWidth/WindowHeight, 0.01, 200.0);
    glMatrixMode(GL_MODELVIEW);

    glfwSetWindowSizeCallback(Reshape);
    glfwSetKeyCallback(KeyboardInput);
    glfwSetCharCallback(CharacterInput);
    glfwEnable(GLFW_STICKY_KEYS);
    glfwEnable(GLFW_KEY_REPEAT);
  }

  const char *render_scene(std::vector<DrawableObject*> &actors)
  {
    if (first_frame) {
      EntryCB cb = Callbacks.begin();
      while (cb != Callbacks.end()) {
	if (cb->first != "idle") {
	  std::cout<<cb->first<<": "<<cb->second->get_message()<<std::endl;
	}
	++cb;
      }
      first_frame = false;
    }

    character_input = ' ';
    CurrentWindow = this;

    glClearColor(Color[0], Color[1], Color[2], 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    GLfloat light_ambient [] = { 0.4, 0.4, 0.4, 1.0 };
    GLfloat light_diffuse [] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

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

    CurrentWindow->exec_callback("idle");
    return "continue";
  }

private:
  void TakeScreenshot(const char *basenm)
  {
    static int nframe = 0;
    static char fname_ppm[512];
    static char fname_png[512];
    static char convert_cmd[512];
    sprintf(fname_ppm, "%s-%04d.ppm", basenm, nframe);
    sprintf(fname_png, "%s-%04d.png", basenm, nframe);
    sprintf(convert_cmd, "convert %s %s; rm %s", fname_ppm, fname_png, fname_ppm);
    printf("writing a screenshot to %s\n", fname_png);
    ++nframe;

    int dimx = WindowWidth;
    int dimy = WindowHeight;
    
    size_t imsize = 3*dimx*dimy;
    char *pixels = (char*) malloc(imsize*sizeof(char));
    glReadPixels(0, 0, dimx, dimy, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    
    FILE *fp = fopen(fname_ppm, "wb");
    if (fp == NULL) {
      luaL_error(__lua_state, "could not create image with filename %s", fname_ppm);
    }
    fprintf(fp, "P6\n%d %d\n255\n", dimx, dimy);
    fwrite(pixels, sizeof(char), imsize, fp);
    fclose(fp);
    system(convert_cmd);
    free(pixels);
  }
  int exec_callback(const char *key)
  {
    EntryCB cb = Callbacks.find(key);
    if (cb != Callbacks.end()) {
      cb->second->call();
      return 1;
    }
    else {
      return 0;
    }
  }
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

    if (CurrentWindow->exec_callback((const char*)&key)) return;

    switch (key) {
    case 'p':
      CurrentWindow->TakeScreenshot("screenshot");
      break;

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
  static void Reshape(int newWidth, int newHeight)
  {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)newWidth/newHeight, 0.01, 200.0);
    glMatrixMode(GL_MODELVIEW);
  }

protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name)
  {
    AttributeMap attr;
    attr["render_scene"] = _render_scene_;
    attr["print_screen"] = _print_screen_;
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
  static int _print_screen_(lua_State *L)
  {
    Window *self = checkarg<Window>(L, 1);
    const char *basenm = luaL_optstring(L, 2, "screenshot");
    self->TakeScreenshot(basenm);
    return 0;
  }
} ;
Window *Window::CurrentWindow;




extern "C" int luaopen_luview(lua_State *L)
{
  lua_newtable(L);

  LuaCppObject::Init(L);
  LuaCppObject::Register<GpuInformation>(L);
  LuaCppObject::Register<Window>(L);

  LuaCppObject::Register<DataSource>(L);
  LuaCppObject::Register<GridSource2D>(L);
  LuaCppObject::Register<ParametricVertexSource3D>(L);
  LuaCppObject::Register<BoundingBox>(L);
  LuaCppObject::Register<ShaderProgram>(L);
  LuaCppObject::Register<ImagePlane>(L);
  LuaCppObject::Register<MatplotlibColormaps>(L);

  LuaCppObject::Register<Tesselation3D>(L);
  LuaCppObject::Register<SegmentsEnsemble>(L);
  LuaCppObject::Register<ParametricSurface>(L);
  LuaCppObject::Register<TrianglesEnsemble>(L);

  return 1;
}
