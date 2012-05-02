#include <list>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream> 
#include <sstream>
using namespace std;

//lines or boxes should have opacity, or perhaps depth of how far out they are drawn


//Toggle Colorbar
//Smart guesses
//Movies
//Help menu
//Current parameters

//This defines these keywords as numbers, useful for data readin
enum{VOL,RHO,VXX,VYY,VZZ,PPP};

#define NUMVAR 10 //The number of variables we will load in.

char* DIR_BIN = "output999.bin";
int CommandMode = 0;
double PLOT_VALUE = 1;
double COLORBAR = 3;

//Drawing Variables for later
int NumberToDraw;
double DensityCut;
double DensityWidth;
int Delaunay;
double CUT;
double WIDTH;
int varC;
double POINTSIZE;
int loggy;
int VALEDIT=0;
static float xTranslate = .5;
static float yTranslate = 0.5;
static float zTranslate = 2.5;
static float RotationAngleX = 20;
static float RotationAngleY = 0;

void InitGraphics(int argc, char **argv);
void get_rgb( double , float * , float * , float * , int);

float * colordata;
float * pointdata;


double minval, maxval;

//struct Cell;


struct Cell{
   double x[3];
   std::list<struct Cell *> myFriends;
   double v[NUMVAR];
   bool Bc;
   double color[4];
};

struct Cell * theCells;
int Nc, Nf, Nq;
int * Cell0;
int * Cell1;


