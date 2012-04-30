#include <list>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//lines or boxes should have opacity, or perhaps depth of how far out they are drawn

//The code should take arguments for what data file it will be reading from

//Delaunay should have cuts, and both delaunay and point showing need to be made FAST

//This defines these keywords as numbers, useful for data readin
enum{VOL,RHO,VXX,VYY,VZZ,PPP};

#define DIR_BIN "outputNewestest.bin"
#define NUMVAR 10 //The number of variables we will load in.

double PLOT_VALUE = 1;
double COLORBAR = 3;

void InitGraphics(int argc, char **argv);
void get_rgb( double , float * , float * , float * , int );

float * colordata;
float * pointdata;


double minval, maxval;

//struct Cell;


struct Cell{
   double x[3];
   std::list<struct Cell *> myFriends;
   double v[NUMVAR];
   bool Bc;
   double color[3];
};

struct Cell * theCells;
int Nc, Nf, Nq;
int * Cell0;
int * Cell1;


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

   int j = 0;
   while( j<Nc ){
      double Q[Nq];
      int bc;

      fread(  Q , sizeof(double) , Nq , pFile);
      fread( &bc, sizeof(int)    , 1  , pFile);

      //Load in the spacial data
      theCells[j].x[0]   = Q[0];
      theCells[j].x[1]   = Q[1];
      theCells[j].x[2]   = Q[2];
      //Load in the volume
      theCells[j].v[0] = Q[Nq-1]; //Vol
      //Load in the data in the data file
      for(int i=1 ; i<Nq-2 ; ++i ){
         theCells[j].v[i]  = Q[i+2]; 
      }


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
static void Recolor();
static void ChangeDraw();
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
int varC;

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
// glutTimerFunc(33, timerCB, 33);  


 if (FullScreenMode) {
   glutFullScreen();
 }
NumberToDraw = Nc;
DensityCut = 1;
DensityWidth = 1.1;
Delaunay = 0;
CUT = .1;
WIDTH = .01;
varC = 1;

Recolor();
glutMainLoop();



}


std::vector<double>Lecol;
std::vector<double>Lepos;

static void Recolor()
{
   maxval = -HUGE_VAL;
   minval = HUGE_VAL;

        float rrr,ggg,bbb;
int j = 0;
while (j<Nc){
      if( maxval < theCells[j].v[varC] ) maxval = theCells[j].v[varC];
      if( minval > theCells[j].v[varC] ) minval = theCells[j].v[varC];


      double val = (theCells[j].v[varC] - minval)/(maxval -minval);
      if( val > 1.0 ) val = 1.0;
      if( val < 0.0 ) val = 0.0;

get_rgb( val , &rrr , &ggg , &bbb , COLORBAR );
theCells[j].color[0] =rrr;
theCells[j].color[1] =ggg;
theCells[j].color[2] =bbb;
j++;


    glColorPointer(3, GL_DOUBLE, sizeof(struct Cell),&(theCells[0].color));
    glVertexPointer(3, GL_DOUBLE, sizeof(struct Cell),&(theCells[0].x));
}

ChangeDraw();
}

std::vector<unsigned int>ind;
std::vector<unsigned int>dind;
static void ChangeDraw()
{

ind.clear();
dind.clear();

int i;
 //   	double v0 = .5*(theCells[i0].v[varC]-DensityCut)/DensityWidth+.5;
//    	double v1 = .5*(theCells[i1].v[varC]-DensityCut)/DensityWidth+.5;
//    	double vv = .5*(v0+v1);

if(Delaunay){
for(i = 0; i < Nf; ++i ){
    	int i0 = Cell0[i];
    	int i1 = Cell1[i];
    if( !(theCells[i0].Bc)  && !(theCells[i1].Bc) &&
      ( fabs(theCells[i0].v[varC]-DensityCut) < DensityWidth) &&
      ( fabs(theCells[i1].v[varC]-DensityCut) < DensityWidth) &&
      ( fabs(theCells[i0].x[2]-CUT) < WIDTH || 
	fabs(theCells[i0].x[1]-CUT) < WIDTH || 
	fabs(theCells[i0].x[0]-CUT) < WIDTH ) &&
      ( fabs(theCells[i1].x[2]-CUT) < WIDTH || 
	fabs(theCells[i1].x[1]-CUT) < WIDTH || 
	fabs(theCells[i1].x[0]-CUT) < WIDTH ) )
        {
	dind.push_back(Cell0[i]);
 	dind.push_back(Cell1[i]);
	}
}
}


if(!Delaunay){
for(i = 0; i < NumberToDraw; ++i ){
    if( !(theCells[i].Bc)  &&
      ( fabs(theCells[i].v[varC]-DensityCut) < DensityWidth) &&
      ( fabs(theCells[i].x[2]-CUT) < WIDTH || 
	fabs(theCells[i].x[1]-CUT) < WIDTH || 
	fabs(theCells[i].x[0]-CUT) < WIDTH ) )
	{ ind.push_back(i);}
}
}

}
//End of ChangeDraw Function




// The main drawing function. //
static void DrawGLScene()
{
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 glLoadIdentity();
 glTranslatef(-xTranslate, -yTranslate, -zTranslate);
 glRotatef(RotationAngleX, 1, 0, 0);
 glRotatef(RotationAngleY, 0, 1, 0);



//    glPointSize(3);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

if(!Delaunay){
    glDrawElements(GL_POINTS, ind.size(), GL_UNSIGNED_INT, &ind[0] );
}
if(Delaunay){
    glDrawElements(GL_LINES, dind.size(), GL_UNSIGNED_INT, &dind[0] );
}
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

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
 if (NumberToDraw * 2 <= Nc){NumberToDraw*=2 ;
ChangeDraw();}
 }   
 else if (key == 'l') {
 NumberToDraw = NumberToDraw / 2;
ChangeDraw();
 }   


//Density Cuts
 else if (key == 'P') { 
 DensityCut *=1.1;
ChangeDraw();
 }   
 else if (key == 'p') {
 DensityCut /=1.1;
ChangeDraw();
 }   
 else if (key == 'O') { 
 DensityWidth *=1.1;
ChangeDraw();
 }   
 else if (key == 'o') {
 DensityWidth /=1.1;
ChangeDraw();
}




 else if (key == 'D') { 
 if(Delaunay){Delaunay=0;}
 else{Delaunay=1;}
ChangeDraw();
 }   


//Spacial Cuts
 else if (key == 'C') {
 CUT += .01;
ChangeDraw();
 }   
 else if (key == 'c') {
 CUT -= .01;
ChangeDraw();
 }   
 else if (key == 'V') {
 WIDTH += .01;
ChangeDraw();
 }   
 else if (key == 'v') {
 WIDTH -= .01;
ChangeDraw();
 }   

//ColorBar

 else if (key == '[') {
 COLORBAR += 1;
Recolor();
 }   
 else if (key == ']') {
 COLORBAR -= 1;
Recolor();
}



//number keys for variables
 else if (key == '0'){varC = 0; Recolor();}   
 else if (key == '1'){varC = 1; Recolor();}   
 else if (key == '2'){varC = 2; Recolor();}   
 else if (key == '3'){varC = 3; Recolor();}   
 else if (key == '4'){varC = 4; Recolor();}   
 else if (key == '5'){varC = 5; Recolor();}   
 else if (key == '6'){varC = 6; Recolor();}   
 else if (key == '7'){varC = 7; Recolor();}   
 else if (key == '8'){varC = 8; Recolor();}   
 else if (key == '9'){varC = 9; Recolor();}   

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
