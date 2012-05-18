

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include "luview.hpp"
#include "teapot.h"

#define GL_GLEXT_PROTOTYPES
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


static void draw_cube()
{
  const GLfloat x0 = -0.5;
  const GLfloat x1 = +0.5;
  const GLfloat c0 =  0.0;
  const GLfloat c1 =  1.0;

  glBegin(GL_QUADS);
  glNormal3f(1,0,0);
  glColor3f(c0,c0,c0); glVertex3f(x0,x0,x0);
  glColor3f(c0,c0,c1); glVertex3f(x0,x0,x1);
  glColor3f(c0,c1,c1); glVertex3f(x0,x1,x1);
  glColor3f(c0,c1,c0); glVertex3f(x0,x1,x0);
  glEnd();

  glBegin(GL_QUADS);
  glNormal3f(0,1,0);
  glColor3f(c0,c0,c0); glVertex3f(x0,x0,x0);
  glColor3f(c1,c0,c0); glVertex3f(x1,x0,x0);
  glColor3f(c1,c0,c1); glVertex3f(x1,x0,x1);
  glColor3f(c0,c0,c1); glVertex3f(x0,x0,x1);
  glEnd();

  glBegin(GL_QUADS);
  glNormal3f(0,0,1);
  glColor3f(c0,c0,c0); glVertex3f(x0,x0,x0);
  glColor3f(c0,c1,c0); glVertex3f(x0,x1,x0);
  glColor3f(c1,c1,c0); glVertex3f(x1,x1,x0);
  glColor3f(c1,c0,c0); glVertex3f(x1,x0,x0);
  glEnd();
}

VolumeRendering::VolumeRendering()
{
  gl_modes.push_back(GL_TEXTURE_2D);
  //  gl_modes.push_back(GL_LIGHTING);
  //  gl_modes.push_back(GL_LIGHT0);
  //  gl_modes.push_back(GL_BLEND);
  //  gl_modes.push_back(GL_COLOR_MATERIAL);
}


void VolumeRendering::draw_local()
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glTranslatef(0,1.5,0);
  draw_cube();
  glTranslatef(0,-1.5,0);
  glPopAttrib();

  int Nx=1024, Ny=768;

  GLuint texId;
  GLuint fboId;
  GLuint rboId;

  glGenTextures(1, &texId);
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
  glBindFramebuffer(GL_FRAMEBUFFER, 0);


  // switch to drawing on the frame buffer
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  //  glPushMatrix();

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    printf("warning! the fbo is not complete\n");
  }

  // clear buffers
  glClearColor(0.5, 0.5, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw_cube();

  //  glPopMatrix();
  glPopAttrib();
  glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind FBO



  // trigger mipmaps generation explicitly
  // NOTE: If GL_GENERATE_MIPMAP is set to GL_TRUE, then glCopyTexSubImage2D()
  // triggers mipmap generation automatically. However, the texture attached
  // onto a FBO should generate mipmaps manually via glGenerateMipmap().
  glBindTexture(GL_TEXTURE_2D, texId);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindTexture(GL_TEXTURE_2D, texId);
  glPolygonMode(GL_FRONT, GL_FILL);
  glBegin(GL_POLYGON);
  glNormal3f(0,0,1);
  glTexCoord2d(0,0); glVertex2f(-0.5,-0.5);
  glTexCoord2d(0,1); glVertex2f(-0.5, 0.5);
  glTexCoord2d(1,1); glVertex2f( 0.5, 0.5);
  glTexCoord2d(1,0); glVertex2f( 0.5,-0.5);
  glEnd();
  glBindTexture(GL_TEXTURE_2D, 0);


  glDeleteTextures(1, &texId);
  glDeleteFramebuffers(1, &fboId);
  glDeleteRenderbuffers(1, &rboId);
}
