#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//You should make a "read data file" function
//This function will then be called when you press a number 
//to get a new type of data 


//Fix the way the camera works


//be able to toggle Delaunay on and off

//lines or boxes should have opacity, or perhaps depth of how far out they are drawn

//The code should take arguments for what data file it will be reading from


//Make isosurfaces for densities


#define PLOT_VALUE Q[3] 
#define COLORBAR 3
#define DIR_BIN "outputBIG.bin"

void InitGraphics(int argc, char **argv);
void get_rgb( double , float * , float * , float * , int );

float * colordata;
float * pointdata;


double minval, maxval;

struct Cell;


struct Cell{

   int Id;
   double x[3];
   std::list<struct Cell *> myFriends;
   double theta;
   double value;
   bool Bc;

};

struct Cell * theCells;
int Nc, Nf, Nq;
int * Cell0;
int * Cell1;


bool compare_cell( Cell * A, Cell * B ){
   return( A->theta < B->theta );
}


int countlines(char * filename){
   FILE *pFile = fopen(filename, "r");
   int lines=0;
   char c;
   while ((c = fgetc(pFile)) != EOF){
      if (c == '\n') ++lines;
   }
   fclose(pFile);
   return(lines);
}





int main(int argc, char **argv) 
{

   FILE *pFile;
   
   pFile = fopen(DIR_BIN,"rb");

//first three numbers in the file
   fread( &Nq , sizeof(int) , 1 , pFile ); //NUmber of doubles per cell
   fread( &Nc , sizeof(int) , 1 , pFile ); //Number of cells
   fread( &Nf , sizeof(int) , 1 , pFile ); //number of faces

   theCells = new struct Cell[Nc];
   Cell0 = (int *) malloc( Nf*sizeof(int) );//new int[Nf];
   Cell1 = (int *) malloc( Nf*sizeof(int) );//new int[Nf];
  
   printf("Stage 1 Complete, Nq = %d Nc = %d Nf = %d\n",Nq,Nc,Nf);
      maxval = -HUGE_VAL;
      minval = HUGE_VAL;

   int j = 0;
   while( j<Nc ){
      double Q[Nq];
      int bc;
      double x,y,z,rho, Pp, vx, vy, vz,Vol;

      fread(  Q , sizeof(double) , Nq , pFile);
      fread( &bc, sizeof(int)    , 1  , pFile);
//No matter how many vaiables we have, we take 7. More will be ignored, less will segfault 
//The first three gets x,y,z data
      theCells[j].x[0]   = Q[0];
      theCells[j].x[1]   = Q[1];
      theCells[j].x[2]   = Q[2];

      rho = Q[3];
      vx  = Q[4];
      vy  = Q[5];
      vz  = Q[6];
      Pp  = Q[7];

      Vol = Q[Nq-1];

//connected to a DEFINE statement earlier on which you use to choose what your going to plot
      theCells[j].value = PLOT_VALUE;


      if( maxval < theCells[j].value ) maxval = theCells[j].value;
      if( minval > theCells[j].value ) minval = theCells[j].value;


      theCells[j].Bc = false;
      if( bc == 1 ) theCells[j].Bc = true;

      ++j;
   }
//Done with cells, the rest of the data sits on the faces. Pairs of integers saying which cells are neighbors.
   for( j=0 ; j<Nf ; ++j ){
      fread( &(Cell0[j]) , sizeof(int) , 1 , pFile );
      fread( &(Cell1[j]) , sizeof(int) , 1 , pFile );
   }
 
   printf("Stage 2 Complete Nc = %d\n",Nc);
   
   for( j = 0; j < Nf; ++j ){
      theCells[Cell0[j]].myFriends.push_back( &(theCells[Cell1[j]]) );
      theCells[Cell1[j]].myFriends.push_back( &(theCells[Cell0[j]]) );
   }

   printf("Stage 3 Complete\n");

InitGraphics(argc, argv);

   free(Cell0);
   free(Cell1);

return 1;

}

//////////////////////////////





































//////////BEGIN THE GLUT
//-----------------------------------------------------------

/* The number of our GLUT window */
int window; 


#include <GL/gl.h>	
#include <GL/glu.h>
#include <GL/glut.h>

static void DrawGLScene();
static void ResizeGLScene(int Width, int Height);
static void MousePressed(int button, int state, int x, int y);
static void KeyPressed(unsigned char key, int x, int y);
static void SpecialKeyPressed(int key, int x, int y);

