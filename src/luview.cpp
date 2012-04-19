

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

  int luaopen_luview(lua_State *L);
}



#define GETSET_TRAITS_D1(prop)						\
  static int _get_##prop##_(lua_State *L) {				\
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);	\
    lua_remove(L, 1);							\
    return __get_vec__(L, &self->prop, 1);				\
  }									\
  static int _set_##prop##_(lua_State *L) {				\
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);	\
    lua_remove(L, 1);							\
    return __set_vec__(L, &self->prop, 1);				\
  }									\

#define GETSET_TRAITS_D3(prop)						\
  static int _get_##prop##_(lua_State *L) {				\
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);	\
    lua_remove(L, 1);							\
    return __get_vec__(L, self->prop, 3);				\
  }									\
  static int _set_##prop##_(lua_State *L) {				\
    LuviewTraitedObject *self = checkarg<LuviewTraitedObject>(L, 1);	\
    lua_remove(L, 1);							\
    return __set_vec__(L, self->prop, 3);				\
  }									\


class LuviewTraitedObject : public LuaCppObject
{
protected:
  double Position[3];
  double Orientation[3];
  double Color[3];
  double Scale[3];
  double LineWidth;
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
    RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
  }
  GETSET_TRAITS_D3(Position);
  GETSET_TRAITS_D3(Orientation);
  GETSET_TRAITS_D3(Color);
  GETSET_TRAITS_D3(Scale);
  GETSET_TRAITS_D1(LineWidth);

  static int __get_vec__(lua_State *L, double *v, int n) {
    for (int i=0; i<n; ++i) lua_pushnumber(L, v[i]);
    return n;
  }
  static int __set_vec__(lua_State *L, double *v, int n) {
    for (int i=0; i<n; ++i) v[i] = luaL_checknumber(L, i+1);
    return 0;
  }
} ;


class DrawableObject : public LuviewTraitedObject
{
public:
  virtual void draw()
  {
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
    Color[0] = 0.12;
    Color[1] = 0.05;
    Color[2] = 0.02;

    Orientation[0] = 9.0;
    Orientation[1] = 0.0;
    Orientation[2] = 0.0;
  }
  virtual ~Window() { }

private:
  void start_window()
  {
    glfwInit();
    glfwOpenWindow(WindowWidth, WindowHeight, 0,0,0,0,0,0, GLFW_WINDOW);

    glClearColor(Color[0], Color[1], Color[2], 0.0);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float) WindowWidth / WindowHeight, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    glfwSetKeyCallback(KeyboardInput);
    glfwSetCharCallback(CharacterInput);
    glfwEnable(GLFW_STICKY_KEYS);
    glfwEnable(GLFW_KEY_REPEAT);
  }

  void render_scene(std::vector<DrawableObject*> &actors)
  {
    CurrentWindow = this;
    this->start_window();

    while (1) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glLoadIdentity();

      glTranslated(0,0,-1.4);
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
        break;
      }
    }
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

    self->render_scene(actors);
    return 0;
  }
} ;
Window *Window::CurrentWindow;


class BoundingBox : public DrawableObject
{
private:
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


int luaopen_luview(lua_State *L)
{
  printf("loading luview...\n");

  lua_newtable(L);

  LuaCppObject::Init(L);
  LuaCppObject::Register<Window>(L, -1);
  LuaCppObject::Register<BoundingBox>(L, -1);

  return 1;
}

