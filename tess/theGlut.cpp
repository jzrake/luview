#include <list>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sstream>
#include <glui.h>//AwesomeSauce.
#include "theGlut.h"
#include "Cell.h"
#include "keys.cpp"

//External Viewing Parameters from MAIN 
//These should be passed in a better way :(
extern int CommandMode;
extern int COLORBAR;
extern int NumberToDraw;
extern float DensityCut;
extern float DensityWidth;
extern int Delaunay;
extern float CUT;
extern float WIDTH;
extern int varC;
extern int POINTSIZE;
extern int loggy;
extern int VALEDIT;
extern float obj_pos[];
extern struct Cell * theCells;
extern float AngleX;
extern float AngleY;
extern float AngleZ;
extern float * colordata;
extern float * pointdata;
extern int Nc, Nf, Nq;
extern int * Cell0;
extern int * Cell1;
extern double minval, maxval;

//Internal Viewing Parameters
int SHOWSTRING=0;
int SHOWHELP=0;
float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
int window; 
int main_window;
int WindowWidth=800;
int WindowHeight=600;
int FullScreenMode=0;

//Declare the UI Bar
GLUI *glui;

void myGlutIdle( void )
{
  glutSetWindow(main_window);
  //Something should be done about the main screen losing focus:

  //  if(current_interactor!=-1) {
  //    pv[current_interactor]->idle(gMeshList);
  //  glutPostRedisplay();
}


void InitGraphics(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
  glutInitWindowSize(WindowWidth, WindowHeight);
  glutInitWindowPosition(80, 80);

  main_window = glutCreateWindow("BezView");
  glutDisplayFunc       (DrawGLScene);
  glutIdleFunc          (DrawGLScene);
  glutReshapeFunc       (ResizeGLScene);
  glutKeyboardFunc      (KeyPressed);
  glutSpecialFunc       (SpecialKeyPressed);

  glClearColor(0, 0., 0, 0.0);
  glClearDepth(1.0);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glLoadIdentity();
  if (FullScreenMode) {
    glutFullScreen();
  }
  glMatrixMode(GL_MODELVIEW);
  //This is much nicer, none of this 4x4 nonsense
  //But it doesn't work with UI-Ball
  //glMatrixMode(GL_PROJECTION);  

  //Enable for Transparency:
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  /****************************************/
  /*         Here's the GLUI code         */
  /****************************************/
  if (!CommandMode){
    GLUI_Master.set_glutKeyboardFunc(KeyPressed);
    GLUI_Master.set_glutSpecialFunc(SpecialKeyPressed);
    // GLUI_Master.set_glutMouseFunc(mouse);
    GLUI_Master.set_glutReshapeFunc(ResizeGLScene);
    GLUI_Master.set_glutIdleFunc(myGlutIdle );

    glui= GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_LEFT);
    glui->set_main_gfx_window(main_window);

    GLUI_Spinner *densityCut_spinner =
      glui->add_spinner( "Density Cut:", GLUI_SPINNER_FLOAT, &DensityCut, 1,Recolor);
    densityCut_spinner->set_float_limits( 0, 5);

    GLUI_Spinner *densityWidth_spinner =
      glui->add_spinner( "Density Width:", GLUI_SPINNER_FLOAT, &DensityWidth, 1,Recolor );
    densityWidth_spinner->set_float_limits( 0, 5);

    GLUI_Spinner *spacialWidth_spinner =
      glui->add_spinner( "Spacial Width:", GLUI_SPINNER_FLOAT, &WIDTH, 1,ChangeDraw );
    spacialWidth_spinner->set_float_limits( 0, 10);

    GLUI_Spinner *spacialCut_spinner =
      glui->add_spinner( "Spacial Cut:", GLUI_SPINNER_FLOAT, &CUT, 1,ChangeDraw );
    spacialCut_spinner->set_float_limits( -.5, .5);

    GLUI_Spinner *Variable_spinner =
      glui->add_spinner( "Variable:", GLUI_SPINNER_INT, &varC, 1,Recolor );
    Variable_spinner->set_int_limits( 0, 9);

    GLUI_Spinner *Colormap_spinner =
      glui->add_spinner( "Color Map:", GLUI_SPINNER_INT, &COLORBAR, 1,Recolor );
    Colormap_spinner->set_int_limits( 0, 9);

    GLUI_Spinner *PointSize_spinner =
      glui->add_spinner( "Point Size:", GLUI_SPINNER_INT, &POINTSIZE );
    PointSize_spinner->set_int_limits( 0, 20);

    glui->add_checkbox( "Delaunay", &Delaunay, 1, Recolor );

    glui->add_checkbox( "Log?", &loggy, 1, Recolor );

    glui->add_checkbox( "Scale Colormap?", &VALEDIT, 1, Recolor );

    glui->add_checkbox( "Show Help?", &SHOWHELP);

    GLUI_Translation *trans_xy = 
      new GLUI_Translation(glui, "Objects XY", GLUI_TRANSLATION_XY, obj_pos);
    trans_xy->set_speed( 0.01f );
    GLUI_Translation *trans_z = 
      new GLUI_Translation(glui, "Objects Z", GLUI_TRANSLATION_Z, &obj_pos[2]);
    trans_z->set_speed( 0.01f );
    GLUI_Rotation *view_rot = new GLUI_Rotation(glui, "Objects", view_rotate);
    view_rot->set_spin( .5 );

    new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );

    glui->set_main_gfx_window(main_window);
  }

  if (NumberToDraw==0){
    NumberToDraw = Nc;
  }

  Recolor(0);
  glutMainLoop();
}






