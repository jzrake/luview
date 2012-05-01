
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


static void draw_cylinder(double *x0, double *x1, double rad0, double rad1)
{
  double r[3] = {x1[0] - x0[0], x1[1] - x0[1], x1[2] - x0[2]};
  double mag = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
  r[0] /= mag;
  r[1] /= mag;
  r[2] /= mag;

  double a[3], zhat[3] = { 0, 0, 1 };
  a[0] = zhat[1]*r[2] - zhat[2]*r[1];
  a[1] = zhat[2]*r[0] - zhat[0]*r[2];
  a[2] = zhat[0]*r[1] - zhat[1]*r[0];

  double angle = acos(r[2]) * 180.0 / M_PI;
  //  printf("rotating by %f around (%f %f %f)\n", angle, a[0], a[1], a[2]);

  glPushMatrix();
  glTranslated(x0[0], x0[1], x0[2]);
  glRotated(angle, a[0], a[1], a[2]);

  GLUquadric *quad = gluNewQuadric();
  gluCylinder(quad, rad0, rad1, mag, 10, 1);
  gluDeleteQuadric(quad);

  glPopMatrix();
}


Tesselation3D::Tesselation3D()
{
  gl_modes.push_back(GL_LIGHTING);
  gl_modes.push_back(GL_LIGHT0);
  gl_modes.push_back(GL_NORMALIZE);
  gl_modes.push_back(GL_BLEND);
  gl_modes.push_back(GL_COLOR_MATERIAL);

  Np = 8;
  pointlist = new double[Np * 3];

  int n;

  n = 0;
  pointlist[3*n + 0] = 0.0;
  pointlist[3*n + 1] = 1.0;
  pointlist[3*n + 2] = 0.0;

  n = 1;
  pointlist[3*n + 0] =-0.5;
  pointlist[3*n + 1] = 0.0;
  pointlist[3*n + 2] = 0.0;

  n = 2;
  pointlist[3*n + 0] =+0.5;
  pointlist[3*n + 1] = 0.0;
  pointlist[3*n + 2] = 0.0;

  n = 3;
  pointlist[3*n + 0] = 0.0;
  pointlist[3*n + 1] = 0.25;
  pointlist[3*n + 2] = 1.0;

  n = 4;
  pointlist[3*n + 0] = 0.0;
  pointlist[3*n + 1] = 0.25;
  pointlist[3*n + 2] =-1.0;

  n = 5;
  pointlist[3*n + 0] = 0.0;
  pointlist[3*n + 1] =-0.25;
  pointlist[3*n + 2] =-1.0;

  n = 6;
  pointlist[3*n + 0] =-0.5;
  pointlist[3*n + 1] = 0.25;
  pointlist[3*n + 2] =-1.0;

  n = 7;
  pointlist[3*n + 0] =+0.5;
  pointlist[3*n + 1] = 0.25;
  pointlist[3*n + 2] =-1.0;
}

Tesselation3D::~Tesselation3D()
{
  delete [] pointlist;
}

