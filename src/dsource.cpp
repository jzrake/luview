

#include "luview.hpp"
extern "C" {
#define LUNUM_API_NOCOMPLEX
#include "numarray.h"
#include "lunum.h"
}

DataSource::DataSource()
  : __cpu_transform(NULL),
    __gpu_transform(NULL),
    __input_ds(NULL),
    __output_ds(),
    __cpu_data(NULL),
    __tex_id(0),
    __ind_data(NULL),
    __num_dimensions(0),
    __num_indices(0),
    __staged(true)
{
  glGenTextures(1, &__tex_id);
  for (int i=0; i<__DATASOURCE_MAXDIMS; ++i) __num_points[i] = 0;
}

DataSource::~DataSource()
{
  if (__cpu_data) free(__cpu_data);
  if (__ind_data) free(__ind_data);
  glDeleteTextures(1, &__tex_id);
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
    this->__staged = false;
  }
  else if (this->__staged) {
    this->__refresh_cpu();
    this->__staged = false;
  }
}
const GLfloat *DataSource::get_data()
{
  this->__trigger_refresh();
  return __cpu_data;
}
const GLuint *DataSource::get_indices() { return __ind_data; }
GLuint DataSource::get_texture_id() { return __tex_id; }
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
void DataSource::set_normalize(int comp, bool mode)
{
  __normalize[comp] = mode;
}
void DataSource::check_num_dimensions(int ndims, const char *name)
{
  if (ndims != __num_dimensions) {
    luaL_error(__lua_state, "%s must have %d dimensions", name, ndims);
  }
}
void DataSource::check_num_points(int npnts, int dim, const char *name)
{
  if (npnts != this->get_num_points(dim)) {
    luaL_error(__lua_state, "%s must have %d points along dimension %d",
	       name, npnts, dim);
  }
}
void DataSource::check_has_data(const char *name)
{
  if (__cpu_data == NULL) {
    luaL_error(__lua_state, "%s must provide a floating point data buffer");
  }
}
void DataSource::check_has_indices(const char *name)
{
  if (__ind_data == NULL) {
    luaL_error(__lua_state, "%s must provide an index buffer");
  }
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
  RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
}

int DataSource::_get_output_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  const char *key = luaL_checkstring(L, 2);
  self->retrieve(self->get_output(key));
  return 1;
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
  return 0;
}
int DataSource::_set_normalize_(lua_State *L)
{
  DataSource *self = checkarg<DataSource>(L, 1);
  int comp = luaL_checkinteger(L, 2);
  bool mode = lua_toboolean(L, 3);
  luaL_checktype(L, 3, LUA_TBOOLEAN);
  self->set_normalize(comp, mode);
  return 0;
}
