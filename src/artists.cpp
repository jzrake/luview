

#include "luview.hpp"
#include <cmath>

static void draw_cylinder(const GLfloat *x0, const GLfloat *x1,
			  GLfloat rad0, GLfloat rad1, int qual)
{
  GLfloat r[3] = {x1[0] - x0[0], x1[1] - x0[1], x1[2] - x0[2]};
  GLfloat mag = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
  r[0] /= mag;
  r[1] /= mag;
  r[2] /= mag;

  GLfloat a[3], zhat[3] = { 0, 0, 1 };
  a[0] = zhat[1]*r[2] - zhat[2]*r[1];
  a[1] = zhat[2]*r[0] - zhat[0]*r[2];
  a[2] = zhat[0]*r[1] - zhat[1]*r[0];

  GLfloat angle = acos(r[2]) * 180.0 / M_PI;

  glPushMatrix();
  glTranslated(x0[0], x0[1], x0[2]);
  glRotated(angle, a[0], a[1], a[2]);

  GLUquadric *quad = gluNewQuadric();
  gluCylinder(quad, rad0, rad1, mag, qual, 1);
  gluDeleteQuadric(quad);

  glPopMatrix();
}

static void draw_sphere(const GLfloat *x0, GLfloat rad, int qual)
{
  GLUquadric *quad = gluNewQuadric();
  glPushMatrix();
  glTranslated(x0[0], x0[1], x0[2]);
  gluSphere(quad, rad, qual, qual);
  glPopMatrix();
  gluDeleteQuadric(quad);
}

