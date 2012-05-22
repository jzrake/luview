

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
  GLenum fmt;
  int size;
  const char *name;
} ;
static TextureFormat textureFormats[] =
  {{GL_LUMINANCE, 1, "luminance"},
   {GL_ALPHA, 1, "alpha"},
   {GL_RGB, 3, "rgb"},
   {GL_RGBA, 4, "rgba"},
   {0, 0, NULL}};



DataSource::DataSource()
  : __cpu_transform(NULL),
    __gpu_transform(NULL),
    __input_ds(NULL),
    __output_ds(),
    __cpu_data(NULL),
    __ind_data(NULL),
    __texture_id(0),
    __texture_format(GL_LUMINANCE),
    __num_dimensions(0),
    __num_indices(0),
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
    return __input_ds->__ancestor_is_staged();
  }
}
void DataSource::__trigger_refresh()
{
  if (this->__ancestor_is_staged()) {
    this->__input_ds->__trigger_refresh();
    this->__refresh_cpu();
    this->__cp_cpu_to_gpu();
    this->__staged = false;
  }
  else if (this->__staged) {
    this->__refresh_cpu();
    this->__cp_cpu_to_gpu();
    this->__staged = false;
  }
}
const GLfloat *DataSource::get_data()
{
  this->__trigger_refresh();
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
void DataSource::set_data(const GLfloat *data, const int *np, int nd)
{
  __num_dimensions = nd;
  for (int i=0; i<__num_dimensions; ++i) __num_points[i] = np[i];
  size_t sz = this->get_size() * sizeof(GLfloat);
  __cpu_data = (GLfloat*) realloc(__cpu_data, sz);
  std::memcpy(__cpu_data, data, sz);
}
void DataSource::set_indices(const GLuint *indices, int ni)
{
  __num_indices = ni;
  size_t sz = ni * sizeof(GLuint);
  __ind_data = (GLuint*) realloc(__ind_data, sz);
  std::memcpy(__ind_data, indices, sz);
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
  this->__trigger_refresh();
  glBindTexture(__texture_target, __texture_id);
  glTexParameteri(__texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(__texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}
void DataSource::__cp_cpu_to_gpu()
{
  const int *N = __num_points;
  const GLfloat *buf = __cpu_data;
  const GLenum fmt = textureFormats[__texture_format].fmt;
  const int sz = textureFormats[__texture_format].size;

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
      glTexImage2D(GL_TEXTURE_2D, 0, 1, N[0], N[1], 0, fmt, GL_FLOAT, buf);
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
      glTexImage3D(GL_TEXTURE_3D, 0, 1, N[0], N[1], N[2], 0, fmt, GL_FLOAT, buf);
      __texture_target = GL_TEXTURE_3D;
    }
    else {
      // (2): the last axis must have size sz
      glBindTexture(GL_TEXTURE_2D, __texture_id);
      glTexImage2D(GL_TEXTURE_2D, 0, sz, N[0], N[1], 0, fmt, GL_FLOAT, buf);
      __texture_target = GL_TEXTURE_2D;
    }
    break;
  }
  glPopAttrib();
}


void DataSource::__execute_gpu_transform()
{
  if (__input_ds == NULL || __gpu_transform == NULL) return;

  const int *N = __input_ds->__num_points;
  const GLfloat *input_data = __input_ds->get_data();

  //  code to eventually do the data transfer all in hardware
  //
  //  GLuint read_buf;
  //  glGenBuffers(1, &read_buf);
  //  glBindBuffer(GL_PIXEL_PACK_BUFFER, read_buf);
  //  __input_ds->become_texture();

  ren2tex_start(N[0], N[1], __texture_id); // binds a new fbo
  __gpu_transform->activate();

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPushMatrix();
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, N[0], N[1]);
  glLoadIdentity();

  glWindowPos2i(0, 0);
  glDrawPixels(N[0], N[1], GL_RGBA, GL_FLOAT, input_data);

  glPopMatrix();
  glPopAttrib();

  __gpu_transform->deactivate();

  ren2tex_finish(); // unbinds and frees the fbo
}


DataSource::LuaInstanceMethod
DataSource::__getattr__(std::string &method_name)
{
  AttributeMap attr;
  attr["get_output"] = _get_output_;
  attr["get_data"] = _get_data_;
  attr["set_data"] = _set_data_;
  attr["get_input"] = _get_input_;
  attr["set_input"] = _set_input_;
  attr["get_transform"] = _get_transform_;
  attr["set_transform"] = _set_transform_;
  attr["get_mode"] = _get_mode_;
  attr["set_mode"] = _set_mode_;
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
  int comp = luaL_checkinteger(L, 2);
  bool mode = lua_toboolean(L, 3);
  luaL_checktype(L, 3, LUA_TBOOLEAN);
  self->__normalize[comp] = mode;
  self->__staged = true;
  return 0;
}
int DataSource::_get_data_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  const int N = self->get_size();
  struct Array A = array_new_zeros(N, ARRAY_TYPE_FLOAT);
  std::memcpy(A.data, self->__cpu_data, N*array_sizeof(ARRAY_TYPE_FLOAT));
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
  self->__staged = true;
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
  std::vector<const char*> modes;
  for (int n=0; ; ++n) {
    if (textureFormats[n].name == NULL) break;
    modes.push_back(textureFormats[n].name);
  }
  self->__texture_format = luaL_checkoption(L, 2, NULL, &modes[0]);
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
  self->__input_ds = self->replace(self->__input_ds, inpt);
  self->__staged = true;
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

