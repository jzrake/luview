

#include "luview.hpp"
#include <cmath>

template <class T> static void draw_cylinder(const T *x0, const T *x1, T rad0, T rad1)
{
  T r[3] = {x1[0] - x0[0], x1[1] - x0[1], x1[2] - x0[2]};
  T mag = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
  r[0] /= mag;
  r[1] /= mag;
  r[2] /= mag;

  T a[3], zhat[3] = { 0, 0, 1 };
  a[0] = zhat[1]*r[2] - zhat[2]*r[1];
  a[1] = zhat[2]*r[0] - zhat[0]*r[2];
  a[2] = zhat[0]*r[1] - zhat[1]*r[0];

  T angle = acos(r[2]) * 180.0 / M_PI;

  glPushMatrix();
  glTranslated(x0[0], x0[1], x0[2]);
  glRotated(angle, a[0], a[1], a[2]);

  GLUquadric *quad = gluNewQuadric();
  gluCylinder(quad, rad0, rad1, mag, 72, 1);
  gluDeleteQuadric(quad);

  glPopMatrix();
}


BoundingBox::BoundingBox()
{
  gl_modes.push_back(GL_DEPTH_TEST);
  gl_modes.push_back(GL_LIGHTING);
  gl_modes.push_back(GL_LIGHT0);
  gl_modes.push_back(GL_BLEND);
  gl_modes.push_back(GL_COLOR_MATERIAL);
  gl_modes.push_back(GL_AUTO_NORMAL);
  gl_modes.push_back(GL_NORMALIZE);
}

void BoundingBox::draw_local()
{
  GLfloat x0[3], x1[3];
  GLfloat lw = LineWidth * 0.01;

  x0[0] = -0.5; x0[1] = -0.5; x0[2] = -0.5;
  x1[0] = +0.5; x1[1] = -0.5; x1[2] = -0.5;
  draw_cylinder<GLfloat>(x0, x1, lw, lw);
  x0[0] = -0.5; x0[1] = -0.5; x0[2] = +0.5;
  x1[0] = +0.5; x1[1] = -0.5; x1[2] = +0.5;
  draw_cylinder<GLfloat>(x0, x1, lw, lw);
  x0[0] = -0.5; x0[1] = +0.5; x0[2] = -0.5;
  x1[0] = +0.5; x1[1] = +0.5; x1[2] = -0.5;
  draw_cylinder<GLfloat>(x0, x1, lw, lw);
  x0[0] = -0.5; x0[1] = +0.5; x0[2] = +0.5;
  x1[0] = +0.5; x1[1] = +0.5; x1[2] = +0.5;
  draw_cylinder<GLfloat>(x0, x1, lw, lw);

  x0[0] = -0.5; x0[1] = -0.5; x0[2] = -0.5;
  x1[0] = -0.5; x1[1] = +0.5; x1[2] = -0.5;
  draw_cylinder<GLfloat>(x0, x1, lw, lw);
  x0[0] = +0.5; x0[1] = -0.5; x0[2] = -0.5;
  x1[0] = +0.5; x1[1] = +0.5; x1[2] = -0.5;
  draw_cylinder<GLfloat>(x0, x1, lw, lw);
  x0[0] = -0.5; x0[1] = -0.5; x0[2] = +0.5;
  x1[0] = -0.5; x1[1] = +0.5; x1[2] = +0.5;
  draw_cylinder<GLfloat>(x0, x1, lw, lw);
  x0[0] = +0.5; x0[1] = -0.5; x0[2] = +0.5;
  x1[0] = +0.5; x1[1] = +0.5; x1[2] = +0.5;
  draw_cylinder<GLfloat>(x0, x1, lw, lw);

  x0[0] = -0.5; x0[1] = -0.5; x0[2] = -0.5;
  x1[0] = -0.5; x1[1] = -0.5; x1[2] = +0.5;
  draw_cylinder<GLfloat>(x0, x1, lw, lw);
  x0[0] = -0.5; x0[1] = +0.5; x0[2] = -0.5;
  x1[0] = -0.5; x1[1] = +0.5; x1[2] = +0.5;
  draw_cylinder<GLfloat>(x0, x1, lw, lw);
  x0[0] = +0.5; x0[1] = -0.5; x0[2] = -0.5;
  x1[0] = +0.5; x1[1] = -0.5; x1[2] = +0.5;
  draw_cylinder<GLfloat>(x0, x1, lw, lw);
  x0[0] = +0.5; x0[1] = +0.5; x0[2] = -0.5;
  x1[0] = +0.5; x1[1] = +0.5; x1[2] = +0.5;
  draw_cylinder<GLfloat>(x0, x1, lw, lw);
}


ImagePlane::ImagePlane()
{
  gl_modes.push_back(GL_DEPTH_TEST);
  gl_modes.push_back(GL_LIGHTING);
  gl_modes.push_back(GL_LIGHT0);
  gl_modes.push_back(GL_BLEND);
  gl_modes.push_back(GL_COLOR_MATERIAL);
  gl_modes.push_back(GL_NORMALIZE);
  gl_modes.push_back(GL_TEXTURE_1D);
  gl_modes.push_back(GL_TEXTURE_2D);

  Lx0 = -0.5;
  Lx1 = +0.5;
  Ly0 = -0.5;
  Ly1 = +0.5;
}