int main(int argc, char *argv[])
{
NumberToDraw = 0;
DensityCut = 1.0;
DensityWidth = 1.1;
Delaunay = 0;
CUT = .1;
WIDTH = .01;
varC = 1;
POINTSIZE = 1;
loggy = 1;

  if (argc < 2)
    {
      printf("For manual viewing please specifiy file and 0 for manual mode\n Example: './vbin TestOutput.bin 0");
      exit(1);
    }


if (argv[1]){
DIR_BIN = argv[1];
}

if (atoi(argv[2])){
 CommandMode++;
}
if (CommandMode){
NumberToDraw = atoi(argv[3]);
DensityCut = atof(argv[4]);
DensityWidth = atof(argv[5]);
Delaunay = atoi(argv[6]);
CUT = atof(argv[7]);
WIDTH = atof(argv[8]);
varC = atoi(argv[9]);
POINTSIZE = atof(argv[10]);
loggy = atoi(argv[11]);
xTranslate = atof(argv[12]);
yTranslate = atof(argv[13]);
zTranslate = atof(argv[14]);
RotationAngleX = atof(argv[15]);
RotationAngleY = atof(argv[16]);
COLORBAR = atoi(argv[17]);
PLOT_VALUE = atoi(argv[18]);
VALEDIT = atoi(argv[19]);
}
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
 
   
   for( j = 0; j < Nf; ++j ){
      theCells[Cell0[j]].myFriends.push_back( &(theCells[Cell1[j]]) );
      theCells[Cell1[j]].myFriends.push_back( &(theCells[Cell0[j]]) );
   }


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
static void KeyPressed(unsigned char key, int x, int y);
static void SpecialKeyPressed(int key, int x, int y);
static void glutPrint(float x, float y, float z, const char* text, float r, float g, float b, float a);
static void TakeScreenshot(const char *fname);


static int WindowID;
static int WindowWidth    = 800;
static int WindowHeight   = 600;
static int FullScreenMode = 0;



//Here are some variables we use to do visualization

void InitGraphics(int argc, char **argv)
{
 glutInit(&argc, argv);

//if(!CommandMode){
 glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
 glutInitWindowSize(WindowWidth, WindowHeight);
 glutInitWindowPosition(80, 80);
 WindowID = glutCreateWindow("BezView");
 glutDisplayFunc       (DrawGLScene);
 glutIdleFunc          (DrawGLScene);
 glutReshapeFunc       (ResizeGLScene);
 glutKeyboardFunc      (KeyPressed);
 glutSpecialFunc       (SpecialKeyPressed);

// glClearColor(0.2, 0.2, 0.1, 0.0);
 glClearColor(0, 0., 0, 0.0);
 glClearDepth(1.0);
 glDepthFunc(GL_LESS);
 glEnable(GL_DEPTH_TEST);
 glShadeModel(GL_SMOOTH);
 glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 if (FullScreenMode) {
   glutFullScreen();
 }
 gluPerspective(45.0f, (GLfloat)WindowWidth/(GLfloat)WindowHeight,
0.1f, 100.0f);
 glMatrixMode(GL_MODELVIEW);
//glEnable(GL_BLEND);
//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

if (NumberToDraw==0){
NumberToDraw = Nc;
}

Recolor();
glutMainLoop();

}






//std::vector<double>ind;
static void Recolor()
{
//edgecolor.clear();
maxval = -HUGE_VAL;
minval = HUGE_VAL;

float rrr,ggg,bbb;
int j = 0;
while( j<Nc ){
   double v0 = theCells[j].v[varC];
if (loggy){
   v0 = log(v0);
}
   if( maxval < v0 ) maxval = v0;
   if( minval > v0 ) minval = v0;
   ++j;
}
j=0;

while (j<Nc){

   double v0 = theCells[j].v[varC];
if (loggy){
   v0 = log(v0);
}
if (VALEDIT){
minval = DensityCut - DensityWidth;
maxval = DensityCut + DensityWidth;
}
   double val = (v0 - minval)/(maxval -minval);
   if( val > 1.0 ) val = 1.0;
   if( val < 0.0 ) val = 0.0;
   get_rgb( val , &rrr , &ggg , &bbb , COLORBAR);

   theCells[j].color[0] =rrr;
   theCells[j].color[1] =ggg;
   theCells[j].color[2] =bbb;
   theCells[j].color[3] = .9*val+.1;
   j++;
   }
glColorPointer(4, GL_DOUBLE, sizeof(struct Cell),&(theCells[0].color));
glVertexPointer(3, GL_DOUBLE, sizeof(struct Cell),&(theCells[0].x));

ChangeDraw();

}

std::vector<unsigned int>ind;
std::vector<unsigned int>dind;

static void ChangeDraw()
{

ind.clear();
dind.clear();

int i;

if(Delaunay){
for(i = 0; i < Nf; ++i ){
    	int i0 = Cell0[i];
    	int i1 = Cell1[i];
    if( (!(theCells[i0].Bc) || !(theCells[i1].Bc)) && 
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

int SHOWSTRING=0;
int SHOWHELP=0;


// The main drawing function. //
static void DrawGLScene()
{
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 glLoadIdentity();
 glTranslatef(-xTranslate, -yTranslate, -zTranslate);
 glRotatef(RotationAngleX, 1, 0, 0);
 glRotatef(RotationAngleY, 0, 1, 0);

if (SHOWSTRING){
stringstream ss;//create a stringstream
ss << NumberToDraw; ss << " ";
ss << DensityCut; ss << " ";
ss << DensityWidth; ss << " ";
ss << Delaunay; ss << " ";
ss << CUT; ss << " ";
ss << WIDTH; ss << " ";
ss << varC; ss << " ";
ss << POINTSIZE; ss << " ";
ss << loggy; ss << " ";
ss << xTranslate; ss << " ";
ss << yTranslate; ss << " ";
ss << zTranslate; ss << " ";
ss << RotationAngleX; ss << " ";
ss << RotationAngleY; ss << " ";
ss << COLORBAR; ss << " ";
ss << VALEDIT; ss << " ";
ss << PLOT_VALUE;
string str(ss.str());
const char* cstr1 = str.c_str();
glutPrint( 0 , 0 , 0  , cstr1 , 1, 1 , 1 , 0.5f );
}


if( SHOWHELP ){
     int NLines = 15;
     double dy = -.08;
     char help[NLines][256];
     sprintf(help[0],"Help Display:");
     sprintf(help[1],"WASD - Movment");
     sprintf(help[2],"Arrow Keys - Camera");
     sprintf(help[3],"r/f - Raise/Lower elevation");
     sprintf(help[4],"F - Toggle Fullscreen");
     sprintf(help[5],"l/m - Show less/more points");
     sprintf(help[6],"C/c - Spacial Cut Position");
     sprintf(help[7],"V/v - Spacial Cut Width");
     sprintf(help[8],"P/p - Value Cut Position");
     sprintf(help[9],"P/p - Value Cut Width");
     sprintf(help[10],"P/p - Value Cut Position");
     sprintf(help[11],"O/o - Value Cut Width");
     sprintf(help[12],"S - Toggle Params");
     sprintf(help[13],"D - Toggle Delaunay");
     sprintf(help[14],"Esc - Quit Program");
     for( int i=0 ; i<NLines ; ++i ){
      glutPrint(.8,1+i*dy,.001, help[i] , 1,1 , 1 , 0.5f );
     }
  }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

if(!Delaunay){
    glPointSize(POINTSIZE);
    glDrawElements(GL_POINTS, ind.size(), GL_UNSIGNED_INT, &ind[0] );
}
if(Delaunay){
    glDrawElements(GL_LINES, dind.size(), GL_UNSIGNED_INT, &dind[0] );
}
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);


    glutSwapBuffers();
if(CommandMode){
    TakeScreenshot("test.ppm");
    exit(1);
}

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
if(VALEDIT){Recolor();}
else{ChangeDraw();}
 }   
 else if (key == 'p') {
 DensityCut /=1.1;
if(VALEDIT){Recolor();}
else{ChangeDraw();}
 }   
 else if (key == 'O') { 
 DensityWidth *=1.1;
if(VALEDIT){Recolor();}
else{ChangeDraw();}
 }   
 else if (key == 'o') {
 DensityWidth /=1.1;
if(VALEDIT){Recolor();}
else{ChangeDraw();}
}




 else if (key == 'D') { 
 Delaunay=!Delaunay;
Recolor();
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
 COLORBAR++;
Recolor();
 }   
 else if (key == ']') {
 COLORBAR--;
Recolor();
}

 else if (key == '?') {
 POINTSIZE++;
 }   
 else if (key == '/') {
 POINTSIZE--;
}
 else if (key == 'L') {
 loggy = !loggy;
Recolor();
}
 else if (key == 'S') {
SHOWSTRING = !SHOWSTRING;
}
 else if (key == 'H') {
SHOWHELP = !SHOWHELP;
}
 else if (key == 'M') {
VALEDIT = !VALEDIT;
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


void glutPrint(float x, float y, float z , const char* text, float r, float g, float b, float a){
   if(!text || !strlen(text)) return;
   glColor4f(r,g,b,a);
   glRasterPos3f(x,y,z);
   while (*text) {
       glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
       text++;
   }
 }




///////////////////////////

void TakeScreenshot(const char *fname)
{
  int dimx = WindowWidth;
  int dimy = WindowHeight;

  size_t imsize = 3*dimx*dimy;
  char *pixels = (char*) malloc(imsize*sizeof(char));
  glReadPixels(0, 0, dimx, dimy, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  FILE *fp = fopen(fname, "wb");
  fprintf(fp, "P6\n%d %d\n255\n", dimx, dimy);
  fwrite(pixels, sizeof(char), imsize, fp);
  fclose(fp);

  free(pixels);
}
