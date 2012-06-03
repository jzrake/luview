
#include "luview.hpp"
#include "FTGL/ftgl.h"

TextRendering::TextRendering()
{
  gl_modes.push_back(GL_DEPTH_TEST);
  gl_modes.push_back(GL_LIGHTING);
  gl_modes.push_back(GL_LIGHT0);
  gl_modes.push_back(GL_BLEND);
  gl_modes.push_back(GL_COLOR_MATERIAL);
  gl_modes.push_back(GL_AUTO_NORMAL);
  gl_modes.push_back(GL_NORMALIZE);
}
void TextRendering::draw_local()
{
  //  FTGLPixmapFont font("/usr/X11/lib/X11/fonts/TTF/VeraSeBd.ttf");
  FTExtrudeFont font("/usr/X11/lib/X11/fonts/TTF/VeraSeBd.ttf");

  // If something went wrong, bail out.
  if(font.Error()) return;

  // Set the font size and render a small text.
  font.FaceSize(1);
  font.Depth(0.2);
  font.Render("Hello World!");
}