void Tesselation3D::draw_local()
{
  int delaunay = 0;
  int voronoi = 1;
  tetgenio inp, out;

  inp.initialize();
  out.initialize();

  inp.numberofpoints = Np;
  inp.pointlist = new double[inp.numberofpoints * 3];
  memcpy(inp.pointlist, pointlist, Np*3*sizeof(double));

  // z: number indices from zero
  // v: generate voronoi
  // Q: quiet
  // ee: generate edges (NOTE: e -> subedges breaks)
  tetrahedralize("zveeQ", &inp, &out);

  if (delaunay) {
    /*
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
    */

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

  if (voronoi) {

    for (int n=0; n<out.numberofvfacets; ++n) {
      tetgenio::vorofacet &f = out.vfacetlist[n];
      std::vector<REAL*> verts;

      for (int m=0; m<f.elist[0]; ++m) {
        int eindex = f.elist[m+1];
        int vindex = out.vedgelist[eindex].v1;
        if (out.vedgelist[eindex].v2 == -1) {
	  //          printf("skipping ray\n");
	  continue;
        }
        verts.push_back(&out.vpointlist[3*vindex]);
      }
      if (verts.size() < 3) {
	//	printf("skipping facet with %ld vertices\n", verts.size());
	continue;
      }

      REAL normal[3];
      compute_normal(verts[0], verts[1], verts[2], normal);
      glNormal3dv(normal);

      glBegin(GL_POLYGON);
      for (unsigned int m=0; m<verts.size(); ++m) {
        glVertex3dv(verts[m]);
      }
      glEnd();
    }


    glBegin(GL_LINES);
    for (int n=0; n<out.numberofvedges; ++n) {
      tetgenio::voroedge &e = out.vedgelist[n];
      if (e.v2 == -1) {
	REAL *u = &out.vpointlist[3*e.v1];
	REAL v[3] = {u[0] + e.vnormal[0], u[1] + e.vnormal[1], u[2] + e.vnormal[2]};

	/*
	REAL normal[3] = {u[0]-v[0], u[1]-v[1], u[2]-v[2]};
	glNormal3dv(normal);
	glVertex3dv(u);
	glVertex3dv(v);
	*/

	draw_cylinder(u, v, 0.02, 0.02);
      }
      else {
	REAL *u = &out.vpointlist[3*e.v1];
	REAL *v = &out.vpointlist[3*e.v2];
	/*
	REAL normal[3] = {u[0]-v[0], u[1]-v[1], u[2]-v[2]};
	glNormal3dv(normal);
	glVertex3dv(u);
	glVertex3dv(v);
	*/
	draw_cylinder(u, v, 0.02, 0.02);
      }
    }
    glEnd();

  }
}

void Tesselation3D::compute_normal(double *u, double *v, double *w, double *n)
{
  double uv[3] = {v[0] - u[0], v[1] - u[1], v[2] - u[2]};
  double uw[3] = {w[0] - u[0], w[1] - u[1], w[2] - u[2]};

  n[0] = uv[1]*uw[2] - uv[2]*uw[1];
  n[1] = uv[2]*uw[0] - uv[0]*uw[2];
  n[2] = uv[0]*uw[1] - uv[1]*uw[0];

  double mag = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
  n[0] /= mag;
  n[1] /= mag;
  n[2] /= mag;
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


/*
  n=0;
  pointlist[3*n + 0] =-0.5;
  pointlist[3*n + 1] =-0.5;
  pointlist[3*n + 2] =-0.5;

  n=1;
  pointlist[3*n + 0] =-0.5;
  pointlist[3*n + 1] =-0.5;
  pointlist[3*n + 2] =+0.5;

  n=2;
  pointlist[3*n + 0] =-0.5;
  pointlist[3*n + 1] =+0.5;
  pointlist[3*n + 2] =-0.5;

  n=3;
  pointlist[3*n + 0] =-0.5;
  pointlist[3*n + 1] =+0.5;
  pointlist[3*n + 2] =+0.5;

  n=4;
  pointlist[3*n + 0] =+0.5;
  pointlist[3*n + 1] =-0.5;
  pointlist[3*n + 2] =-0.5;

  n=5;
  pointlist[3*n + 0] =+0.5;
  pointlist[3*n + 1] =-0.5;
  pointlist[3*n + 2] =+0.5;

  n=6;
  pointlist[3*n + 0] =+0.5;
  pointlist[3*n + 1] =+0.5;
  pointlist[3*n + 2] =-0.5;

  n=7;
  pointlist[3*n + 0] =+0.5;
  pointlist[3*n + 1] =+0.5;
  pointlist[3*n + 2] =+0.5;
*/

  /*
  double phi = (1.0 + sqrt(5.0)) / 2.0;
  n=0;
  pointlist[3*n + 0] =-1.0;
  pointlist[3*n + 1] =-1.0;
  pointlist[3*n + 2] =-1.0;

  n=1;
  pointlist[3*n + 0] =-1.0;
  pointlist[3*n + 1] =-1.0;
  pointlist[3*n + 2] =+1.0;

  n=2;
  pointlist[3*n + 0] =-1.0;
  pointlist[3*n + 1] =+1.0;
  pointlist[3*n + 2] =-1.0;

  n=3;
  pointlist[3*n + 0] =-1.0;
  pointlist[3*n + 1] =+1.0;
  pointlist[3*n + 2] =+1.0;

  n=4;
  pointlist[3*n + 0] =+1.0;
  pointlist[3*n + 1] =-1.0;
  pointlist[3*n + 2] =-1.0;

  n=5;
  pointlist[3*n + 0] =+1.0;
  pointlist[3*n + 1] =-1.0;
  pointlist[3*n + 2] =+1.0;

  n=6;
  pointlist[3*n + 0] =+1.0;
  pointlist[3*n + 1] =+1.0;
  pointlist[3*n + 2] =-1.0;

  n=7;
  pointlist[3*n + 0] =+1.0;
  pointlist[3*n + 1] =+1.0;
  pointlist[3*n + 2] =+1.0;

  n=8;
  pointlist[3*n + 0] = 0.0;
  pointlist[3*n + 1] =-1.0/phi;
  pointlist[3*n + 2] =-phi;

  n=9;
  pointlist[3*n + 0] = 0.0;
  pointlist[3*n + 1] =-1.0/phi;
  pointlist[3*n + 2] =+phi;

  n=10;
  pointlist[3*n + 0] = 0.0;
  pointlist[3*n + 1] =+1.0/phi;
  pointlist[3*n + 2] =-phi;

  n=11;
  pointlist[3*n + 0] = 0.0;
  pointlist[3*n + 1] =+1.0/phi;
  pointlist[3*n + 2] =+phi;

  n=12;
  pointlist[3*n + 2] = 0.0;
  pointlist[3*n + 0] =-1.0/phi;
  pointlist[3*n + 1] =-phi;

  n=13;
  pointlist[3*n + 2] = 0.0;
  pointlist[3*n + 0] =-1.0/phi;
  pointlist[3*n + 1] =+phi;

  n=14;
  pointlist[3*n + 2] = 0.0;
  pointlist[3*n + 0] =+1.0/phi;
  pointlist[3*n + 1] =-phi;

  n=15;
  pointlist[3*n + 2] = 0.0;
  pointlist[3*n + 0] =+1.0/phi;
  pointlist[3*n + 1] =+phi;

  n=16;
  pointlist[3*n + 1] = 0.0;
  pointlist[3*n + 2] =-1.0/phi;
  pointlist[3*n + 0] =-phi;

  n=17;
  pointlist[3*n + 1] = 0.0;
  pointlist[3*n + 2] =-1.0/phi;
  pointlist[3*n + 0] =+phi;

  n=18;
  pointlist[3*n + 1] = 0.0;
  pointlist[3*n + 2] =+1.0/phi;
  pointlist[3*n + 0] =-phi;

  n=19;
  pointlist[3*n + 1] = 0.0;
  pointlist[3*n + 2] =+1.0/phi;
  pointlist[3*n + 0] =+phi;
  */