void Recolor( int call)
{

  maxval = -HUGE_VAL;
  minval = HUGE_VAL;

  float rrr,ggg,bbb;
  int j = 0;

  //Go through and find the Max/Min Vals
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

  //Now go through again, coloring
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

  ChangeDraw(0);
}

std::vector<unsigned int>ind;
std::vector<unsigned int>dind;

void ChangeDraw(int call)
{
  ind.clear();
  dind.clear();

  if(Delaunay){
    for(int i = 0; i < Nf; ++i ){
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
    for(int i = 0; i < NumberToDraw; ++i ){
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



// The main drawing function
void DrawGLScene()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glTranslatef( obj_pos[0], obj_pos[1], -obj_pos[2] ); 
  glMultMatrixf( view_rotate );

  //Show Coodinates
  if (SHOWSTRING){
    std::stringstream ss;//create a stringstream
    ss << obj_pos[0]; ss << " ";
    ss << obj_pos[1]; ss << " ";
    ss << obj_pos[2]; ss << " ";
    ss << AngleX; ss << " ";
    ss << AngleY; ss << " ";
    ss << AngleZ; ss << " ";
    std::string str(ss.str());
    const char* cstr1 = str.c_str();
    glutPrint( 0 , 0 , 0  , cstr1 , 1, 1 , 1 , 0.5f );
  }

  //Show a help screen
  if( SHOWHELP ){
    int NLines = 16;
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
    sprintf(help[12],"S - Toggle Coordinates");
    sprintf(help[13],"D - Toggle Delaunay");
    sprintf(help[14],"M - Toggle Density Color Map");
    sprintf(help[15],"Esc - Quit Program");
    for( int i=0 ; i<NLines ; ++i ){
      glutPrint(.8,1+i*dy,.001, help[i] , 1,1 , 1 , 0.5f );
    }
  }

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  if(Delaunay){
    glDrawElements(GL_LINES, dind.size(), GL_UNSIGNED_INT, &dind[0] );
  }
  else{
    glPointSize(POINTSIZE);
    glDrawElements(GL_POINTS, ind.size(), GL_UNSIGNED_INT, &ind[0] );
  }

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);

  if(CommandMode){
    TakeScreenshot("test.ppm");
    exit(1);
  }
  glutSwapBuffers();
}
//End of the Display Function


void ResizeGLScene(int Width, int Height)
{
  if (Height==0) Height=1;
  glViewport(0, 0, Width, Height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(45.0f, (GLfloat)Width/(GLfloat)Height, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
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

void TakeScreenshot(const char *fname)
{
  int dimx = WindowWidth;
  int dimy = WindowHeight;
  FILE *fp=fopen(fname,"wb");
  std::vector<unsigned char> out(3*dimx*dimy);
  glPixelStorei(GL_PACK_ALIGNMENT,1); /* byte aligned output */
  glReadPixels(0,0,dimx,dimy, GL_RGB,GL_UNSIGNED_BYTE,&out[0]);
  fprintf(fp,"P3\n%d %d\n255\n",dimx,dimy);
  for (int y=0;y<dimy;y++) { /* flip image bottom-to-top */
    fwrite(&out[3*(dimy-1-y)*dimx],1,3*dimx,fp);
  }
  fclose(fp);
}
