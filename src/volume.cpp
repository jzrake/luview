

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


const float CAMERA_DISTANCE = 6.0f;
const int   TEXTURE_WIDTH   = 256;  // NOTE: texture size cannot be larger than
const int   TEXTURE_HEIGHT  = 256;  // the rendering window size in non-FBO mode
int screenWidth = 1024;
int screenHeight = 768;
GLuint textureId;


static void draw_cube()
{
  glBindTexture(GL_TEXTURE_2D, textureId);

  glColor4f(1, 1, 1, 1);
  glBegin(GL_TRIANGLES);
  // front faces
  glNormal3f(0,0,1);
  // face v0-v1-v2
  glTexCoord2f(1,1);  glVertex3f(1,1,1);
  glTexCoord2f(0,1);  glVertex3f(-1,1,1);
  glTexCoord2f(0,0);  glVertex3f(-1,-1,1);
  // face v2-v3-v0
  glTexCoord2f(0,0);  glVertex3f(-1,-1,1);
  glTexCoord2f(1,0);  glVertex3f(1,-1,1);
  glTexCoord2f(1,1);  glVertex3f(1,1,1);

  // right faces
  glNormal3f(1,0,0);
  // face v0-v3-v4
  glTexCoord2f(0,1);  glVertex3f(1,1,1);
  glTexCoord2f(0,0);  glVertex3f(1,-1,1);
  glTexCoord2f(1,0);  glVertex3f(1,-1,-1);
  // face v4-v5-v0
  glTexCoord2f(1,0);  glVertex3f(1,-1,-1);
  glTexCoord2f(1,1);  glVertex3f(1,1,-1);
  glTexCoord2f(0,1);  glVertex3f(1,1,1);

  // top faces
  glNormal3f(0,1,0);
  // face v0-v5-v6
  glTexCoord2f(1,0);  glVertex3f(1,1,1);
  glTexCoord2f(1,1);  glVertex3f(1,1,-1);
  glTexCoord2f(0,1);  glVertex3f(-1,1,-1);
  // face v6-v1-v0
  glTexCoord2f(0,1);  glVertex3f(-1,1,-1);
  glTexCoord2f(0,0);  glVertex3f(-1,1,1);
  glTexCoord2f(1,0);  glVertex3f(1,1,1);

  // left faces
  glNormal3f(-1,0,0);
  // face  v1-v6-v7
  glTexCoord2f(1,1);  glVertex3f(-1,1,1);
  glTexCoord2f(0,1);  glVertex3f(-1,1,-1);
  glTexCoord2f(0,0);  glVertex3f(-1,-1,-1);
  // face v7-v2-v1
  glTexCoord2f(0,0);  glVertex3f(-1,-1,-1);
  glTexCoord2f(1,0);  glVertex3f(-1,-1,1);
  glTexCoord2f(1,1);  glVertex3f(-1,1,1);

  // bottom faces
  glNormal3f(0,-1,0);
  // face v7-v4-v3
  glTexCoord2f(0,0);  glVertex3f(-1,-1,-1);
  glTexCoord2f(1,0);  glVertex3f(1,-1,-1);
  glTexCoord2f(1,1);  glVertex3f(1,-1,1);
  // face v3-v2-v7
  glTexCoord2f(1,1);  glVertex3f(1,-1,1);
  glTexCoord2f(0,1);  glVertex3f(-1,-1,1);
  glTexCoord2f(0,0);  glVertex3f(-1,-1,-1);

  // back faces
  glNormal3f(0,0,-1);
  // face v4-v7-v6
  glTexCoord2f(0,0);  glVertex3f(1,-1,-1);
  glTexCoord2f(1,0);  glVertex3f(-1,-1,-1);
  glTexCoord2f(1,1);  glVertex3f(-1,1,-1);
  // face v6-v5-v4
  glTexCoord2f(1,1);  glVertex3f(-1,1,-1);
  glTexCoord2f(0,1);  glVertex3f(1,1,-1);
  glTexCoord2f(0,0);  glVertex3f(1,-1,-1);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, 0);
}


VolumeRendering::VolumeRendering()
{
  //  gl_modes.push_back(GL_BLEND);
  gl_modes.push_back(GL_TEXTURE_2D);



  // create a texture object
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap generation included in OpenGL v1.4
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}


void VolumeRendering::draw_local()
{
  static float t=0;
  // compute rotation angle
  const float ANGLE_SPEED = 90;   // degree/s
  float angle = ANGLE_SPEED * (t+=0.001);

  // render to texture //////////////////////////////////////////////////////


  // adjust viewport and projection matrix to texture dimension
  glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0f, (float)(TEXTURE_WIDTH)/TEXTURE_HEIGHT, 1.0f, 100.0f);
  glMatrixMode(GL_MODELVIEW);

  // camera transform
  //  glLoadIdentity();
  //  glTranslatef(0, 0, -CAMERA_DISTANCE);

  // clear buffer
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushAttrib(GL_COLOR_BUFFER_BIT | GL_PIXEL_MODE_BIT); // for GL_DRAW_BUFFER and GL_READ_BUFFER
  glDrawBuffer(GL_BACK);
  glReadBuffer(GL_BACK);

  // draw a rotating teapot at the origin
  glPushMatrix();
  glRotatef(angle*0.5f, 1, 0, 0);
  glRotatef(angle, 0, 1, 0);
  glRotatef(angle*0.7f, 0, 0, 1);
  glTranslatef(0, -1.575f, 0);
  drawTeapot();
  glPopMatrix();

  // copy the framebuffer pixels to a texture
  glBindTexture(GL_TEXTURE_2D, textureId);
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
  glBindTexture(GL_TEXTURE_2D, 0);

  glPopAttrib(); // GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT



  // back to normal viewport and projection matrix
  glViewport(0, 0, screenWidth, screenHeight);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0f, (float)(screenWidth)/screenHeight, 1.0f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // clear framebuffer
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glPushMatrix();

  // draw a cube with the dynamic texture
  draw_cube();

  glPopMatrix();
}

