
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


Tesselation3D::Tesselation3D()
{
  inp = new tetgenio;
  out = new tetgenio;
}

Tesselation3D::~Tesselation3D()
{
  delete inp;
  delete out;
}

void Tesselation3D::__init_lua_objects()
{
  hold(__output_ds["triangles"] = create<DataSource>(__lua_state));
  hold(__output_ds["segments"] = create<DataSource>(__lua_state));
}

void Tesselation3D::__refresh_cpu()
{
  //  inp->load_poly("/Users/jzrake/Work/luview/data/balls3astr_12_16");
  //  inp->load_poly("/Users/jzrake/Work/luview/data/CutSphere");
  //  inp->load_poly("/Users/jzrake/Work/luview/data/hinge");//, tetgenbehavior::MESH);
  //  inp->load_node("/Users/jzrake/Work/luview/data/brain");

  // z: number indices from zero
  // v: generate voronoi
  // Q: quiet
  // ee: generate edges (NOTE: e -> subedges breaks)
  out->initialize();
  try {
    tetrahedralize("zveeQ", inp, out);
  }
  catch (int) {
    luaL_error(__lua_state, "need to load some data before tesselating");
  }

  // ---------------------------------------------------------------------------
  // Get the triangle faces
  // ---------------------------------------------------------------------------
  {
    GLuint *indices = new GLuint[3 * out->numberofedges];
    GLfloat *verts = new GLfloat[3 * out->numberofpoints];

    for (int n=0; n<3*out->numberoftrifaces; ++n) {
      indices[n] = out->trifacelist[n];
    }
    for (int n=0; n<3*out->numberofpoints; ++n) {
      verts[n] = out->pointlist[n];
    }

    int N[2] = { out->numberofpoints, 3 };
    __output_ds["triangles"]->set_data(verts, N, 2);
    __output_ds["triangles"]->set_indices(indices, 3*out->numberoftrifaces);

    delete [] verts;
    delete [] indices;
  }

  // ---------------------------------------------------------------------------
  // Get the segments
  // ---------------------------------------------------------------------------
  {
    GLuint *indices = new GLuint[2 * out->numberofedges];
    GLfloat *verts = new GLfloat[3 * out->numberofpoints];

    for (int n=0; n<2*out->numberofedges; ++n) {
      indices[n] = out->edgelist[n];
    }
    for (int n=0; n<3*out->numberofpoints; ++n) {
      verts[n] = out->pointlist[n];
    }

    int N[2] = { out->numberofpoints, 3 };
    __output_ds["segments"]->set_data(verts, N, 2);
    __output_ds["segments"]->set_indices(indices, 2*out->numberofedges);

    delete [] verts;
    delete [] indices;
  }
}

Tesselation3D::LuaInstanceMethod Tesselation3D::__getattr__(std::string &method_name)
{
  AttributeMap attr;
  attr["load_poly"] = _load_poly_;
  attr["load_node"] = _load_node_;
  RETURN_ATTR_OR_CALL_SUPER(DataSource);
}
int Tesselation3D::_load_node_(lua_State *L)
{
  Tesselation3D *self = checkarg<Tesselation3D>(L, 1);
  const char *fname_ = luaL_checkstring(L, 2);
  char *fname = new char[strlen(fname_)];
  strcpy(fname, fname_);
  self->inp->load_node(fname); // doesn't accept const char*
  delete [] fname;
  self->__staged = true;
  return 0;
}
int Tesselation3D::_load_poly_(lua_State *L)
{
  Tesselation3D *self = checkarg<Tesselation3D>(L, 1);
  const char *fname_ = luaL_checkstring(L, 2);
  char *fname = new char[strlen(fname_)];
  strcpy(fname, fname_);
  self->inp->load_poly(fname); // doesn't accept const char*
  delete [] fname;
  self->__staged = true;
  return 0;
}
