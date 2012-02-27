

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "numarray.h"
#include "image.h"
#include "lunum.h"
#include "luview.h"
#include "GL/glfw.h"


static int luaC_Init(lua_State *L);
static int luaC_Terminate(lua_State *L);
static int luaC_OpenWindow(lua_State *L);
static int luaC_RedrawScene(lua_State *L);
static int luaC_BoundingBoxArtist(lua_State *L);
static int luaC_PointsListArtist(lua_State *L);
static int luaC_SphereArtist(lua_State *L);
static int luaC_ColormapFilter(lua_State *L);

static void KeyboardInput(int key, int state);
static void CharacterInput(int key, int state);
static double modulate(double a, double b);

static int Autoplay     = 0;
static int WindowWidth  = 1024;
static int WindowHeight = 768;
static lua_State *LuaState = NULL;


int luaopen_luview(lua_State *L)
{
  printf("loading luview...\n");

  luaL_Reg luview_api[] = { { "Init"       , luaC_Init },
                            { "Terminate"   , luaC_Terminate },
                            { "OpenWindow"  , luaC_OpenWindow },
                            { "RedrawScene" , luaC_RedrawScene },
                            { "BoundingBoxArtist", luaC_BoundingBoxArtist },
                            { "PointsListArtist" , luaC_PointsListArtist },
                            { "SphereArtist"     , luaC_SphereArtist },
                            { "ColormapFilter" , luaC_ColormapFilter },
                            { NULL, NULL} };

  lua_newtable(L);
  luaL_setfuncs(L, luview_api, 0);
  lv_register_types(L);

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
  double ClearColor[3] = { 0.025, 0.050, 0.025 };

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
// @inputs: (1) Scene := { Camera={}, Keyboard={}, Actors={ Artist={}, Traits={}, } }
// @return: nil
{
  LuaState = L;

  glfwSetKeyCallback(KeyboardInput);
  glfwSetCharCallback(CharacterInput);

  glfwEnable(GLFW_STICKY_KEYS);
  glfwEnable(GLFW_KEY_REPEAT);

  while (1) {

    lua_getfield(L, 1, "Camera");
    struct LuviewTraits C = *luview_totraits(L, 2);
    lua_pop(L, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslated(-C.Position[0], -C.Position[1], -C.Position[2]);
    glScaled(C.Scale, C.Scale, C.Scale);

    glRotated(C.Orientation[0], 1, 0, 0);
    glRotated(C.Orientation[1], 0, 1, 0);
    glRotated(C.Orientation[2], 0, 0, 1);

    lua_getfield(L, 1, "Actors");
    int nactors = lua_rawlen(L, -1);

    for (int nactor=1; nactor<=nactors; ++nactor) {

      lua_rawgeti(L, -1, nactor);
      int actpos = lua_gettop(L);


      lua_getfield(L, actpos, "Traits");
      struct LuviewTraits T = *luview_totraits(L, -1);
      lua_pop(L, 1);

      if (T.IsVisible) {

        lua_getfield(L, actpos, "Artist");
        lua_getfield(L, actpos, "Traits");
        lua_getfield(L, actpos, "DataSource");

        int err = lua_pcall(L, 2, 0, 0);
        if (err) {
          luaL_error(L, lua_tostring(L, -1));
        }
      }
      lua_pop(L, 1); // pop the nth Actor
    }
    lua_pop(L, 1); // pop the Actors list

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
// -----------------------------------------------------------------------------
// @inputs: (1) Traits
// @return: nil
// -----------------------------------------------------------------------------
{
  luaL_checktype(L, 1, LUA_TTABLE);
  struct LuviewTraits T = *luview_totraits(L, 1);

  if (T.HasFocus) T.LineWidth *= modulate(1.0,5.0);

  glPushMatrix();
  glColor3dv(T.Color);
  glLineWidth(T.LineWidth);

  glScaled(T.Scale, T.Scale, T.Scale);
  glTranslated(T.Position[0], T.Position[1], T.Position[2]);
  glRotated(T.Orientation[0], 1, 0, 0);
  glRotated(T.Orientation[1], 0, 1, 0);
  glRotated(T.Orientation[2], 0, 0, 1);

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
  glPopMatrix();

  return 0;
}

int luaC_PointsListArtist(lua_State *L)
// -----------------------------------------------------------------------------
// @inputs: (1) Traits (2) DataSource
// @return: nil
// -----------------------------------------------------------------------------
{
  luaL_checktype(L, 1, LUA_TTABLE);
  struct LuviewTraits T = *luview_totraits(L, 1);

  lua_getfield(L, 2, "Positions");
  struct Array *A = lunum_checkarray1(L, -1);

  lua_getfield(L, 2, "Colors");
  struct Array *B = lunum_checkarray1(L, -1);

  double *positions = (double*) A->data;
  double *colors = (double*) B->data;

  glPushMatrix();
  glColor3dv(T.Color);
  glLineWidth(T.LineWidth);

  T.Scale *= 1.0 + T.HasFocus*modulate(-0.001,0.001);

  glScaled(T.Scale, T.Scale, T.Scale);
  glTranslated(T.Position[0], T.Position[1], T.Position[2]);
  glRotated(T.Orientation[0], 1, 0, 0);
  glRotated(T.Orientation[1], 0, 1, 0);
  glRotated(T.Orientation[2], 0, 0, 1);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glPointSize(T.LineWidth);
  glBegin(GL_POINTS);

  for (int i=0; i<A->shape[0]; ++i) {
    glColor4dv(colors + 4*i);
    glVertex3dv(positions + 3*i);
  }

  glEnd();
  glDisable(GL_BLEND);
  glPopMatrix();
  return 0;
}

int luaC_SphereArtist(lua_State *L)
{
  luaL_checktype(L, 1, LUA_TTABLE);
  struct LuviewTraits T = *luview_totraits(L, 1);

  glPushMatrix();
  glColor3dv(T.Color);
  glLineWidth(T.LineWidth);

  T.Scale *= 1.0 + T.HasFocus*modulate(-0.001,0.001);

  glScaled(T.Scale, T.Scale, T.Scale);
  glTranslated(T.Position[0], T.Position[1], T.Position[2]);
  glRotated(T.Orientation[0], 1, 0, 0);
  glRotated(T.Orientation[1], 0, 1, 0);
  glRotated(T.Orientation[2], 0, 0, 1);


  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);


  GLUquadric *q = gluNewQuadric();
  gluSphere(q, 2.0, 100, 100);
  gluDeleteQuadric(q);
  glPopMatrix();

  return 0;
}

int luaC_ColormapFilter(lua_State *L)
// -----------------------------------------------------------------------------
// @inputs: (1) N-element array of data values
//          (2) color-map index [0-6]
//          (3) transparency mode [0.0, 1.0] or 'L' (linear) or '1', '2', '3' (use cmap)
//
// @return: (N x 4) array of color-mapped doubles.
// -----------------------------------------------------------------------------
{
  struct Array *A = lunum_checkarray1(L, 1);
  struct Array  B = array_new_zeros(A->size * 4, ARRAY_TYPE_DOUBLE);
  const int cmap_ind = luaL_checkinteger(L, 2);

  int alpha_mode=0, ind=0;
  double alpha_val=1.0;

  if (lua_type(L, 3) == LUA_TSTRING) {
    switch (lua_tostring(L, 3)[0]) {
    case 'L': alpha_mode = 2; break;
    case '1': alpha_mode = 3; ind = 0;
    case '2': alpha_mode = 3; ind = 1;
    case '3': alpha_mode = 3; ind = 2;
    }
  }
  else if (lua_type(L, 3) == LUA_TNUMBER) {
    alpha_mode = 1;
    alpha_val = lua_tonumber(L, 3);
  }
  else {
    alpha_mode = 0;
    alpha_val = 1.0;
  }

  const float *cmap = luview_get_colormap(cmap_ind);
  const double *in = (double*) A->data;
  double *out = (double*) B.data;

  double smax=-1e16, smin=1e16;

  for (int m=0; m<A->size; ++m) {
    if (in[m] < smin) smin = in[m];
    if (in[m] > smax) smax = in[m];
  }
  for (int m=0; m<A->size; ++m) {
    const int cm = 255.0 * (in[m] - smin) / (smax - smin);
    out[4*m + 0] = cmap[3*cm + 0];
    out[4*m + 1] = cmap[3*cm + 1];
    out[4*m + 2] = cmap[3*cm + 2];
    if (alpha_mode == 2) {
      out[4*m + 3] = cm/255.0;
    }
    else if (alpha_mode == 3) {
      out[4*m + 3] = cmap[3*cm + ind];
    }
    else {
      out[4*m + 3] = alpha_val;
    }
  }
  int shape[2] = { A->size, 4 };
  array_resize(&B, shape, 2);
  lunum_pusharray1(L, &B);
  return 1;
}



void KeyboardInput(int key, int state)
{
  if (state != GLFW_PRESS) return;

  lua_getfield(LuaState, 1, "Keyboard");
  lua_pushvalue(LuaState, 1); // Scene
  lua_pushnumber(LuaState, key);
  lua_pushnumber(LuaState, state);
  lua_call(LuaState, 3, 0);
}

void CharacterInput(int key, int state)
{
  char keystr[1] = { key };
  if (state != GLFW_PRESS) return;

  lua_getfield(LuaState, 1, "Keyboard");
  lua_pushvalue(LuaState, 1); // Scene
  lua_pushlstring(LuaState, keystr, 1);
  lua_pushnumber(LuaState, state);
  lua_call(LuaState, 3, 0);
}



double modulate(double a, double b)
{
  const double w = 2*M_PI;
  const double t = (double)clock() / CLOCKS_PER_SEC;
  return a + (b-a) * sin(w*t);
}



/*



  GLfloat LightPosition        [] = {0.0, 0.0, 1.0, 1.0};
  GLfloat redDiffuseMaterial   [] = {1.0, 0.0, 0.0}; // set the material to red
  GLfloat whiteSpecularMaterial[] = {1.0, 1.0, 1.0}; // set the material to white
  GLfloat greenEmissiveMaterial[] = {0.0, 1.0, 0.0}; // set the material to green
  GLfloat whiteSpecularLight   [] = {1.0, 1.0, 1.0}; // set the light specular to white
  GLfloat blackAmbientLight    [] = {0.0, 0.0, 0.0}; // set the light ambient to black
  GLfloat whiteDiffuseLight    [] = {1.0, 1.0, 1.0}; // set the diffuse light to white
  GLfloat blankMaterial        [] = {0.0, 0.0, 0.0}; // set the diffuse light to white
  GLfloat mShininess           [] = {128.0};         // set the shininess of the material


  // Set up lighting
  // ---------------------------------------------------------------------------
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);

  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);

  glLightfv(GL_LIGHT1, GL_SPECULAR, whiteSpecularLight);
  glLightfv(GL_LIGHT1, GL_AMBIENT, blackAmbientLight);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, whiteDiffuseLight);
  glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);

  glEnable(GL_LIGHT1);

*/