static int WindowID;
static int WindowWidth    = 768;
static int WindowHeight   = 512;
static int FullScreenMode = 0;
static int mouseButtons   = 0;
static int mouseOldX;
static int mouseOldY;

static float xTranslate = .5;
static float yTranslate = 0.5;
static float zTranslate = 2.5;
static float RotationAngleX = 20;
static float RotationAngleY = 0;


//Here are some variables we use to do visualization
int NumberToDraw;
double DensityCut;
double DensityWidth;
int Delaunay;
double CUT;
double WIDTH;

void InitGraphics(int argc, char **argv)
{
 glutInit(&argc, argv);

 glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
 glutInitWindowSize(WindowWidth, WindowHeight);
 glutInitWindowPosition(80, 80);
 WindowID = glutCreateWindow("BezView");

 glutDisplayFunc       (DrawGLScene);
 glutIdleFunc          (DrawGLScene);
 glutReshapeFunc       (ResizeGLScene);
 glutKeyboardFunc      (KeyPressed);
 glutSpecialFunc       (SpecialKeyPressed);
 glutMouseFunc         (MousePressed);


 glClearColor(0.2, 0.2, 0.1, 0.0);
 glClearDepth(1.0);
 glDepthFunc(GL_LESS);
 glEnable(GL_DEPTH_TEST);
 glShadeModel(GL_SMOOTH);
 glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 gluPerspective(45.0f, (GLfloat)WindowWidth/(GLfloat)WindowHeight,
0.1f, 100.0f);
 glMatrixMode(GL_MODELVIEW);
 glEnable(GL_BLEND);


 if (FullScreenMode) {
   glutFullScreen();
 }
NumberToDraw = Nc;
DensityCut = 1;
DensityWidth = 1.1;
Delaunay = 0;
CUT = .1;
WIDTH = .01;


 glutMainLoop();
}

/*

// The main drawing function. //
static void DrawBetterGLScene()
{
//load all of points into pointdata and colors into colordata

std::vector <GLuint> ind;   
for( n = 0; n < NumberToDraw; ++n ){
      if( !(theCells[n].Bc)  &&
          ( fabs(theCells[n].value-DensityCut) < DensityWidth) &&
          (fabs(theCells[n].x[2]-CUT) < WIDTH || fabs(theCells[n].x[1]-CUT) < WIDTH || fabs(theCells[n].x[0]-CUT) < WIDTH ) )
{ ind.push_back(n);}}

    glPointSize(5.0);

    glEnableClientState(GL_VERTEX_ARRAY);
    if (colordata) glEnableClientState(GL_COLOR_ARRAY);

    if (colordata) {
      glColorPointer(4, GL_FLOAT, 4*sizeof(GLfloat), colordata);
    }
    glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), pointdata);
    glDrawElements(GL_POINTS, ind.size(), GL_UNSIGNED_INT, ind);

    glDisableClientState(GL_VERTEX_ARRAY);
    if (colordata) glDisableClientState(GL_COLOR_ARRAY);

    free(ind);





}*/


