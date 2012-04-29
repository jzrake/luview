


#include <fstream>
#include <streambuf>
#include "luview.hpp"


ShaderExample::ShaderExample()
{
  gl_modes.push_back(GL_BLEND);
}

ShaderExample::~ShaderExample()
{

}

void ShaderExample::draw_local()
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




void ShaderExample::printShaderInfoLog(GLuint obj)
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

void ShaderExample::printProgramInfoLog(GLuint obj)
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
