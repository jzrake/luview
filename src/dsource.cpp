

#include "luview.hpp"
extern "C" {
#define LUNUM_API_NOCOMPLEX
#include "numarray.h"
#include "lunum.h"
}

extern "C" {
void ren2tex_start(int Nx, int Ny, GLuint texture_target);
void ren2tex_finish();
}

struct TextureFormat
{
  int ind;
  GLenum fmt;
  int size;
  const char *name;
} ;
static TextureFormat textureFormats[] =
  {{0, GL_NONE, 0, "none"},
   {1, GL_LUMINANCE, 1, "luminance"},
   {2, GL_ALPHA, 1, "alpha"},
   {3, GL_RGB, 3, "rgb"},
   {4, GL_RGBA, 4, "rgba"},
   {5, 0, 0, NULL}};



DataSource::DataSource()
  : __cpu_transform(NULL),
    __gpu_transform(NULL),
    __input_ds(NULL),
    __output_ds(),
    __cpu_data(NULL),
    __ind_data(NULL),
    __texture_id(0),
    __texture_format(0),
    __num_dimensions(1),
    __num_indices(0),
    __normalize(false),
    __staged(true)
{
  glGenTextures(1, &__texture_id);
  for (int i=0; i<__DATASOURCE_MAXDIMS; ++i) __num_points[i] = 0;
}

DataSource::~DataSource()
{
  if (__cpu_data) free(__cpu_data);
  if (__ind_data) free(__ind_data);
  glDeleteTextures(1, &__texture_id);
}

