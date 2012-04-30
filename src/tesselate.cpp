
/* -----------------------------------------------------------------------------
 *
 * AUTHOR: Jonathan Zrake, NYU CCPP: zrake@nyu.edu
 *
 *
 * USAGE: $> ./tetrun [number of trials]
 *
 *
 * DESCRIPTION:
 *
 * Example code with demonstrates how to use tetgen to generate a Delaunay and
 * Voronoi diagram in 3d.
 *
 * tetgen home: http://wias-berlin.de/software/tetgen/index.html
 * tetgen manual: http://wias-berlin.de/software/tetgen/files/tetgen-manual.pdf
 *
 *
 * NOTES:
 *
 * The tetgen library is not valgrind safe. That is not to say that it generates
 * memory leaks, it does not when used correctly. The first time run_test() is
 * called valgrind believes to have found uninitialized values. After many more
 * calls (31 on my mac) the tetgen library hits an assertion error. This does
 * not happen when valgrind was not used to invoke the executable.
 *
 * -----------------------------------------------------------------------------
 */


#define TETLIBRARY
#include "tetgen.h"
#include "luview.hpp"


Tesselation3D::Tesselation3D()
{
  gl_modes.push_back(GL_LIGHTING);
  gl_modes.push_back(GL_LIGHT0);
  gl_modes.push_back(GL_BLEND);
  gl_modes.push_back(GL_COLOR_MATERIAL);
  gl_modes.push_back(GL_NORMALIZE);

  Np = 360;
  pointlist = new double[Np * 3];
  for (int n=0; n<Np; ++n) {
    pointlist[3*n + 0] = 0.5 - 1.0*rand() / RAND_MAX;
    pointlist[3*n + 1] = 0.5 - 1.0*rand() / RAND_MAX;
    pointlist[3*n + 2] = 0.5 - 1.0*rand() / RAND_MAX;
  }
}
Tesselation3D::~Tesselation3D()
{
  delete [] pointlist;
}

void Tesselation3D::draw_local()
{
  tetgenio inp, out;

  inp.initialize();
  out.initialize();

  inp.numberofpoints = Np;
  inp.pointlist = new double[inp.numberofpoints * 3];
  memcpy(inp.pointlist, pointlist, Np*3*sizeof(double));

  // v: generate voronoi
  // Q: quiet
  // ee: generate edges (NOTE: e -> subedges breaks)
  tetrahedralize("veeQ", &inp, &out);

  GLfloat mat_diffuse[] = { 0.3, 0.6, 0.7, 0.8 };
  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.8 };
  GLfloat mat_shininess[] = { 100.0 };

  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  glBegin(GL_TRIANGLES);
  for (int n=0; n<out.numberoftrifaces; ++n) {
    int n0 = out.trifacelist[3*n + 0];
    int n1 = out.trifacelist[3*n + 1];
    int n2 = out.trifacelist[3*n + 2];
    REAL *u = &out.pointlist[3*n0];
    REAL *v = &out.pointlist[3*n1];
    REAL *w = &out.pointlist[3*n2];
    REAL normal[3];
    compute_normal(u, v, w, normal);
    glNormal3dv(normal);
    glVertex3dv(u);
    glVertex3dv(v);
    glVertex3dv(w);
  }
  glEnd();

  glBegin(GL_LINES);
  for (int n=0; n<out.numberofedges; ++n) {
    int n0 = out.edgelist[2*n + 0];
    int n1 = out.edgelist[2*n + 1];
    REAL *u = &out.pointlist[3*n0];
    REAL *v = &out.pointlist[3*n1];
    REAL normal[3] = {u[0]-v[0], u[1]-v[1], u[2]-v[2]};
    glNormal3dv(normal);
    glVertex3dv(u);
    glVertex3dv(v);
  }
  glEnd();
}

void Tesselation3D::compute_normal(double *u, double *v, double *w, double *n)
{
  double uv[3] = {v[0] - u[0], v[1] - u[1], v[2] - u[2]};
  double uw[3] = {w[0] - u[0], w[1] - u[1], w[2] - u[2]};

  n[0] = uv[1]*uw[2] - uv[2]*uw[1];
  n[1] = uv[2]*uw[0] - uv[0]*uw[2];
  n[2] = uv[0]*uw[1] - uv[1]*uw[0];
}







  /*
  int n;

  n=0;
  pointlist[3*n + 0] = -0.5;
  pointlist[3*n + 1] = -0.5;
  pointlist[3*n + 2] = -0.5;

  n=1;
  pointlist[3*n + 0] = -0.5;
  pointlist[3*n + 1] = -0.5;
  pointlist[3*n + 2] = +0.5;

  n=2;
  pointlist[3*n + 0] = -0.5;
  pointlist[3*n + 1] = +0.5;
  pointlist[3*n + 2] = -0.5;

  n=3;
  pointlist[3*n + 0] = -0.5;
  pointlist[3*n + 1] = +0.5;
  pointlist[3*n + 2] = +0.5;

  n=4;
  pointlist[3*n + 0] = +0.5;
  pointlist[3*n + 1] = -0.5;
  pointlist[3*n + 2] =  0.0;

  n=5;
  pointlist[3*n + 0] = +0.5;
  pointlist[3*n + 1] = +0.5;
  pointlist[3*n + 2] =  0.0;
  */



  /*
    glBegin(GL_LINES);
    for (int n=0; n<out.numberofvedges; ++n) {
    tetgenio::voroedge e = out.vedgelist[n];
    if (e.v2 == -1) continue; // -1 indicates ray
    int n0 = e.v1;
    int n1 = e.v2;
    REAL *u = &out.vpointlist[3*n0];
    REAL *v = &out.vpointlist[3*n1];
    glVertex3dv(u);
    glVertex3dv(v);
    }
    glEnd();
  */
