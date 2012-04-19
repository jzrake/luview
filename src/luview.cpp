

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




class DrawableObject : public LuaCppObject
{
public:
  virtual void draw() = 0;
protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name)
  {
    AttributeMap attr;
    attr["draw"] = _draw_;
    RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
  }
  static int _draw_(lua_State *L)
  {
    DrawableObject *self = checkarg<DrawableObject>(L, 1);
    glPushMatrix();
    self->draw();
    glPopMatrix();
    return 0;
  }
} ;


class Window : public LuaCppObject
{
private:
  double WindowWidth, WindowHeight;
  static double RotationAngleX, RotationAngleY;

public:
  Window() : WindowWidth(1024),
	     WindowHeight(768) { }
  virtual ~Window() { }

private:
  void start_window()
  {
    double ClearColor[3] = { 0.025, 0.050, 0.025 };

    glfwInit();
    glfwOpenWindow(WindowWidth, WindowHeight, 0,0,0,0,0,0, GLFW_WINDOW);

    glClearColor(ClearColor[0], ClearColor[1], ClearColor[2], 0.0);
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
    this->start_window();

    while (1) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glLoadIdentity();

      glTranslated(0,0,-1.4);
      glRotated(RotationAngleX, 1, 0, 0);
      glRotated(RotationAngleY, 0, 1, 0);

      for (std::vector<DrawableObject*>::iterator actor=actors.begin();
           actor!=actors.end(); ++actor) {
        (*actor)->draw();
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
    switch (key) {
    case GLFW_KEY_RIGHT : RotationAngleY += 3; break;
    case GLFW_KEY_LEFT  : RotationAngleY -= 3; break;
      
    case GLFW_KEY_DOWN  : RotationAngleX += 3; break;
    case GLFW_KEY_UP    : RotationAngleX -= 3; break;
    }
    printf("received key: %d\n", key);
  }
  static void CharacterInput(int key, int state)
  {
    //    char keystr[1] = { key };
    //    if (state != GLFW_PRESS) return;
  }



protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name)
  {
    AttributeMap attr;
    attr["render_scene"] = _render_scene_;
    RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
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
double Window::RotationAngleX = 9.0;
double Window::RotationAngleY = 0.0;



class BoundingBox : public DrawableObject
{
public:
  void draw()
  {
    glColor3d(0.8, 0.7, 0.4);
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

