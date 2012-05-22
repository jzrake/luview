


#include <fstream>
#include <streambuf>
#include "luview.hpp"


ShaderProgram::ShaderProgram() : vert(0), frag(0), prog(0) { }

void ShaderProgram::set_program(const char *vert_src, const char *frag_src)
{
  if (vert || frag || prog) {
    unset_program();
  }

  vert = glCreateShader(GL_VERTEX_SHADER);
  frag = glCreateShader(GL_FRAGMENT_SHADER);
  prog = glCreateProgram();

  glShaderSource(vert, 1, &vert_src, NULL);
  glShaderSource(frag, 1, &frag_src, NULL);

  glCompileShader(vert);
  glCompileShader(frag);

  glAttachShader(prog, vert);
  glAttachShader(prog, frag);
  glLinkProgram(prog);

  printShaderInfoLog(vert);
  printShaderInfoLog(frag);
  printProgramInfoLog(prog);
}

void ShaderProgram::activate()
{
  glGetIntegerv(GL_CURRENT_PROGRAM, &prev_prog);
  glUseProgram(prog);
}
void ShaderProgram::deactivate()
{
  glUseProgram(prev_prog);
}

void ShaderProgram::unset_program()
{
  glDetachShader(prog, vert);
  glDetachShader(prog, frag);
  glDeleteShader(vert);
  glDeleteShader(frag);
  glDeleteProgram(prog);
}
ShaderProgram::~ShaderProgram()
{
  unset_program();
}

void ShaderProgram::printShaderInfoLog(GLuint obj)
{
  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;

  glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

  if (infologLength > 0) {
    infoLog = (char*) malloc(infologLength);
    glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
    luaL_error(__lua_state, "[glsl] %s", infoLog);
    free(infoLog);
  }
}

void ShaderProgram::printProgramInfoLog(GLuint obj)
{
  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;

  glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

  if (infologLength > 0) {
    infoLog = (char*) malloc(infologLength);
    glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
    luaL_error(__lua_state, "[glsl] %s", infoLog);
    free(infoLog);
  }
}

void ShaderProgram::set_uniform(const char *name, GLint value)
{
  GLint loc = glGetUniformLocation(prog, name);
  GLint existing_pro; // save the existing program state
  glGetIntegerv(GL_CURRENT_PROGRAM, &existing_pro);

  glUseProgram(prog);
  glUniform1i(loc, value);

  glUseProgram(existing_pro); // replace the existing program
}

ShaderProgram::LuaInstanceMethod ShaderProgram::__getattr__
(std::string &method_name)
{
  AttributeMap attr;
  attr["set_program"] = _set_program_;
  RETURN_ATTR_OR_CALL_SUPER(LuaCppObject);
}
int ShaderProgram::_set_program_(lua_State *L)
{
  ShaderProgram *self = checkarg<ShaderProgram>(L, 1);
  const char *v = luaL_checkstring(L, 2);
  const char *f = luaL_checkstring(L, 3);
  self->set_program(v, f);
  return 0;
}


/*
void ShaderProgram::draw_local()
{
  GLuint vert = glCreateShader(GL_VERTEX_SHADER);
  GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
  GLuint prog = glCreateProgram();

  std::ifstream v("/Users/jzrake/Work/luview/test.vert");
  std::ifstream f("/Users/jzrake/Work/luview/test.frag");

  std::string vert_src_str((std::istreambuf_iterator<char>(v)),
			   std::istreambuf_iterator<char>());

  std::string frag_src_str((std::istreambuf_iterator<char>(f)),
			   std::istreambuf_iterator<char>());

  const char *vert_src = vert_src_str.c_str();
  const char *frag_src = frag_src_str.c_str();

  glShaderSource(vert, 1, &vert_src, NULL);
  glShaderSource(frag, 1, &frag_src, NULL);

  glCompileShader(vert);
  glCompileShader(frag);

  glAttachShader(prog, vert);
  glAttachShader(prog, frag);

  glLinkProgram(prog);
  glUseProgram(prog);

  printShaderInfoLog(vert);
  printShaderInfoLog(frag);
  printProgramInfoLog(prog);


  glBegin(GL_TRIANGLES);
  glVertex3d(0.5, 0.0, 0.0);
  glVertex3d(0.0, 0.5, 0.0);
  glVertex3d(0.0,-0.5, 0.0);

  glVertex3d(0.5, 0.0, 0.5);
  glVertex3d(0.0, 0.5, 0.5);
  glVertex3d(0.0,-0.5, 0.5);

  glVertex3d(0.5, 0.0,-0.5);
  glVertex3d(0.0, 0.5,-0.5);
  glVertex3d(0.0,-0.5,-0.5);
  glEnd();

  glUseProgram(0);
  glDetachShader(prog, vert);
  glDetachShader(prog, frag);
  glDeleteShader(vert);
  glDeleteShader(frag);
  glDeleteProgram(prog);
}
*/
