
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

}

Tesselation3D::~Tesselation3D()
{

}

int Tesselation3D::get_num_points(int d)
{
  switch (d) {
  case 0: return Np;
  default: return 0;
  }
}
int Tesselation3D::get_size() { return Np; }
int Tesselation3D::get_num_components() { return 3; }
int Tesselation3D::get_num_dimensions() { return 1; }



GLfloat *Tesselation3D::get_data()
{
  if (!input) {
    return NULL;
  }
  tetgenio inp, out;
  inp.initialize();
  out.initialize();

  inp.numberofpoints = input->get_size();
  inp.pointlist = new double[inp.numberofpoints*3];

  GLfloat *points = input->get_data();
  for (int n=0; n<inp.numberofpoints*3; ++n) inp.pointlist[n] = points[n];

  // z: number indices from zero
  // v: generate voronoi
  // Q: quiet
  // ee: generate edges (NOTE: e -> subedges breaks)
  tetrahedralize("zveeQ", &inp, &out);

  // super returns these on get_size() and get_indices
  Np = out.numberofedges;
  indices = (GLuint*) realloc(indices, 2 * out.numberofedges * sizeof(GLuint));
  output = (GLfloat*) realloc(output, 3 * out.numberofpoints * sizeof(GLfloat));

  for (int n=0; n<2*out.numberofedges; ++n) {
    indices[n] = out.edgelist[n];
  }
  for (int n=0; n<3*out.numberofpoints; ++n) {
    output[n] = out.pointlist[n];
  }

  delete [] inp.pointlist;
  inp.pointlist = NULL;

  return output;
}


Tesselation3D::LuaInstanceMethod Tesselation3D::__getattr__(std::string &method_name)
{
  AttributeMap attr;
  RETURN_ATTR_OR_CALL_SUPER(PointsSource);
}