ParameterizedPathArtist::ParameterizedPathArtist()
{
   gl_modes.push_back(GL_DEPTH_TEST);
  gl_modes.push_back(GL_LIGHTING);
  gl_modes.push_back(GL_LIGHT0);
  gl_modes.push_back(GL_BLEND);
  gl_modes.push_back(GL_COLOR_MATERIAL);
  gl_modes.push_back(GL_AUTO_NORMAL);
  gl_modes.push_back(GL_NORMALIZE); 
}
void ParameterizedPathArtist::draw_local()
{
  EntryDS cp = DataSources.find("points");

  if (cp != DataSources.end()) {
    glActiveTexture(GL_TEXTURE0 + 2);
    cp->second->compile();
    cp->second->check_has_data("path");
    cp->second->check_num_dimensions("path", 2);
    cp->second->check_num_points("path", 3, 1);
  }
  else {
    return;
  }

  int Np = cp->second->get_num_points(0);
  const GLfloat *points = cp->second->get_data();
  GLfloat lw = LineWidth * 0.01;

  for (int n=0; n<Np-1; ++n) {
    draw_sphere(&points[3*n], lw, 12);
    draw_cylinder(&points[3*n], &points[3*(n+1)], lw, lw, 24);
  }
  draw_sphere(&points[3*(Np-1)], lw, 12);
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
  draw_cylinder(x0, x1, lw, lw, 72);
  draw_sphere(x0, lw, 72);
  draw_sphere(x1, lw, 72);
  x0[0] = -0.5; x0[1] = -0.5; x0[2] = +0.5;
  x1[0] = +0.5; x1[1] = -0.5; x1[2] = +0.5;
  draw_cylinder(x0, x1, lw, lw, 72);
  draw_sphere(x0, lw, 72);
  draw_sphere(x1, lw, 72);
  x0[0] = -0.5; x0[1] = +0.5; x0[2] = -0.5;
  x1[0] = +0.5; x1[1] = +0.5; x1[2] = -0.5;
  draw_cylinder(x0, x1, lw, lw, 72);
  draw_sphere(x0, lw, 72);
  draw_sphere(x1, lw, 72);
  x0[0] = -0.5; x0[1] = +0.5; x0[2] = +0.5;
  x1[0] = +0.5; x1[1] = +0.5; x1[2] = +0.5;
  draw_cylinder(x0, x1, lw, lw, 72);
  draw_sphere(x0, lw, 72);
  draw_sphere(x1, lw, 72);

  x0[0] = -0.5; x0[1] = -0.5; x0[2] = -0.5;
  x1[0] = -0.5; x1[1] = +0.5; x1[2] = -0.5;
  draw_cylinder(x0, x1, lw, lw, 72);
  x0[0] = +0.5; x0[1] = -0.5; x0[2] = -0.5;
  x1[0] = +0.5; x1[1] = +0.5; x1[2] = -0.5;
  draw_cylinder(x0, x1, lw, lw, 72);
  x0[0] = -0.5; x0[1] = -0.5; x0[2] = +0.5;
  x1[0] = -0.5; x1[1] = +0.5; x1[2] = +0.5;
  draw_cylinder(x0, x1, lw, lw, 72);
  x0[0] = +0.5; x0[1] = -0.5; x0[2] = +0.5;
  x1[0] = +0.5; x1[1] = +0.5; x1[2] = +0.5;
  draw_cylinder(x0, x1, lw, lw, 72);

  x0[0] = -0.5; x0[1] = -0.5; x0[2] = -0.5;
  x1[0] = -0.5; x1[1] = -0.5; x1[2] = +0.5;
  draw_cylinder(x0, x1, lw, lw, 72);
  x0[0] = -0.5; x0[1] = +0.5; x0[2] = -0.5;
  x1[0] = -0.5; x1[1] = +0.5; x1[2] = +0.5;
  draw_cylinder(x0, x1, lw, lw, 72);
  x0[0] = +0.5; x0[1] = -0.5; x0[2] = -0.5;
  x1[0] = +0.5; x1[1] = -0.5; x1[2] = +0.5;
  draw_cylinder(x0, x1, lw, lw, 72);
  x0[0] = +0.5; x0[1] = +0.5; x0[2] = -0.5;
  x1[0] = +0.5; x1[1] = +0.5; x1[2] = +0.5;
  draw_cylinder(x0, x1, lw, lw, 72);
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
    shader->set_uniform("tex1d", 0);
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

ParametricSurface::ParametricSurface()
{
  gl_modes.push_back(GL_DEPTH_TEST);
  gl_modes.push_back(GL_LIGHTING);
  gl_modes.push_back(GL_LIGHT0);
  gl_modes.push_back(GL_BLEND);
  gl_modes.push_back(GL_COLOR_MATERIAL);
  gl_modes.push_back(GL_AUTO_NORMAL);
  gl_modes.push_back(GL_NORMALIZE);
}
void ParametricSurface::draw_local()
{
  EntryDS cp = DataSources.find("points");

  if (cp != DataSources.end()) {
    cp->second->compile();
    cp->second->check_has_data("points");
    cp->second->check_num_dimensions("points", 3);
    cp->second->check_num_points("points", 3, 2); // 3 components on dimension 2
  }
  else {
    return;
  }
  int Nx = cp->second->get_num_points(0);
  int Ny = cp->second->get_num_points(1);
  const GLfloat *data = cp->second->get_data();

  glBegin(GL_TRIANGLES);
  for (int i=0; i<Nx-1; ++i) {
    for (int j=0; j<Ny-1; ++j) {
      const GLfloat *u = &data[((i+0)*Ny + j+0)*3];
      const GLfloat *v = &data[((i+0)*Ny + j+1)*3];
      const GLfloat *w = &data[((i+1)*Ny + j+0)*3];
      const GLfloat *q = &data[((i+1)*Ny + j+1)*3];

      const GLfloat d1[3] = {v[0]-u[0], v[1]-u[1], v[2]-u[2]};
      const GLfloat d2[3] = {w[0]-v[0], w[1]-v[1], w[2]-v[2]};
      const GLfloat d3[3] = {q[0]-w[0], q[1]-w[1], q[2]-w[2]};

      GLfloat n1[3];
      n1[0] = d1[2]*d2[1] - d1[1]*d2[2];
      n1[1] = d1[0]*d2[2] - d1[2]*d2[0];
      n1[2] = d1[1]*d2[0] - d1[0]*d2[1];

      GLfloat n2[3];
      n2[0] = d3[2]*d2[1] - d3[1]*d2[2];
      n2[1] = d3[0]*d2[2] - d3[2]*d2[0];
      n2[2] = d3[1]*d2[0] - d3[0]*d2[1];

      glNormal3fv(n1);
      glVertex3fv(u);
      glVertex3fv(v);
      glVertex3fv(w);

      glNormal3fv(n2);
      glVertex3fv(v);
      glVertex3fv(w);
      glVertex3fv(q);
    }
  }
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
      draw_cylinder(u, v, 0.01*LineWidth, 0.01*LineWidth, 24);
    }
  }
  else {
    glBegin(GL_LINES);
    for (int n=0; n<Np; ++n) {
      const GLfloat *u = &verts[3*indices[2*n + 0]];
      const GLfloat *v = &verts[3*indices[2*n + 1]];
      const GLfloat n[3] = {v[0]-u[0], v[1]-u[1], v[2]-u[2]};
      glNormal3fv(n);
      glVertex3fv(u);
      glVertex3fv(v);
    }
    glEnd();
  }
}