static void DrawGLScene()
{

 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 glLoadIdentity();
 glTranslatef(-xTranslate, -yTranslate, -zTranslate);

 glRotatef(RotationAngleX, 1, 0, 0);
 glRotatef(RotationAngleY, 0, 1, 0);


/////////////////////////

 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   int k;
//Normally < Nc
   for( k = 0; k < NumberToDraw; ++k ){
      if( !(theCells[k].Bc)  && 
          ( fabs(theCells[k].value-DensityCut) < DensityWidth) &&
          (fabs(theCells[k].x[2]-CUT) < WIDTH || fabs(theCells[k].x[1]-CUT) < WIDTH || fabs(theCells[k].x[0]-CUT) < WIDTH ) ){

      double val = (theCells[k].value - minval)/(maxval -minval);
      if( val > 1.0 ) val = 1.0;
      if( val < 0.0 ) val = 0.0;

      float rrr,ggg,bbb;
      get_rgb( val , &rrr , &ggg , &bbb , COLORBAR );
      glColor3f( rrr , ggg , bbb );

      double particleRadius = .005;

      glPushMatrix();
      glTranslatef (theCells[k].x[0], theCells[k].x[1], theCells[k].x[2]);
      glutSolidCube(particleRadius);
     // glutSolidSphere(particleRadius, 3, 3); //last two #'s are resolution
      glPopMatrix();
     }  
}

/////if 3d Box

 //glColor3d(0.6, 0.3, 0.3);
// glLineWidth(3.0);
// glutWireCube(2.0);

/////if Delaunay //Nf
      for(int f=0 ; f<Nf; ++f ){
	
	int i0 = Cell0[f];
	int i1 = Cell1[f];
	double x0 =theCells[i0].x[0];
	double y0 =theCells[i0].x[1];
	double z0 =theCells[i0].x[2];
	double x1 =theCells[i1].x[0];
	double y1 =theCells[i1].x[1];
	double z1 =theCells[i1].x[2];

if(Delaunay){
        if( theCells[i0].Bc == 0 && theCells[i1].Bc == 0 ){	
       //  double zc = theCells[k].x[2];
      //   std::list<struct Cell *>::iterator cp;
      //   cp = theCells[k].myFriends.begin();
    //     while( cp != theCells[k].myFriends.end() ){
            glColor3f( 0.0 , 0.0 , 1 );//1.0 , 0.0 );
            glLineWidth( .001 );//2.0f );
            glBegin(GL_LINES);
            glVertex3f( x0 , y0 , z0 );
            glVertex3f( x1 , y1 , z1  );
            glEnd();
    //        ++cp;
        }
      }
      }




 glFlush();
 glutSwapBuffers();

}



void ResizeGLScene(int Width, int Height)
{
 if (Height==0) Height=1;
 glViewport(0, 0, Width, Height);

 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();

 gluPerspective(45.0f, (GLfloat)Width/(GLfloat)Height, 0.1f, 100.0f);
 glMatrixMode(GL_MODELVIEW);
}
void SpecialKeyPressed(int key, int x, int y)
{
 if      (key == GLUT_KEY_RIGHT) RotationAngleY -= 3;
 else if (key == GLUT_KEY_LEFT ) RotationAngleY += 3;

 if      (key == GLUT_KEY_DOWN ) RotationAngleX -= 3;
 else if (key == GLUT_KEY_UP   ) RotationAngleX += 3;

 glutPostRedisplay();
}
void MousePressed(int button, int state, int x, int y)
{
 if      (state == GLUT_DOWN) mouseButtons |= 1<<button;
 else if (state == GLUT_UP  ) mouseButtons  = 0;

 mouseOldX = x;
 mouseOldY = y;
 glutPostRedisplay();
}


void KeyPressed(unsigned char key, int x, int y)
{
 static const int ESCAPE = 27;

 if (key == ESCAPE || key == 'q') {
   glutDestroyWindow(WindowID);
   exit(0);
 }
 else if (key == 'd') {
 xTranslate += .1;
 }
 else if (key == 's') {
 zTranslate += .1;
 }
 else if (key == 'a') {
 xTranslate -= .1;
 }
 else if (key == 'w') {
 zTranslate -= .1;
 }
 else if (key == 'r') {
 yTranslate += .1;
 }
 else if (key == 'f') {
 yTranslate -= .1;
 }
 else if (key == 'm') {
 if (NumberToDraw * 2 <= Nc){NumberToDraw*=2 ;}
 }   
 else if (key == 'l') {
 NumberToDraw = NumberToDraw / 2;
 }   


//Density Cuts
 else if (key == 'P') { 
 DensityCut *=1.1;
 }   
 else if (key == 'p') {
 DensityCut /=1.1;
 }   
 else if (key == 'O') { 
 DensityWidth *=1.1;
 }   
 else if (key == 'o') {
 DensityWidth /=1.1;
}




 else if (key == 'D') { 
 if(Delaunay){Delaunay=0;}
 else{Delaunay=1;}
 }   


//Spacial Cuts
 else if (key == 'C') {
 CUT += .01;
 }   
 else if (key == 'c') {
 CUT -= .01;
 }   
 else if (key == 'V') {
 WIDTH += .01;
 }   
 else if (key == 'v') {
 WIDTH -= .01;
 }   


 else if (key == 'F') {
   if (FullScreenMode) {
     glutReshapeWindow(WindowWidth, WindowHeight);
     glutPositionWindow(0,0);
     FullScreenMode = 0;
   }
   else {
     glutFullScreen();
     FullScreenMode = 1;
   }
 }
 glutPostRedisplay();
}
