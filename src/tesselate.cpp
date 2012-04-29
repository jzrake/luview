
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


void Tesselation3D::draw_local()
{
  int n;
  tetgenio inp, out;

  inp.initialize();
  out.initialize();

  inp.numberofpoints = 4;
  inp.pointlist = new REAL[inp.numberofpoints * 3];

  n = 0;
  inp.pointlist[3*n + 0] = 0.0;
  inp.pointlist[3*n + 1] = 1.0;
  inp.pointlist[3*n + 2] = 0.0;

  n = 1;
  inp.pointlist[3*n + 0] =-0.5;
  inp.pointlist[3*n + 1] = 0.0;
  inp.pointlist[3*n + 2] = 0.0;

  n = 2;
  inp.pointlist[3*n + 0] =+0.5;
  inp.pointlist[3*n + 1] = 0.0;
  inp.pointlist[3*n + 2] = 0.0;

  n = 3;
  inp.pointlist[3*n + 0] = 0.0;
  inp.pointlist[3*n + 1] = 0.25;
  inp.pointlist[3*n + 2] = 1.0;

  // v: generate voronoi
  // Q: quiet
  // ee: generate edges (NOTE: e -> subedges breaks)
  tetrahedralize("veeQ", &inp, &out);

  std::cout << "Delaunay information:" << std::endl;
  std::cout << "numberofpoints: " << out.numberofpoints << std::endl;
  std::cout << "numberofedges: " << out.numberofedges << std::endl;
  std::cout << "numberoftrifaces: " << out.numberoftrifaces << std::endl;
  std::cout << "numberoftetrahedra: " << out.numberoftetrahedra << std::endl;
  std::cout << "numberofcorners: " << out.numberofcorners << std::endl;
  std::cout << "edges are:" << std::endl;
  for (int n=0; n<out.numberofedges; ++n) {
    int n0 = out.edgelist[2*n + 0];
    int n1 = out.edgelist[2*n + 1];
    REAL *u = &out.pointlist[3*n0];
    REAL *v = &out.pointlist[3*n1];
    printf("(%f %f %f) -> (%f %f %f)\n", u[0], u[1], u[2], v[0], v[1], v[2]);
  }

  std::cout << "Voronoi information:" << std::endl;
  std::cout << "numberofvpoints: " << out.numberofvpoints << std::endl;
  std::cout << "numberofvedges: " << out.numberofvedges << std::endl;
  std::cout << "numberofvfacets: " << out.numberofvfacets << std::endl;
  std::cout << "numberofvcells: " << out.numberofvcells << std::endl;
  std::cout << "edges are:" << std::endl;
  for (int n=0; n<out.numberofvedges; ++n) {
    tetgenio::voroedge e =  out.vedgelist[n];
    int n0 = e.v1;
    int n1 = e.v2;
    REAL *u = &out.vpointlist[3*n0];
    REAL *v = n1 == -1 ? e.vnormal : &out.vpointlist[3*n1]; // -1 indicates ray
    printf("(%f %f %f) -> (%f %f %f)\n", u[0], u[1], u[2], v[0], v[1], v[2]);
  }
}