void VolumeRendering::draw_local3()
{
  int Nx=512, Ny=512;
  GLuint texId;

  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glViewport(Nx, Ny, Nx, Ny);

  glBindTexture(GL_TEXTURE_2D, texId);
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, Nx, Ny, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glPopAttrib();

  glBindTexture(GL_TEXTURE_2D, texId);
  glPolygonMode(GL_FRONT, GL_FILL);
  glBegin(GL_POLYGON);
  glTexCoord2d(0,0); glVertex2f(0,0);
  glTexCoord2d(0,1); glVertex2f(0,1);
  glTexCoord2d(1,1); glVertex2f(1,1);
  glTexCoord2d(1,0); glVertex2f(1,0);
  glEnd();
  glBindTexture(GL_TEXTURE_2D, 0);

  glDeleteTextures(1, &texId);
}

void VolumeRendering::draw_local1()
{
  int Nx=256, Ny=256;

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


  glBindFramebuffer(GL_FRAMEBUFFER_EXT, fboId);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, texId, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboId);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);


  // switch to drawing on the frame buffer
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  // clear buffers
  //  glClearColor(0, 1, 0, 1);
  //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPolygonMode(GL_FRONT, GL_FILL);
  glBegin(GL_POLYGON);
  glColor3f(1,0,0); glVertex3f(1,0,0);
  glColor3f(1,0,1); glVertex3f(1,0,1);
  glColor3f(1,1,1); glVertex3f(1,1,1);
  glColor3f(1,1,0); glVertex3f(1,1,0);
  glEnd();

  // unbind FBO
  glPopAttrib();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);



  // trigger mipmaps generation explicitly
  // NOTE: If GL_GENERATE_MIPMAP is set to GL_TRUE, then glCopyTexSubImage2D()
  // triggers mipmap generation automatically. However, the texture attached
  // onto a FBO should generate mipmaps manually via glGenerateMipmap().
  glBindTexture(GL_TEXTURE_2D, texId);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  //  glBindTexture(GL_TEXTURE_2D, TextureMap);
  glBindTexture(GL_TEXTURE_2D, texId);
  glPolygonMode(GL_FRONT, GL_FILL);
  glBegin(GL_POLYGON);
  glTexCoord2d(0,0); glVertex3f(0,0,0);
  glTexCoord2d(0,1); glVertex3f(0,0,1);
  glTexCoord2d(1,1); glVertex3f(0,1,1);
  glTexCoord2d(1,0); glVertex3f(0,1,0);
  glEnd();
  glBindTexture(GL_TEXTURE_2D, 0);


  //  glDeleteTextures(1, &TextureMap);
  //  delete [] texdata;

  glDeleteTextures(1, &texId);
  glDeleteFramebuffers(1, &fboId);
  glDeleteRenderbuffers(1, &rboId);
}




void VolumeRendering::draw_local2()
{
  int Nx=128, Ny=128;

  // ---------------------------------------------------------------------------
  // Setting up the frame buffer and texture
  // ---------------------------------------------------------------------------
  GLuint texId;
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Nx, Ny, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  // create a renderbuffer object to store depth info
  GLuint rboId;
  glGenRenderbuffers(1, &rboId);
  glBindRenderbuffer(GL_RENDERBUFFER, rboId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Nx, Ny);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  // create a framebuffer object
  GLuint fboId;
  glGenFramebuffers(1, &fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);

  // attach the texture to FBO color attachment point
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, texId, 0);

  // attach the renderbuffer to depth attachment point
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboId);

  // check FBO status
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    printf("warning! the fbo was not used!\n");
  }

  // switch back to window-system-provided framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);


  // ---------------------------------------------------------------------------
  // Drawing into the texture
  // ---------------------------------------------------------------------------

  // switch to drawing on the frame buffer
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glPolygonMode(GL_FRONT, GL_FILL);
  glBegin(GL_POLYGON);
  glColor3f(1,0,0); glVertex3f(0,0,-1);
  glColor3f(1,0,1); glVertex3f(0,1,-1);
  glColor3f(1,1,1); glVertex3f(1,1,-1);
  glColor3f(1,1,0); glVertex3f(1,0,-1);
  glEnd();

  // unbind FBO
  glPopAttrib();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // trigger mipmaps generation explicitly
  // NOTE: If GL_GENERATE_MIPMAP is set to GL_TRUE, then glCopyTexSubImage2D()
  // triggers mipmap generation automatically. However, the texture attached
  // onto a FBO should generate mipmaps manually via glGenerateMipmap().
  glBindTexture(GL_TEXTURE_2D, texId);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);



  // ---------------------------------------------------------------------------
  // Using the texture
  // ---------------------------------------------------------------------------

  glBindTexture(GL_TEXTURE_2D, texId);
  glPolygonMode(GL_FRONT, GL_FILL);
  glBegin(GL_POLYGON);
  glTexCoord2d(0,0); glVertex2f(0,0);
  glTexCoord2d(0,1); glVertex2f(0,1);
  glTexCoord2d(1,1); glVertex2f(1,1);
  glTexCoord2d(1,0); glVertex2f(1,0);
  glEnd();
  glBindTexture(GL_TEXTURE_2D, 0);


  // ---------------------------------------------------------------------------
  // Cleanup
  // ---------------------------------------------------------------------------
  glDeleteTextures(1, &texId);
  glDeleteRenderbuffers(1, &rboId);
  glDeleteFramebuffers(1, &fboId);
}

