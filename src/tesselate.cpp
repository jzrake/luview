
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

  inp->initialize();
  out->initialize();
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
  __output_ds["triangles"]->set_input(this);
  __output_ds["segments"]->set_input(this);
}

void Tesselation3D::__refresh_cpu()
{
  // z: number indices from zero
  // v: generate voronoi
  // Q: quiet
  // ee: generate edges (NOTE: e -> subedges breaks)
  // nn: neighbor list (needed for adjtetlist)
  // f: write faces (also needed for nn to work)
  out->deinitialize();
  out->initialize();
  try {
    tetrahedralize("ffnnzee", inp, out);
  }
  catch (int) {
    luaL_error(__lua_state, "need to load some data before tesselating");
  }

  // ---------------------------------------------------------------------------
  // Refresh the output data sources
  // ---------------------------------------------------------------------------
  std::vector<GLfloat> verts;
  std::vector<GLuint> indtri;
  std::vector<GLuint> indseg;

  int leftout=0;

  for (int n=0; n<3*out->numberofpoints; ++n) {
    verts.push_back(out->pointlist[n]);
  }
  for (int n=0; n<out->numberoftrifaces; ++n) {
    //    if (out->adjtetlist[2*n] != -1 && out->adjtetlist[2*n+1] != -1) {
    //    if ((double)rand() / RAND_MAX < 0.01) {
    if (1) {
      indtri.push_back(out->trifacelist[3*n+0]);
      indtri.push_back(out->trifacelist[3*n+1]);
      indtri.push_back(out->trifacelist[3*n+2]);
    }
    else {
      ++leftout;
    }
  }
  for (int n=0; n<2*out->numberofedges; ++n) {
    indseg.push_back(out->edgelist[n]);
  }
  printf("leftout=%d\n", leftout);

  int N[2] = { verts.size()/3, 3 };
  __output_ds["triangles"]->set_data(&verts[0], N, 2);
  __output_ds["triangles"]->set_indices(&indtri[0], indtri.size());
  __output_ds["segments"]->set_data(&verts[0], N, 2);
  __output_ds["segments"]->set_indices(&indseg[0], indseg.size());
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
  //  self->inp->deinitialize();
  //  self->inp->initialize();
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
  //  self->inp->deinitialize();
  //  self->inp->initialize();
  self->inp->load_poly(fname); // doesn't accept const char*
  delete [] fname;
  self->__staged = true;
  return 0;
}
