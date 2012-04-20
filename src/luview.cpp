

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





class ExampleSimpleEvaluator : public DrawableObject
{
public:
  ExampleSimpleEvaluator()
  {
    gl_modes.push_back(GL_MAP2_VERTEX_3);
    gl_modes.push_back(GL_AUTO_NORMAL);
    gl_modes.push_back(GL_NORMALIZE);
  }

  void draw_local()
  {
    const int Nx = 8;
    const int Ny = 8;

    double Lx0=-0.5, Lx1=0.5, Ly0=-0.5, Ly1=0.5;

    const int sx = Ny;
    const int sy = 1;

    const double dx = (Lx1 - Lx0) / (Nx - 1);
    const double dy = (Ly1 - Ly0) / (Ny - 1);

    GLfloat *surfdata = (GLfloat*) malloc(Nx*Ny*3*sizeof(GLfloat));

    for (int i=0; i<Nx; ++i) {
      for (int j=0; j<Ny; ++j) {

        const double x = Lx0 + i*dx;
        const double y = Ly0 + j*dy;
        const double z = x*x + y*y;
        const int m = i*sx + j*sy;

        surfdata[3*m + 0] = x;
        surfdata[3*m + 1] = z;
        surfdata[3*m + 2] = y;
      }
    }

    for (int i=1; i<Nx-1; ++i) {
      for (int j=1; j<Ny-1; ++j) {

        const double x = Lx0 + i*dx;
        const double y = Ly0 + j*dy;
        const GLfloat *s = &surfdata[3*((i-1)*sx + (j-1)*sy)];

        glMap2f(GL_MAP2_VERTEX_3, x-dx, x+dx, 3*sx, 3, y-dy, y+dy, 3*sy, 3, s);
        glMapGrid2f(4, x-dx/2, x+dx/2, 4, y-dy/2, y+dy/2);
        glEvalMesh2(GL_LINE, 0, 4, 0, 4);
      }
    }
    free(surfdata);
  }
} ;


class ExampleSimpleVBO : public DrawableObject
{
public:
  void draw_local()
  {
    GLuint vbo,ibo;
    GLfloat verts[8][3] = {{0.0, 0.0, 0.0},
                           {0.0, 0.0, 0.1},
                           {0.0, 0.1, 0.0},
                           {0.0, 0.1, 0.1},
                           {0.1, 0.0, 0.0},
                           {0.1, 0.0, 0.1},
                           {0.1, 0.1, 0.0},
                           {0.1, 0.1, 0.1}};
    GLubyte ind[8] = {0,3,6,9,12,15,18,21};

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), verts, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 8*sizeof(GLubyte), ind,
                 GL_STATIC_DRAW);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(GLfloat), 0);
    glDrawElements(GL_LINE_STRIP, 8, GL_UNSIGNED_BYTE, 0);
    glDisableClientState(GL_VERTEX_ARRAY);

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
  }
} ;


class ExampleSimpleNURBS : public DrawableObject
{
private:
  GLfloat ctlpoints[4][4][3];
  int showPoints;
  GLUnurbsObj *theNurb;

public:
  ExampleSimpleNURBS() : showPoints(1)
  {
    gl_modes.push_back(GL_LIGHTING);
    gl_modes.push_back(GL_LIGHT0);
    gl_modes.push_back(GL_AUTO_NORMAL);
    gl_modes.push_back(GL_NORMALIZE);

    init_surface();
  }
  ~ExampleSimpleNURBS() { }

private:
  void draw_local()
  {
    int i,j;

    GLfloat knots[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
    GLfloat mat_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 100.0 };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    theNurb = gluNewNurbsRenderer();
    gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
    gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);
    gluNurbsCallback(theNurb, GLU_ERROR, (GLvoid (*)()) nurbsError);

    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glScalef(0.1, 0.1, 0.1);

    gluBeginSurface(theNurb);
    gluNurbsSurface(theNurb,
                    8, knots, 8, knots,
                    4*3, 3, &ctlpoints[0][0][0],
                    4, 4, GL_MAP2_VERTEX_3);
    gluEndSurface(theNurb);

    if (showPoints) {
      glPointSize(5.0);
      glDisable(GL_LIGHTING);
      glColor3f(1.0, 1.0, 0.0);
      glBegin(GL_POINTS);

      for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
          glVertex3f(ctlpoints[i][j][0],
		     ctlpoints[i][j][1],
		     ctlpoints[i][j][2]);
        }
      }
      glEnd();
      glEnable(GL_LIGHTING);
    }

    gluDeleteNurbsRenderer(theNurb);
  }

  void init_surface()
  {
    int u, v;
    for (u=0; u<4; u++) {
      for (v=0; v<4; v++) {
        ctlpoints[u][v][0] = 2.0*((GLfloat)u - 1.5);
        ctlpoints[u][v][1] = 2.0*((GLfloat)v - 1.5);

        if ((u == 1 || u == 2) && (v == 1 || v == 2)) {
          ctlpoints[u][v][2] = 3.0;
	}
        else {
          ctlpoints[u][v][2] = -3.0;
	}
      }
    }
  }

  static void nurbsError(GLenum errorCode)
  {
    const GLubyte *estring;
    estring = gluErrorString(errorCode);
    fprintf(stderr, "Nurbs Error: %s\n", estring);
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
  LuaCppObject::Register<ExampleSimpleEvaluator>(L);
  LuaCppObject::Register<ExampleSimpleVBO>(L);
  LuaCppObject::Register<ExampleSimpleNURBS>(L);

  return 1;
}

