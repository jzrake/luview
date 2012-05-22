
#define GL_GLEXT_PROTOTYPES
#include "GL/glfw.h"
#include <GL/glext.h>

#define glBindFramebuffer glBindFramebufferEXT
#define glGenRenderbuffers glGenRenderbuffersEXT
#define glGenFramebuffers glGenFramebuffersEXT
#define glDeleteRenderbuffers glDeleteRenderbuffersEXT
#define glDeleteFramebuffers glDeleteFramebuffersEXT
#define glRenderbufferStorage glRenderbufferStorageEXT
#define glFramebufferRenderbuffer glFramebufferRenderbufferEXT
#define glBindRenderbuffer glBindRenderbufferEXT
#define glFramebufferTexture2D glFramebufferTexture2DEXT
#define glCheckFramebufferStatus glCheckFramebufferStatusEXT
#define glGenerateMipmap glGenerateMipmapEXT

#define GL_FRAMEBUFFER GL_FRAMEBUFFER_EXT
#define GL_RENDERBUFFER GL_RENDERBUFFER_EXT
#define GL_DEPTH_ATTACHMENT GL_DEPTH_ATTACHMENT_EXT
#define GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_EXT
#define GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE_EXT

static GLuint texId;
static GLuint fboId;
static GLuint rboId;

void ren2tex_start(int Nx, int Ny, GLuint texture_target)
{
  glPushAttrib(GL_TEXTURE_BIT);
  texId = texture_target;
  glGenFramebuffers(1, &fboId);
  glGenRenderbuffers(1, &rboId);

  glBindTexture(GL_TEXTURE_2D, texId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Nx, Ny, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindRenderbuffer(GL_RENDERBUFFER, rboId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Nx, Ny);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, fboId);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, texId, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboId);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    return;
  }

  glPopAttrib(); // GL_TEXTURE_BIT
}

void ren2tex_finish()
{
  glPushAttrib(GL_TEXTURE_BIT);
  glBindTexture(GL_TEXTURE_2D, texId);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glDeleteFramebuffers(1, &fboId);
  glDeleteRenderbuffers(1, &rboId);
  glPopAttrib(); // GL_TEXTURE_BIT
}
