
#ifndef __LuviewObjects_HEADER__
#define __LuviewObjects_HEADER__


#include <vector>
#include "lua_object.hpp"

extern "C" {
#include "GL/glfw.h"
}

#define __DATASOURCE_MAXDIMS 3


// Forward declarations
// -----------------------------------------------------------------------------
class CallbackFunction;
class DataSource;
class PointsSource;
class ShaderProgram;
// -----------------------------------------------------------------------------

class DataSource : public LuaCppObject
{
protected:
  typedef std::map<std::string, DataSource*> DataSourceMap;

  CallbackFunction*  __cpu_transform;
  ShaderProgram*     __gpu_transform;
  DataSource*        __input_ds;
  DataSourceMap      __output_ds;
  GLfloat*           __cpu_data;
  GLuint*            __ind_data;
  GLuint             __texture_id;
  GLuint             __vbo_id;
  GLuint             __ibo_id;
  int                __texture_format; // luminance, alpha, rgba, etc
  GLenum             __texture_target; // e.g. GL_TEXTURE_1D inferred internally

  int __num_dimensions;
  int __num_indices;
  int __num_points[__DATASOURCE_MAXDIMS];

  // if true for component then map output into [0,1]
  bool __normalize;

  void __do_normalize();
  void __trigger_refresh();
  void __execute_gpu_transform();
  bool __ancestor_is_staged();
  bool __staged;

  virtual void __refresh_cpu() { } // re-compile data from sources into cpu buffer
  void __cp_gpu_to_cpu(); // copy data from texture memory to cpu buffer
  void __cp_cpu_to_gpu(); // copy data from cpu buffer to texture memory

public:
  DataSource();
  virtual ~DataSource(); // will free any non-null data buffers

  const GLfloat*  get_data();                // return the cpu data buffer
  const GLuint*   get_indices();             // return the list of indices
  GLuint          get_texture_id();          // return the texture id
  DataSource*     get_output(const char *n); // return an entry of __output_ds

  int get_size();
  int get_num_points(int d);
  int get_num_dimensions();
  int get_num_indices();
  int get_vbo() { return __vbo_id; }
  int get_ibo() { return __ibo_id; }

  void set_input(DataSource *inpt);
  void set_mode(const char *mode);
  /* sets the data buffer manually
     data -> __cpu_data (deep copy)
     np -> __num_points
     nd -> __num_dimensions */
  void set_data(const GLfloat *data, const int *np, int nd);

  /* sets the index buffer manually
     indices -> __ind_data
     ni -> __num_indices */
  void set_indices(const GLuint *indices, int ni);

  void check_num_dimensions(const char *name, int ndims);
  void check_num_points(const char *name, int npnts, int dim);
  void check_has_data(const char *name);
  void check_has_indices(const char *name);

  void become_texture();
  void compile();
protected:
  // ---------------------------------------------------------------------------
  // Lua API methods
  //
  // ---------------------------------------------------------------------------
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _get_output_(lua_State *L); // return a named DataSource object
  static int _set_normalize_(lua_State *L);
  static int _get_data_(lua_State *L); // read from a lunum array
  static int _set_data_(lua_State *L); // return a lunum array
  static int _get_mode_(lua_State *L);
  static int _set_mode_(lua_State *L);
  static int _get_input_(lua_State *L);
  static int _set_input_(lua_State *L);
  static int _get_transform_(lua_State *L);
  static int _set_transform_(lua_State *L);
  static int _get_program_(lua_State *L);
  static int _set_program_(lua_State *L);
  static int _compile_(lua_State *L);
} ;

class GridSource2D :  public DataSource
{
public:
  GridSource2D();
protected:
  int Nu, Nv;
  double u0, u1, v0, v1;
  virtual void __refresh_cpu();
protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _set_num_points_(lua_State *L);
  static int _set_u_range_(lua_State *L);
  static int _set_v_range_(lua_State *L);
} ;

class ParametricVertexSource3D :  public GridSource2D
{
public:
  ParametricVertexSource3D();
protected:
  void __refresh_cpu();
  void __init_lua_objects();
} ;