bool DataSource::__ancestor_is_staged()
{
  if (__input_ds == NULL) {
    return false;
  }
  else {
    return __input_ds->__staged || __input_ds->__ancestor_is_staged();
  }
}
void DataSource::__trigger_refresh()
{
  if (__ancestor_is_staged()) {
    __input_ds->__trigger_refresh();
    __staged = true;
  }
  if (__staged) {
    __refresh_cpu();
    __do_normalize();
    __cp_cpu_to_gpu();
    //    __execute_gpu_transform();
    __staged = false;
  }
}
const GLfloat *DataSource::get_data()
{
  return __cpu_data;
}
const GLuint *DataSource::get_indices()
{
  return __ind_data;
}
GLuint DataSource::get_texture_id()
{
  return __texture_id;
}
DataSource *DataSource::get_output(const char *n)
{
  DataSourceMap::iterator v = __output_ds.find(n);
  return v == __output_ds.end() ? NULL : v->second;
}
int DataSource::get_size()
{
  int N = 1;
  for (int i=0; i<__num_dimensions; ++i) N *= __num_points[i];
  return N;
}
int DataSource::get_num_points(int d)
{
  return d < __DATASOURCE_MAXDIMS ? __num_points[d] : 0;
}
int DataSource::get_num_dimensions() { return __num_dimensions; }
int DataSource::get_num_indices() { return __num_indices; }
void DataSource::set_input(DataSource *inpt)
{
  __input_ds = replace(__input_ds, inpt);
  __staged = true;
}
void DataSource::set_mode(const char *mode)
{
  std::map<std::string, TextureFormat> modes;
  for (int n=0; ; ++n) {
    if (textureFormats[n].name == NULL) break;
    modes[textureFormats[n].name] = textureFormats[n];
  }
  std::map<std::string, TextureFormat>::iterator m = modes.find(mode);
  if (m == modes.end()) {
    luaL_error(__lua_state, "no texture format mode %s", mode);
  }
  __texture_format = m->second.ind;
  __staged = true;
}
void DataSource::set_data(const GLfloat *data, const int *np, int nd)
{
  __num_dimensions = nd;
  for (int i=0; i<__num_dimensions; ++i) __num_points[i] = np[i];
  size_t sz = this->get_size() * sizeof(GLfloat);
  __cpu_data = (GLfloat*) realloc(__cpu_data, sz);
  std::memcpy(__cpu_data, data, sz);
  __staged = true;
}
void DataSource::set_indices(const GLuint *indices, int ni)
{
  size_t sz = ni * sizeof(GLuint);
  __ind_data = (GLuint*) realloc(__ind_data, sz);
  std::memcpy(__ind_data, indices, sz);
  __num_indices = ni;
  __staged = true;
}
void DataSource::check_num_dimensions(const char *name, int ndims)
{
  if (ndims != __num_dimensions) {
    luaL_error(__lua_state, "%s must have %d dimensions", name, ndims);
  }
}
void DataSource::check_num_points(const char *name, int npnts, int dim)
{
  if (npnts != this->get_num_points(dim)) {
    luaL_error(__lua_state, "%s must have %d points along dimension %d",
               name, npnts, dim);
  }
}
void DataSource::check_has_data(const char *name)
{
  if (__cpu_data == NULL) {
    luaL_error(__lua_state, "%s must provide a floating point data buffer",
	       name);
  }
}
void DataSource::check_has_indices(const char *name)
{
  if (__ind_data == NULL) {
    luaL_error(__lua_state, "%s must provide an index buffer", name);
  }
}
void DataSource::become_texture()
{
  glBindTexture(__texture_target, __texture_id);
  glTexParameteri(__texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(__texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(__texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(__texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
void DataSource::compile()
{
  __trigger_refresh();
}
void DataSource::__cp_cpu_to_gpu()
{
  const int *N = __num_points;
  const GLfloat *buf = __cpu_data;
  const GLenum fmt = textureFormats[__texture_format].fmt;
  const int sz = textureFormats[__texture_format].size;

  if (fmt == GL_NONE) return;
  glPushAttrib(GL_TEXTURE_BIT);

  switch (__num_dimensions) {
  case 1:
    glBindTexture(GL_TEXTURE_1D, __texture_id);
    glTexImage1D(GL_TEXTURE_1D, 0, 1, N[0], 0, fmt, GL_FLOAT, buf);
    __texture_target = GL_TEXTURE_1D;
    break;
  case 2:
    // Check whether this data source contains (1) 2d luminance data, or (2) a
    // 1d array of e.g. RGB values.
    if (sz == 1) {
      // (1): it's a 2d texture of scalar data
      glBindTexture(GL_TEXTURE_2D, __texture_id);
      glTexImage2D(GL_TEXTURE_2D, 0, 1, N[1], N[0], 0, fmt, GL_FLOAT, buf);
      __texture_target = GL_TEXTURE_2D;
    }
    else {
      // (2): the last axis must have size sz
      glBindTexture(GL_TEXTURE_1D, __texture_id);
      glTexImage1D(GL_TEXTURE_1D, 0, sz, N[0], 0, fmt, GL_FLOAT, buf);
      __texture_target = GL_TEXTURE_1D;
    }
    break;
  case 3:
    // Check whether this data source contains (1) 3d luminance data, or (2) a
    // 2d array of e.g. RGB values.
    if (sz == 1) {
      // (1): it's a 3d texture of scalar data
      glBindTexture(GL_TEXTURE_3D, __texture_id);
      glTexImage3D(GL_TEXTURE_3D, 0, 1, N[2], N[1], N[0], 0, fmt, GL_FLOAT, buf);
      __texture_target = GL_TEXTURE_3D;
    }
    else {
      // (2): the last axis must have size sz
      glBindTexture(GL_TEXTURE_2D, __texture_id);
      glTexImage2D(GL_TEXTURE_2D, 0, sz, N[1], N[0], 0, fmt, GL_FLOAT, buf);
      __texture_target = GL_TEXTURE_2D;
    }
    break;
  }
  glPopAttrib();
}

void DataSource::__do_normalize()
{
  if (!__normalize || __cpu_data == NULL) return;

  int Nt = this->get_size();
  double x0 = 0.0;
  double x1 = 1.0;
  double xmin = +1e16;
  double xmax = -1e16;
  for (int n=0; n<Nt; ++n) {
    const GLfloat x = __cpu_data[n];
    if (x > xmax) xmax = x;
    if (x < xmin) xmin = x;
  }
  for (int n=0; n<Nt; ++n) {
    __cpu_data[n] -= xmin;
    __cpu_data[n] /= xmax - xmin;
    __cpu_data[n] *= x1 - x0;
    __cpu_data[n] += x0;
  }
}


void DataSource::__execute_gpu_transform()
{
  /*
  if (__gpu_transform == NULL) return;

  printf("executing the gpu thing\n");
  const int *N = __num_points;
  GLuint tex;

  glGenTextures(1, &tex);
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPushMatrix();
  __gpu_transform->activate();

  ren2tex_start(N[0], N[1], tex); // binds a new fbo
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, N[0], N[1]);
  glLoadIdentity();

  glWindowPos2i(0, 0);
  glDrawPixels(N[0], N[1], GL_LUMINANCE, GL_FLOAT, __cpu_data);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_LUMINANCE, GL_FLOAT, __cpu_data);

  ren2tex_finish(); // unbinds and frees the fbo

  __staged = true;
  __gpu_transform->deactivate();
  glPopMatrix();
  glPopAttrib();
  glDeleteTextures(1, &tex);
  */
}


DataSource::LuaInstanceMethod
DataSource::__getattr__(std::string &method_name)
{
  AttributeMap attr;
  attr["get_output"] = _get_output_;
  attr["set_normalize"] = _set_normalize_;
  attr["get_data"] = _get_data_;
  attr["set_data"] = _set_data_;
  attr["get_input"] = _get_input_;
  attr["set_input"] = _set_input_;
  attr["get_transform"] = _get_transform_;
  attr["set_transform"] = _set_transform_;
  attr["get_program"] = _get_program_;
  attr["set_program"] = _set_program_;
  attr["get_mode"] = _get_mode_;
  attr["set_mode"] = _set_mode_;
  attr["compile"] = _compile_;
  RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
}

int DataSource::_get_output_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  const char *key = luaL_checkstring(L, 2);
  self->retrieve(self->get_output(key));
  return 1;
}
int DataSource::_set_normalize_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  bool mode = lua_toboolean(L, 2);
  luaL_checktype(L, 2, LUA_TBOOLEAN);
  self->__normalize = mode;
  self->__staged = true;
  return 0;
}
int DataSource::_get_data_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  const GLfloat *data = self->get_data();
  const int N = self->get_size();
  struct Array A = array_new_zeros(N, ARRAY_TYPE_FLOAT);
  std::memcpy(A.data, data, N*array_sizeof(ARRAY_TYPE_FLOAT));
  array_resize(&A, self->__num_points, self->__num_dimensions);
  lunum_pusharray1(L, &A);
  return 1;
}
int DataSource::_set_data_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  if (lunum_upcast(L, 2, ARRAY_TYPE_FLOAT, 1)) {
    lua_replace(L, 2);
  }
  Array *A = lunum_checkarray1(L, 2);
  self->set_data((GLfloat*)A->data, A->shape, A->ndims);
  return 0;
}
int DataSource::_get_mode_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  lua_pushstring(L, textureFormats[self->__texture_format].name);
  return 1;
}
int DataSource::_set_mode_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  const char *mode = luaL_checkstring(L, 2);
  self->set_mode(mode);
  return 0;
}
int DataSource::_get_input_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  self->retrieve(self->__input_ds);
  return 1;
}
int DataSource::_set_input_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  DataSource *inpt = checkarg<DataSource>(L, 2);
  self->set_input(inpt);
  return 0;
}
int DataSource::_get_transform_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  self->retrieve(self->__cpu_transform);
  return 1;
}
int DataSource::_set_transform_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  CallbackFunction *cb = checkarg<CallbackFunction>(L, 2);
  self->__cpu_transform = self->replace(self->__cpu_transform, cb);
  self->__staged = true;
  return 0;
}
int DataSource::_get_program_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  self->retrieve(self->__gpu_transform);
  return 1;
}
int DataSource::_set_program_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  ShaderProgram *sp = checkarg<ShaderProgram>(L, 2);
  self->__gpu_transform = self->replace(self->__gpu_transform, sp);
  self->__staged = true;
  return 0;
}
int DataSource::_compile_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  self->compile();
  return 0;
}

