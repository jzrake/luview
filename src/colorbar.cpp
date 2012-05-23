

#include <math.h>
#include "luview.hpp"
#include "pyplotcm.h"



ColormapCollection::LuaInstanceMethod ColormapCollection::__getattr__
(std::string &method_name)
{
  AttributeMap attr;
  attr["set_colormap"] = _set_colormap_;
  attr["next_colormap"] = _next_colormap_;
  attr["prev_colormap"] = _prev_colormap_;
  RETURN_ATTR_OR_CALL_SUPER(DataSource);
}
int ColormapCollection::_set_colormap_(lua_State *L)
{
  ColormapCollection *self = checkarg<ColormapCollection>(L, 1);
  const char *name = luaL_checkstring(L, 2);
  self->set_colormap(name);
  self->__staged = true;
  return 0;
}
int ColormapCollection::_next_colormap_(lua_State *L)
{
  ColormapCollection *self = checkarg<ColormapCollection>(L, 1);
  self->next_colormap();
  self->__staged = true;
  return 0;
}
int ColormapCollection::_prev_colormap_(lua_State *L)
{
  ColormapCollection *self = checkarg<ColormapCollection>(L, 1);
  self->prev_colormap();
  self->__staged = true;
  return 0;
}


MatplotlibColormaps::MatplotlibColormaps() : cmap_id(0)
{
  set_mode("rgba");
  cmap_data = pyplot_colors_get_lookup_table_by_id(cmap_id);
}

void MatplotlibColormaps::set_colormap(const char *name)
{
  const float *data = pyplot_colors_get_lookup_table(name);
  if (data == NULL) {
    luaL_error(__lua_state, "no color map named %s\n", name);
  }
  cmap_data = data;
  printf("set colormap to matplotlib:%s\n", name);
}
void MatplotlibColormaps::next_colormap()
{
  const char **names = pyplot_colors_get_names();
  if (names[++cmap_id] == NULL) {
    cmap_id = 0;
  }
  cmap_data = pyplot_colors_get_lookup_table(names[cmap_id]);
  printf("set colormap to matplotlib:%s\n", names[cmap_id]);
}
void MatplotlibColormaps::prev_colormap()
{
  const char **names = pyplot_colors_get_names();
  if (--cmap_id == -1) {
    cmap_id = pyplot_colors_get_num_tables() - 1;
  }
  cmap_data = pyplot_colors_get_lookup_table(names[cmap_id]);
  printf("set colormap to matplotlib:%s\n", names[cmap_id]);
}
void MatplotlibColormaps::__refresh_cpu()
{
  int N[2] = {256, 4};
  this->set_data(cmap_data, N, 2);
}
