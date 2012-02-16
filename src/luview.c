

#include "lualib.h"
#include "lauxlib.h"
#include "GL/glfw.h"


static int luaC_Init(lua_State *L);
static int luaC_Terminate(lua_State *L);
static int luaC_OpenWindow(lua_State *L);
static int luaC_RedrawScene(lua_State *L);
static int luaC_BoundingBoxArtist(lua_State *L);

static void KeyboardInput(int key, int state);
static void CharacterInput(int key, int state);


static int luaC_bounding_box(lua_State *L);


static int Autoplay     = 0;
static int WindowWidth  = 1024;
static int WindowHeight = 768;
static float ZoomFactor = 1.0;


static lua_State *LuaState;


struct LuviewCamera
{
  double position[3];
  double angle[2];
} ;
struct LuviewTraits
{
  double Position[3];
  double Color[3];
  double Scale;
  double LineWidth;
} ;
struct LuviewCamera luview_tocamera(lua_State *L, int pos);
struct LuviewTraits luview_totraits(lua_State *L, int pos);



int luaopen_luview(lua_State *L)
{
  printf("loading luview...\n");

  luaL_Reg luview_api[] = { { "Init"       , luaC_Init },
			   { "Terminate"  , luaC_Terminate },
			   { "OpenWindow" , luaC_OpenWindow },
			   { "RedrawScene", luaC_RedrawScene },
			   { "BoundingBoxArtist", luaC_BoundingBoxArtist },
			   { NULL, NULL} };

  lua_newtable(L);
  luaL_setfuncs(L, luview_api, 0);


  lua_pushnumber(L, GLFW_KEY_LEFT ); lua_setfield(L, -2, "KEY_LEFT" );
  lua_pushnumber(L, GLFW_KEY_RIGHT); lua_setfield(L, -2, "KEY_RIGHT");
  lua_pushnumber(L, GLFW_KEY_UP   ); lua_setfield(L, -2, "KEY_UP"   );
  lua_pushnumber(L, GLFW_KEY_DOWN ); lua_setfield(L, -2, "KEY_DOWN" );

  lua_setglobal(L, "luview");
  lua_getglobal(L, "luview");
  return 1;
}

int luaC_Init(lua_State *L)
{
  glfwInit();
  return 0;
}
int luaC_Terminate(lua_State *L)
{
  glfwTerminate();
  return 0;
}
int luaC_OpenWindow(lua_State *L)
{
  double ClearColor[3] = { 0.1, 0.2, 0.2 };

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

  return 0;
}

int luaC_RedrawScene(lua_State *L)
{
  LuaState = L;

  glfwSetKeyCallback(KeyboardInput);
  glfwSetCharCallback(CharacterInput);

  glfwEnable(GLFW_STICKY_KEYS);
  glfwEnable(GLFW_KEY_REPEAT);

  while (1) {

    lua_getfield(L, 1, "Camera");
    struct LuviewCamera C = luview_tocamera(L, 2);
    lua_pop(L, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslated(-C.position[0], -C.position[1], -C.position[2]);
    glScalef(ZoomFactor, ZoomFactor, ZoomFactor);

    glRotatef(C.angle[0], 1, 0, 0);
    glRotatef(C.angle[1], 0, 1, 0);

    lua_getfield(L, 1, "Draw");
    lua_call(L, 0, 0);

    glFlush();
    glfwSwapBuffers();

    if (glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED)) {
      glfwCloseWindow();
      break;
    }

    if (Autoplay || glfwGetKey(GLFW_KEY_SPACE)) {
      break;
    }
  }
  return 0;
}

int luaC_BoundingBoxArtist(lua_State *L)
{
  lua_newtable(L);

  lua_pushcfunction(L, luaC_bounding_box);
  lua_setfield(L, 1, "Draw");

  return 1;
}