GridSource2D::GridSource2D()
{
  u0 = -0.5;
  u1 =  0.5;
  v0 = -0.5;
  v1 =  0.5;

  Nu = 16;
  Nv = 16;
}

void GridSource2D::__refresh_cpu()
{
  __num_dimensions = 2;
  __num_indices = 0;
  __num_points[0] = Nu;
  __num_points[1] = Nv;
  __cpu_data = (GLfloat*) realloc(__cpu_data, 2*Nu*Nv*sizeof(GLfloat));

  const int su = Nv;
  const int sv = 1;
  const double du = (u1 - u0) / (Nu - 1);
  const double dv = (v1 - v0) / (Nv - 1);

  for (int i=0; i<Nu; ++i) {
    for (int j=0; j<Nv; ++j) {
      const int m = i*su + j*sv;
      __cpu_data[2*m + 0] = u0 + i*du;
      __cpu_data[2*m + 1] = v0 + j*dv;
    }
  }
}

GridSource2D::LuaInstanceMethod
GridSource2D::__getattr__(std::string &method_name)
{
  AttributeMap attr;
  attr["set_num_points"] = _set_num_points_;
  attr["set_u_range"] = _set_u_range_;
  attr["set_v_range"] = _set_v_range_;
  RETURN_ATTR_OR_CALL_SUPER(DataSource);
}
int GridSource2D::_set_num_points_(lua_State *L)
{
  GridSource2D *self = checkarg<GridSource2D>(L, 1);
  self->Nu = luaL_checkinteger(L, 2);
  self->Nv = luaL_checkinteger(L, 3);
  self->__staged = true;
  return 0;
}
int GridSource2D::_set_u_range_(lua_State *L)
{
  GridSource2D *self = checkarg<GridSource2D>(L, 1);
  self->u0 = luaL_checknumber(L, 2);
  self->u1 = luaL_checknumber(L, 3);
  self->__staged = true;
  return 0;
}
int GridSource2D::_set_v_range_(lua_State *L)
{
  GridSource2D *self = checkarg<GridSource2D>(L, 1);
  self->v0 = luaL_checknumber(L, 2);
  self->v1 = luaL_checknumber(L, 3);
  self->__staged = true;
  return 0;
}