void ImagePlane::draw_local()
{
  EntryDS im = DataSources.find("image");
  EntryDS cm = DataSources.find("color_table");

  if (shader) {
    shader->set_uniform("tex1", 0);
    shader->set_uniform("tex2d", 2);
  }
  if (im != DataSources.end()) {
    glActiveTexture(GL_TEXTURE0 + 2);
    im->second->compile();
    im->second->check_has_data("image");
    im->second->become_texture();
  }
  else {
    return;
  }
  if (cm != DataSources.end()) {
    glActiveTexture(GL_TEXTURE0 + 0);
    cm->second->compile();
    cm->second->check_has_data("color_table");
    cm->second->check_num_dimensions("color_table", 2);
    cm->second->check_num_points("color_table", 256, 0);
    cm->second->check_num_points("color_table", 4, 1);
    cm->second->become_texture();
  }

  glBegin(GL_QUADS);
  glNormal3f(0, 0, 1);
  glTexCoord2f(0, 0); glVertex3f(Lx0, Ly0, 0);
  glTexCoord2f(0, 1); glVertex3f(Lx0, Ly1, 0);
  glTexCoord2f(1, 1); glVertex3f(Lx1, Ly1, 0);
  glTexCoord2f(1, 0); glVertex3f(Lx1, Ly0, 0);
  glEnd();
}


SegmentsEnsemble::SegmentsEnsemble()
{
  gl_modes.push_back(GL_DEPTH_TEST);
  gl_modes.push_back(GL_LIGHTING);
  gl_modes.push_back(GL_LIGHT0);
  gl_modes.push_back(GL_BLEND);
  gl_modes.push_back(GL_COLOR_MATERIAL);
  gl_modes.push_back(GL_AUTO_NORMAL);
  gl_modes.push_back(GL_NORMALIZE);
}

void SegmentsEnsemble::draw_local()
{
  EntryDS seg = DataSources.find("segments");

  if (seg != DataSources.end()) {
    seg->second->compile();
    seg->second->check_has_data("segments");
    seg->second->check_has_indices("segments");
    seg->second->check_num_dimensions("segments", 2);
  }
  else {
    return;
  }

  const GLfloat *verts = seg->second->get_data();
  const GLuint *indices = seg->second->get_indices();
  const int Np = seg->second->get_num_indices() / 2;

  int cyl = 0;
  if (cyl) {
    for (int n=0; n<Np; ++n) {
      const GLfloat *u = &verts[3*indices[2*n + 0]];
      const GLfloat *v = &verts[3*indices[2*n + 1]];
      draw_cylinder<GLfloat>(u, v, 0.01*LineWidth, 0.01*LineWidth);
    }
  }
  else {
    glBegin(GL_LINES);
    for (int n=0; n<Np; ++n) {
      const GLfloat *u = &verts[3*indices[2*n + 0]];
      const GLfloat *v = &verts[3*indices[2*n + 1]];
      const GLfloat n[3] = {v[0]-u[0], v[1]-u[1], v[2]-u[2]};
      glVertex3fv(u);
      glVertex3fv(v);
      glNormal3fv(n);
    }
    glEnd();
  }
}


TrianglesEnsemble::TrianglesEnsemble()
{
  gl_modes.push_back(GL_DEPTH_TEST);
  gl_modes.push_back(GL_LIGHTING);
  gl_modes.push_back(GL_LIGHT0);
  gl_modes.push_back(GL_BLEND);
  gl_modes.push_back(GL_COLOR_MATERIAL);
  gl_modes.push_back(GL_NORMALIZE);
}

void TrianglesEnsemble::draw_local()
{
  EntryDS tri = DataSources.find("triangles");

  if (tri != DataSources.end()) {
    //    printf("%s::%s (compiling triangles)\n", _get_type().c_str(), __FUNCTION__);
    tri->second->compile();
    //    printf("%s::%s (done)\n", _get_type().c_str(), __FUNCTION__);
    tri->second->check_has_data("triangles");
    tri->second->check_has_indices("triangles");
    tri->second->check_num_dimensions("triangles", 2);
  }
  else {
    return;
  }

  //  printf("%s::%s (really done)\n", _get_type().c_str(), __FUNCTION__);

  const GLfloat *verts = tri->second->get_data();
  const GLuint *indices = tri->second->get_indices();
  const int Np = tri->second->get_num_indices() / 3;

  glBegin(GL_TRIANGLES);
  for (int n=0; n<Np; ++n) {
    //    printf("%s::%s (B)\n", _get_type().c_str(), __FUNCTION__);

    const GLfloat *u = &verts[3*indices[3*n + 0]];
    const GLfloat *v = &verts[3*indices[3*n + 1]];
    const GLfloat *w = &verts[3*indices[3*n + 2]];

    const GLfloat n1[3] = {v[0]-u[0], v[1]-u[1], v[2]-u[2]};
    const GLfloat n2[3] = {w[0]-v[0], w[1]-v[1], w[2]-v[2]};

    GLfloat n[3];
    n[0] = n1[2]*n2[1] - n1[1]*n2[2];
    n[1] = n1[0]*n2[2] - n1[2]*n2[0];
    n[2] = n1[1]*n2[0] - n1[0]*n2[1];

    glNormal3fv(n);
    glVertex3fv(u);
    glVertex3fv(v);
    glVertex3fv(w);
  }
  glEnd();
  //  printf("%s::%s (done drawing)\n", _get_type().c_str(), __FUNCTION__);
}