int luaC_bounding_box(lua_State *L)
// @inputs: (1) Artist (2) Traits
// @return: nil
{
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_checktype(L, 2, LUA_TTABLE);

  struct LuviewTraits T = luview_totraits(L, 2);

  const double L0  = T.Scale;
  const double Lx0 = -L0;
  const double Lx1 =  L0;
  const double Ly0 = -L0;
  const double Ly1 =  L0;
  const double Lz0 = -L0;
  const double Lz1 =  L0;

  // Drawing a bounding box
  // ---------------------------------------------------------------------------
  glColor3dv(T.Color);
  glLineWidth(T.LineWidth);

  glBegin(GL_LINES);

  // x-edges
  glVertex3f(Lx0, Ly0, Lz0); glVertex3f(Lx1, Ly0, Lz0);
  glVertex3f(Lx0, Ly0, Lz1); glVertex3f(Lx1, Ly0, Lz1);
  glVertex3f(Lx0, Ly1, Lz0); glVertex3f(Lx1, Ly1, Lz0);
  glVertex3f(Lx0, Ly1, Lz1); glVertex3f(Lx1, Ly1, Lz1);

  // y-edges
  glVertex3f(Lx0, Ly0, Lz0); glVertex3f(Lx0, Ly1, Lz0);
  glVertex3f(Lx1, Ly0, Lz0); glVertex3f(Lx1, Ly1, Lz0);
  glVertex3f(Lx0, Ly0, Lz1); glVertex3f(Lx0, Ly1, Lz1);
  glVertex3f(Lx1, Ly0, Lz1); glVertex3f(Lx1, Ly1, Lz1);

  // z-edges
  glVertex3f(Lx0, Ly0, Lz0); glVertex3f(Lx0, Ly0, Lz1);
  glVertex3f(Lx0, Ly1, Lz0); glVertex3f(Lx0, Ly1, Lz1);
  glVertex3f(Lx1, Ly0, Lz0); glVertex3f(Lx1, Ly0, Lz1);
  glVertex3f(Lx1, Ly1, Lz0); glVertex3f(Lx1, Ly1, Lz1);

  glEnd();

  return 0;
}





void KeyboardInput(int key, int state)
{
  if (state != GLFW_PRESS) return;

  lua_getfield(LuaState, 1, "Keyboard");
  lua_pushnumber(LuaState, key);
  lua_pushnumber(LuaState, state);
  lua_call(LuaState, 2, 0);
}

void CharacterInput(int key, int state)
{
  switch (key) {

  case 'z':
    ZoomFactor /= 1.1;
    break;

  case 'Z':
    ZoomFactor *= 1.1;
    break;

  case 'p':
    Autoplay ^= 1;
    break;

  default:
    break;
  }
}





struct LuviewTraits luview_totraits(lua_State *L, int pos)
{
  struct LuviewTraits T;

  lua_getfield(L, pos, "Position");
  lua_rawgeti(L, pos+1, 1); T.Position[0] = lua_tonumber(L, -1); lua_pop(L, 1);
  lua_rawgeti(L, pos+1, 2); T.Position[1] = lua_tonumber(L, -1); lua_pop(L, 1);
  lua_rawgeti(L, pos+1, 3); T.Position[2] = lua_tonumber(L, -1); lua_pop(L, 1);
  lua_pop(L, 1);

  lua_getfield(L, pos, "Color");
  lua_rawgeti(L, pos+1, 1); T.Color[0] = lua_tonumber(L, -1); lua_pop(L, 1);
  lua_rawgeti(L, pos+1, 2); T.Color[1] = lua_tonumber(L, -1); lua_pop(L, 1);
  lua_rawgeti(L, pos+1, 3); T.Color[2] = lua_tonumber(L, -1); lua_pop(L, 1);
  lua_pop(L, 1);

  lua_getfield(L, pos, "Scale"    ); T.Scale     = lua_tonumber(L, -1); lua_pop(L, 1);
  lua_getfield(L, pos, "LineWidth"); T.LineWidth = lua_tonumber(L, -1); lua_pop(L, 1);

  return T;
}

struct LuviewCamera luview_tocamera(lua_State *L, int pos)
{
  struct LuviewCamera C;

  lua_getfield(L, pos, "position");
  lua_rawgeti(L, pos+1, 1); C.position[0] = lua_tonumber(L, -1); lua_pop(L, 1);
  lua_rawgeti(L, pos+1, 2); C.position[1] = lua_tonumber(L, -1); lua_pop(L, 1);
  lua_rawgeti(L, pos+1, 3); C.position[2] = lua_tonumber(L, -1); lua_pop(L, 1);
  lua_pop(L, 1);

  lua_getfield(L, pos, "angle");
  lua_rawgeti(L, pos+1, 1); C.angle[0] = lua_tonumber(L, -1); lua_pop(L, 1);
  lua_rawgeti(L, pos+1, 2); C.angle[1] = lua_tonumber(L, -1); lua_pop(L, 1);
  lua_pop(L, 1);

  return C;
}