TrianglesEnsemble::TrianglesEnsemble()
{
  //  gl_modes.push_back(GL_CULL_FACE);
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
  EntryDS nrm = DataSources.find("normals");
  EntryDS lut = DataSources.find("color_table");
  EntryDS sca = DataSources.find("scalars");

  if (shader) {
    shader->set_uniform("tex1d", 0);
  }
  if (tri != DataSources.end()) {
    tri->second->compile();
    tri->second->check_has_data("triangles");
    tri->second->check_has_indices("triangles");
    tri->second->check_num_dimensions("triangles", 2);
  }
  else {
    return;
  }
  if (nrm != DataSources.end()) {
    nrm->second->compile();
    nrm->second->check_has_data("normals");
    nrm->second->check_has_indices("normals");
    nrm->second->check_num_dimensions("normals", 2);
  }
  if (sca != DataSources.end()) {
    sca->second->compile();
    sca->second->check_has_data("scalars");
  }
  if (lut != DataSources.end()) {
    glActiveTexture(GL_TEXTURE0 + 0);
    lut->second->compile();
    lut->second->check_has_data("color_table");
    lut->second->check_num_dimensions("color_table", 2);
    lut->second->check_num_points("color_table", 256, 0);
    lut->second->check_num_points("color_table", 4, 1);
    lut->second->become_texture();
  }

  const GLfloat *verts = tri->second->get_data();
  const GLuint *indices = tri->second->get_indices();
  const int Np = tri->second->get_num_indices(); // number of indices

  if (nrm != DataSources.end() && sca != DataSources.end()) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Note: glRangeDrawElements might be faster according to
    // http://www.spec.org/gwpg/gpc.static/vbo_whitepaper.html
    glBindBuffer(GL_ARRAY_BUFFER, tri->second->get_vbo());
    glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, nrm->second->get_vbo());
    glNormalPointer(GL_FLOAT, 3*sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, sca->second->get_vbo());
    glTexCoordPointer(1, GL_FLOAT, sizeof(GLfloat), 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tri->second->get_ibo());
    glDrawElements(GL_TRIANGLES, Np, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
  else {
    glBegin(GL_TRIANGLES);
    for (int n=0; n<Np/3; ++n) {
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
      glVertex3fv(v);
      glVertex3fv(u);
      glVertex3fv(w);
    }
    glEnd();
  }
}

PointsEnsemble::PointsEnsemble()
{
  gl_modes.push_back(GL_BLEND);
  gl_modes.push_back(GL_POINT_SPRITE);
  gl_modes.push_back(GL_TEXTURE_2D);

  Orientation[0] = -90.0;
  glGenTextures(1, &sprite_texture);
}
PointsEnsemble::~PointsEnsemble()
{
  glDeleteTextures(1, &sprite_texture);
}


void PointsEnsemble::load_sprite()
{
  int N = 32;
  double del = 2.0 / N;
  unsigned char *data = new unsigned char[N*N];
  for (int i=0; i<N; ++i) {
    for (int j=0; j<N; ++j) {
      int offs = i + j*N;
      double x = (i - N/2) * del;
      double y = (j - N/2) * del;
      double a = 1.0 - hypot(x, y);
      data[offs] = 255 * Alpha * (a > 0.0 ? a : 0.0);
    }
  }
  glBindTexture(GL_TEXTURE_2D, sprite_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, N, N, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
  glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  delete [] data;
}
void PointsEnsemble::draw_local()
{
  EntryDS cp = DataSources.find("points");
  EntryDS sz = DataSources.find("sizes");

  if (cp != DataSources.end()) {
    cp->second->compile();
    cp->second->check_has_data("points");
    cp->second->check_num_dimensions("points", 2);
    cp->second->check_num_points("points", 3, 1);
  }
  else {
    return;
  }
  int Np = cp->second->get_num_points(0);
  GLuint *ind = (GLuint*) malloc(Np*sizeof(GLuint));
  for (int n=0; n<Np; ++n) ind[n] = n;

  if (sz != DataSources.end()) {
    sz->second->compile();
    sz->second->check_has_data("sizes");
    sz->second->check_num_dimensions("sizes", 1);
    sz->second->check_num_points("points", Np, 0);
  }
  load_sprite();

  if (sz == DataSources.end()) {
    // -------------------------------------------------------------------------
    // If no point sizes were given, then we can use a uniform point size, and a
    // vertex array.
    // -------------------------------------------------------------------------
    glPointSize(LineWidth);
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, cp->second->get_vbo());
    glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), 0);
    glDrawElements(GL_POINTS, Np, GL_UNSIGNED_INT, ind);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
  else {
    // -------------------------------------------------------------------------
    // Otherwise, we'll have to render the points one-by-one. glPointSize can
    // only be called outside of glBegin and glEnd.
    // -------------------------------------------------------------------------
    const GLfloat *points = cp->second->get_data();
    const GLfloat *sizes = sz->second->get_data();
    for (int n=0; n<Np; ++n) {
      glPointSize(sizes[n]);
      glBegin(GL_POINTS);
      glVertex3fv(points + 3*n);
      glEnd();
    }
  }
}
