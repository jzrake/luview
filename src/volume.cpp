

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include "luview.hpp"


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




VolumeRendering::VolumeRendering()
{
  //  gl_modes.push_back(GL_BLEND);
  gl_modes.push_back(GL_TEXTURE_2D);
}



void VolumeRendering::draw_local2()
{
  int Nx=100, Ny=100;
  GLfloat *texdata = new GLfloat[Nx*Ny*4];

  for (int i=0; i<Nx; ++i) {
    for (int j=0; j<Ny; ++j) {
      texdata[(i*Ny + j)*4 + 0] = 0.1;
      texdata[(i*Ny + j)*4 + 1] = 0.9;
      texdata[(i*Ny + j)*4 + 2] = 0.9;
      texdata[(i*Ny + j)*4 + 3] = 1.0;
    }
  }

  GLuint TextureMap;
  glGenTextures(1, &TextureMap);
  glBindTexture(GL_TEXTURE_2D, TextureMap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, Nx, Ny, 0, GL_RGBA, GL_FLOAT, texdata);
  glBindTexture(GL_TEXTURE_2D, 0);



  glBindTexture(GL_TEXTURE_2D, TextureMap);
  glPolygonMode(GL_FRONT, GL_FILL);
  glBegin(GL_POLYGON);
  glTexCoord2d(0,0); glVertex2f(0,0);
  glTexCoord2d(0,1); glVertex2f(0,1);
  glTexCoord2d(1,1); glVertex2f(1,1);
  glTexCoord2d(1,0); glVertex2f(1,0);
  glEnd();
  glBindTexture(GL_TEXTURE_2D, 0);


  glDeleteTextures(1, &TextureMap);
  delete [] texdata;
}

void VolumeRendering::draw_local()
{
  int Nx=100, Ny=100;

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


  GLfloat *texdata = new GLfloat[Nx*Ny*4];

  for (int i=0; i<Nx; ++i) {
    for (int j=0; j<Ny; ++j) {
      texdata[(i*Ny + j)*4 + 0] = 0.1;
      texdata[(i*Ny + j)*4 + 1] = sin(5.0*i/Nx + 5.0*j/Ny);
      texdata[(i*Ny + j)*4 + 2] = cos(5.0*i/Nx + 5.0*j/Ny);
      texdata[(i*Ny + j)*4 + 3] = 1.0;
    }
  }

  GLuint TextureMap;
  glGenTextures(1, &TextureMap);
  glBindTexture(GL_TEXTURE_2D, TextureMap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, Nx, Ny, 0, GL_RGBA, GL_FLOAT, texdata);
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


  glDeleteTextures(1, &TextureMap);
  delete [] texdata;

  glDeleteTextures(1, &texId);
  glDeleteFramebuffers(1, &fboId);
  glDeleteRenderbuffers(1, &rboId);
}




void VolumeRendering::draw_local1()
{
  int Nx=100, Ny=100;

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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Nx, Ny, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, 0);
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

  // clear buffers
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPolygonMode(GL_FRONT, GL_FILL);
  glBegin(GL_POLYGON);
  glColor3f(1,0,0); glVertex2f(0,0);
  glColor3f(1,0,1); glVertex2f(0,1);
  glColor3f(1,1,1); glVertex2f(1,1);
  glColor3f(1,1,0); glVertex2f(1,0);
  glEnd();

  // unbind FBO
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
  // Drawing into the texture
  // ---------------------------------------------------------------------------
  glDeleteTextures(1, &texId);
  glDeleteRenderbuffers(1, &rboId);
  glDeleteFramebuffers(1, &fboId);
}

