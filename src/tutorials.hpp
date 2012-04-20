







class SurfaceEvaluator : public SurfaceRendering
{
  void draw_local()
  {
    const int sx = Ny;
    const int sy = 1;

    const double dx = (Lx1 - Lx0) / (Nx - 1);
    const double dy = (Ly1 - Ly0) / (Ny - 1);

    for (int i=1; i<Nx-1; ++i) {
      for (int j=1; j<Ny-1; ++j) {

        const double x = Lx0 + i*dx;
        const double y = Ly0 + j*dy;
        const GLfloat *s = &surfdata[3*((i-1)*sx + (j-1)*sy)];

        glMap2f(GL_MAP2_VERTEX_3, x-dx, x+dx, 3*sx, 3, y-dy, y+dy, 3*sy, 3, s);
        glMapGrid2f(1, x-dx/2, x+dx/2, 1, y-dy/2, y+dy/2);
        glEvalMesh2(GL_LINE, 0, 1, 0, 1);
      }
    }
  }
} ;


class ExampleSimpleEvaluator : public DrawableObject
{
public:
  ExampleSimpleEvaluator()
  {
    gl_modes.push_back(GL_MAP2_VERTEX_3);
    gl_modes.push_back(GL_AUTO_NORMAL);
    gl_modes.push_back(GL_NORMALIZE);
  }

  void draw_local()
  {
    const int Nx = 8;
    const int Ny = 8;

    double Lx0=-0.5, Lx1=0.5, Ly0=-0.5, Ly1=0.5;

    const int sx = Ny;
    const int sy = 1;

    const double dx = (Lx1 - Lx0) / (Nx - 1);
    const double dy = (Ly1 - Ly0) / (Ny - 1);

    GLfloat *surfdata = (GLfloat*) malloc(Nx*Ny*3*sizeof(GLfloat));

    for (int i=0; i<Nx; ++i) {
      for (int j=0; j<Ny; ++j) {

        const double x = Lx0 + i*dx;
        const double y = Ly0 + j*dy;
        const double z = x*x + y*y;
        const int m = i*sx + j*sy;

        surfdata[3*m + 0] = x;
        surfdata[3*m + 1] = z;
        surfdata[3*m + 2] = y;
      }
    }

    for (int i=1; i<Nx-1; ++i) {
      for (int j=1; j<Ny-1; ++j) {

        const double x = Lx0 + i*dx;
        const double y = Ly0 + j*dy;
        const GLfloat *s = &surfdata[3*((i-1)*sx + (j-1)*sy)];

        glMap2f(GL_MAP2_VERTEX_3, x-dx, x+dx, 3*sx, 3, y-dy, y+dy, 3*sy, 3, s);
        glMapGrid2f(4, x-dx/2, x+dx/2, 4, y-dy/2, y+dy/2);
        glEvalMesh2(GL_LINE, 0, 4, 0, 4);
      }
    }
    free(surfdata);
  }
} ;



class ExampleSimpleVBO : public DrawableObject
{
public:
  void draw_local()
  {
    GLuint vbo,ibo;
    GLfloat verts[8][3] = {{0.0, 0.0, 0.0},
                           {0.0, 0.0, 0.1},
                           {0.0, 0.1, 0.0},
                           {0.0, 0.1, 0.1},
                           {0.1, 0.0, 0.0},
                           {0.1, 0.0, 0.1},
                           {0.1, 0.1, 0.0},
                           {0.1, 0.1, 0.1}};
    GLubyte ind[8] = {0,3,6,9,12,15,18,21};

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), verts, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 8*sizeof(GLubyte), ind,
                 GL_STATIC_DRAW);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(GLfloat), 0);
    glDrawElements(GL_LINE_STRIP, 8, GL_UNSIGNED_BYTE, 0);
    glDisableClientState(GL_VERTEX_ARRAY);

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
  }
} ;




class ExampleSimpleNURBS : public DrawableObject
{
private:
  GLfloat ctlpoints[6][6][3];
  int showPoints;
  GLUnurbsObj *theNurb;

public:
  ExampleSimpleNURBS() : showPoints(1)
  {
    gl_modes.push_back(GL_LIGHTING);
    gl_modes.push_back(GL_LIGHT0);
    gl_modes.push_back(GL_AUTO_NORMAL);
    gl_modes.push_back(GL_NORMALIZE);

    Orientation[0] = 90.0;

    init_surface();
  }
  ~ExampleSimpleNURBS() { }

private:
  void draw_local()
  {
    int i,j;
    GLfloat knots[8] = {0,1,2,3,4,5,6,7};

    GLfloat mat_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 100.0 };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    theNurb = gluNewNurbsRenderer();
    gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
    gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);
    gluNurbsCallback(theNurb, GLU_ERROR, (GLvoid (*)()) nurbsError);

    gluBeginSurface(theNurb);
    gluNurbsSurface(theNurb,
                    8, knots, 8, knots,
                    3*6, 3, &ctlpoints[0][0][0],
                    2, 2, GL_MAP2_VERTEX_3);
    gluEndSurface(theNurb);

    if (showPoints) {
      glPointSize(5.0);
      glDisable(GL_LIGHTING);
      glColor3f(1.0, 1.0, 0.0);
      glBegin(GL_POINTS);

      for (i=0; i<6; i++) {
        for (j=0; j<6; j++) {
          glVertex3f(ctlpoints[i][j][0],
		     ctlpoints[i][j][1],
		     ctlpoints[i][j][2]);
        }
      }
      glEnd();
      glEnable(GL_LIGHTING);
    }

    gluDeleteNurbsRenderer(theNurb);
  }

  void init_surface()
  {
    int u, v;
    for (u=0; u<6; u++) {
      for (v=0; v<6; v++) {

        ctlpoints[u][v][0] = u - 2.5;
        ctlpoints[u][v][1] = v - 2.5;

        if ((u == 2 || u == 3) && (v == 2 || v == 3)) {
          ctlpoints[u][v][2] = -3.0;
	}
        else {
          ctlpoints[u][v][2] = 3.0;
	}
      }
    }
  }

  static void nurbsError(GLenum errorCode)
  {
    fprintf(stderr, "Nurbs Error: %s\n", gluErrorString(errorCode));
    exit(0);
  }
} ;