ParametricVertexSource3D::ParametricVertexSource3D()
  : GridSource2D()
{

}
void ParametricVertexSource3D::__refresh_cpu()
{
  if (__input_ds == NULL) {
    luaL_error(__lua_state, "need an input data source\n");
  }
  std::string tname = _get_type();
  __input_ds->check_has_data(tname.c_str());
  __input_ds->check_num_dimensions(tname.c_str(), 2);

  Nu = __input_ds->get_num_points(0);
  Nv = __input_ds->get_num_points(1);

  __num_dimensions = 2;
  //  __num_indices = 

  __num_points[0] = Nu*Nv;
  __num_points[1] = 3;
  __cpu_data = (GLfloat*) realloc(__cpu_data, 3*Nu*Nv*sizeof(GLfloat));

  printf("%d %d\n", __num_points[0], __num_points[1]);

  const GLfloat *input = __input_ds->get_data();
  const int su = Nv;
  const int sv = 1;
  const double du = (u1 - u0) / (Nu - 1);
  const double dv = (v1 - v0) / (Nv - 1);

  for (int i=0; i<Nu; ++i) {
    for (int j=0; j<Nv; ++j) {
      const int m = i*su + j*sv;
      __cpu_data[3*m + 0] = u0 + i*du;
      __cpu_data[3*m + 1] = v0 + j*dv;
      __cpu_data[3*m + 2] = input[m];
    }
  }
}
