
#include "luview.hpp"
#include "FTGL/ftgl.h"

//  FTPolygonFont font("/usr/X11/lib/X11/fonts/TTF/VeraSeBd.ttf");
//  FTBitmapFont font("/usr/X11/lib/X11/fonts/TTF/VeraSeBd.ttf");
//  FTOutlineFont font("/usr/X11/lib/X11/fonts/TTF/VeraSeBd.ttf");

TextRendering::TextRendering()
{
  gl_modes.push_back(GL_DEPTH_TEST);
  gl_modes.push_back(GL_LIGHTING);
  gl_modes.push_back(GL_LIGHT0);
  gl_modes.push_back(GL_BLEND);
  gl_modes.push_back(GL_COLOR_MATERIAL);
  gl_modes.push_back(GL_AUTO_NORMAL);
  gl_modes.push_back(GL_NORMALIZE);

  const char *font_file = "/usr/X11/lib/X11/fonts/TTF/Vera.ttf";

  fonts["bitmap"] = new FTBitmapFont(font_file);
  fonts["buffer"] = new FTExtrudeFont(font_file);
  fonts["extrude"] = new FTExtrudeFont(font_file);
  fonts["outline"] = new FTExtrudeFont(font_file);
  fonts["pixmap"] = new FTExtrudeFont(font_file);
  fonts["polygon"] = new FTExtrudeFont(font_file);
  fonts["texture"] = new FTExtrudeFont(font_file);
}
TextRendering::~TextRendering()
{
  delete fonts["bitmap"];
  delete fonts["buffer"];
  delete fonts["extrude"];
  delete fonts["outline"];
  delete fonts["pixmap"];
  delete fonts["polygon"];
  delete fonts["texture"];
}
void TextRendering::draw_local()
{
  FTFont *font = fonts["extrude"];

  if (font->Error()) {
    luaL_error(__lua_state, "could not open TrueType font");
  }

  font->FaceSize(1);
  font->Depth(0.2);
  font->Render(text.c_str());
}
void TextRendering::set_text(std::string t)
{
  text = t;
}
const std::string &TextRendering::get_text()
{
  return text;
}

TextRendering::LuaInstanceMethod
TextRendering::__getattr__(std::string &method_name)
{
  AttributeMap attr;
  attr["get_text"] = _get_text_;
  attr["set_text"] = _set_text_;
  RETURN_ATTR_OR_CALL_SUPER(DrawableObject);
}

int TextRendering::_get_text_(lua_State *L)
{
  TextRendering *self = checkarg<TextRendering>(L, 1);
  lua_pushstring(L, self->get_text().c_str());
  return 1;
}

int TextRendering::_set_text_(lua_State *L)
{
  TextRendering *self = checkarg<TextRendering>(L, 1);
  self->set_text(luaL_checkstring(L, 2));
  return 0;
}
