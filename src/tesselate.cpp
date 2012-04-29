
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


#include <iostream>

#define NUMBER_TRIALS 1 // try >30 with valgrind to observe crash
#define TETLIBRARY
#include "tetgen.h"

#include "luview.hpp"

Tesselation3D::Tesselation3D()
{
  Np = 36;
  pointlist = new double[Np * 3];

  for (int n=0; n<Np; ++n) {
    pointlist[3*n + 0] = 0.5 - 1.0*rand() / RAND_MAX;
    pointlist[3*n + 1] = 0.5 - 1.0*rand() / RAND_MAX;
    pointlist[3*n + 2] = 0.5 - 1.0*rand() / RAND_MAX;
  }
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
  /*
  glBegin(GL_LINES);
  for (int n=0; n<out.numberofedges; ++n) {
    int n0 = out.edgelist[2*n + 0];
    int n1 = out.edgelist[2*n + 1];
    REAL *u = &out.pointlist[3*n0];
    REAL *v = &out.pointlist[3*n1];
    glVertex3dv(u);
    glVertex3dv(v);
  }
  glEnd();
  */
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

}