class CallbackFunction : public LuaCppObject
{
public:
  std::vector<double> call();
  std::vector<double> call(double u);
  std::vector<double> call(double u, double v);
  std::vector<double> call(double u, double v, double w);
  std::vector<double> call(std::vector<double> X);
  void set_message(const char *msg) { message = msg; }
  const char *get_message() { return message.c_str(); }
  static CallbackFunction *create_from_stack(lua_State *L, int pos);
private:
  std::string message;
  virtual std::vector<double> call_priv(double *x, int narg) = 0;
  int _call();
} ;

class LuaFunction : public CallbackFunction
{
private:
  virtual std::vector<double> call_priv(double *x, int narg);
} ;

class ColormapCollection : public DataSource
{
public:
  virtual void set_colormap(const char *name) { }
  virtual void next_colormap() { }
  virtual void prev_colormap() { }
protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _set_colormap_(lua_State *L);
  static int _next_colormap_(lua_State *L);
  static int _prev_colormap_(lua_State *L);
} ;

class MatplotlibColormaps : public ColormapCollection
{
public:
  MatplotlibColormaps();
  void set_colormap(const char *name);
  void next_colormap();
  void prev_colormap();
private:
  const float *cmap_data;
  int cmap_id;
  void __refresh_cpu();
} ;

class tetgenio;
class Tesselation3D : public DataSource
{
public:
  tetgenio *inp, *out;
  Tesselation3D();
  virtual ~Tesselation3D();
private:
  void __refresh_cpu();
protected:
  void __init_lua_objects();
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _load_node_(lua_State *L);
  static int _load_poly_(lua_State *L);
} ;




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
  double Orientation[4];
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

class GpuInformation : public LuaCppObject
{
protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _ext_supported_(lua_State *L);
  static int _print_(lua_State *L);
} ;

class ShaderProgram : public LuaCppObject
{
private:
  GLuint vert, frag, prog;
  GLint prev_prog;
public:
  ShaderProgram();
  virtual ~ShaderProgram();
  void set_uniform(const char *name, GLint value);
  void set_program(const char *vert_src, const char *frag_src);
  void unset_program();
  void activate();
  void deactivate();
private:
  void printShaderInfoLog(GLuint obj);
  void printProgramInfoLog(GLuint obj);
protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _set_program_(lua_State *L);
} ;

class DrawableObject : public LuviewTraitedObject
{
protected:
  std::vector<int> gl_modes;
  ShaderProgram *shader;
public:
  DrawableObject();
  virtual void draw();
protected:
  virtual void draw_local() = 0;
protected:
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _get_shader_(lua_State *L);
  static int _set_shader_(lua_State *L);
} ;

class VolumeRendering : public DrawableObject
{
public:
  VolumeRendering();
private:
  void draw_local();
} ;

class NbodySimulation : public LuaCppObject
{
public:
  NbodySimulation();
  virtual ~NbodySimulation();
  void advance();
private:
  struct MassiveParticle
  {
    int id;
    double m;
    double x[3], v[3], a[3];
  } ;
  int NumberOfParticles;
  double TimeStep;
  PointsSource *output_points;
  MassiveParticle *particles;
  void init_particles();
  void refresh_output();
  void ComputeForces(struct MassiveParticle *P0, int N);
  void ComputeForces2(struct MassiveParticle *P0, int N);
  void MoveParticlesFwE(struct MassiveParticle *P0, int N, double dt);
  void MoveParticlesRK2(struct MassiveParticle *P0, int N, double dt);
  void MoveParticlesRK4(struct MassiveParticle *P0, int N, double dt);
  double RandomDouble(double a, double b);
protected:
  void __init_lua_objects();
  virtual LuaInstanceMethod __getattr__(std::string &method_name);
  static int _advance_(lua_State *L);
  static int _get_output_(lua_State *L);
} ;

class BoundingBox : public DrawableObject
{
public:
  BoundingBox();
private:
  void draw_local();
} ;

class ImagePlane : public DrawableObject
{
public:
  ImagePlane();
private:
  GLfloat Lx0, Lx1, Ly0, Ly1;
  void draw_local();
} ;

class ParametricSurface : public DrawableObject
{
public:
  ParametricSurface();
private:
  GLfloat Lx0, Lx1, Ly0, Ly1;
  void draw_local();
} ;

class SegmentsEnsemble : public DrawableObject
{
public:
  SegmentsEnsemble();
private:
  void draw_local();
} ;

class TrianglesEnsemble : public DrawableObject
{
public:
  TrianglesEnsemble();
private:
  void draw_local();
} ;


#endif // __LuviewObjects_HEADER__
