
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
#include "tetgen/tetgen.h"
#include "luview.hpp"

static tetgenio inp, out;

Tesselation3D::Tesselation3D()
{

}

Tesselation3D::~Tesselation3D()
{

}


void Tesselation3D::__refresh_cpu()
{
  inp.initialize();
  out.initialize();

  inp.load_poly("/Users/jzrake/Work/luview/data/balls3astr_12_16");
  //  inp.load_node("/Users/jzrake/Work/luview/data/brain");

  // z: number indices from zero
  // v: generate voronoi
  // Q: quiet
  // ee: generate edges (NOTE: e -> subedges breaks)
  tetrahedralize("zveeQ", &inp, &out);

  GLuint *indices = new GLuint[3 * out.numberofedges];
  GLfloat *verts = new GLfloat[3 * out.numberofpoints];

  for (int n=0; n<3*out.numberoftrifaces; ++n) {
    indices[n] = out.trifacelist[n];
  }
  for (int n=0; n<3*out.numberofpoints; ++n) {
    verts[n] = out.pointlist[n];
  }

  int N[2] = { out.numberofpoints, 3 };
  this->set_data(verts, N, 2);
  this->set_indices(indices, 3*out.numberofedges);

  delete [] verts;
  delete [] indices;
}


void Tesselation3D::__refresh_cpu1()
{
  inp.initialize();
  out.initialize();

  //  inp.load_plc("/Users/jzrake/Work/luview/data/hinge", tetgenbehavior::MESH);
  //  inp.load_poly("/Users/jzrake/Work/luview/data/balls3astr_12_16");
  inp.load_node("/Users/jzrake/Work/luview/data/brain");

  // z: number indices from zero
  // v: generate voronoi
  // Q: quiet
  // ee: generate edges (NOTE: e -> subedges breaks)
  tetrahedralize("zveeQ", &inp, &out);

  GLuint *indices = new GLuint[2 * out.numberofedges];
  GLfloat *verts = new GLfloat[3 * out.numberofpoints];

  for (int n=0; n<2*out.numberofedges; ++n) {
    indices[n] = out.edgelist[n];
  }
  for (int n=0; n<3*out.numberofpoints; ++n) {
    verts[n] = out.pointlist[n];
  }

  int N[2] = { out.numberofpoints, 3 };
  this->set_data(verts, N, 2);
  this->set_indices(indices, 2*out.numberofedges);

  delete [] verts;
  delete [] indices;
}


Tesselation3D::LuaInstanceMethod Tesselation3D::__getattr__(std::string &method_name)
{
  AttributeMap attr;
  RETURN_ATTR_OR_CALL_SUPER(DataSource);
}
