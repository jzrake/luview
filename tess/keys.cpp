#include "theGlut.h"
#include <glui.h>


//External Viewing Parameters from TheGlut 
////These should be passed in a better way :(
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
extern int SHOWSTRING;
extern int SHOWHELP;
extern float view_rotate[];
extern int FullScreenMode;
extern int main_window;
extern int WindowWidth;
extern int WindowHeight;
extern GLUI *glui;



void AngleUpdate()
{
  double c1 = cos(AngleX);
  double c2 = cos(AngleY);
  double c3 = cos(AngleZ);
  double s1 = sin(AngleX);
  double s2 = sin(AngleY);
  double s3 = sin(AngleZ);

  view_rotate[0]  = c2*c3;
  view_rotate[1]  = c2*s3;
  view_rotate[2]  = -s2;

  view_rotate[4]  = c3*s1*s2 - c1*s3;
  view_rotate[5]  = c1*c3 + s1*s2*s3;
  view_rotate[6]  = c2*s1;

  view_rotate[8]  = s1*s3 + c1*c3*s2;
  view_rotate[9]  = c1*s2*s3 - c3*s1;
  view_rotate[10] = c1*c2;
}


void SpecialKeyPressed(int key, int x, int y)
{
  if      (key == GLUT_KEY_RIGHT) {AngleY+=.05;AngleUpdate();}
  else if (key == GLUT_KEY_LEFT ) {AngleY-=.05;AngleUpdate();}

  if      (key == GLUT_KEY_DOWN ){AngleX+=.05;AngleUpdate();}
  else if (key == GLUT_KEY_UP   ){AngleX-=.05;AngleUpdate();}

  glutPostRedisplay();
}

void KeyPressed(unsigned char key, int x, int y)
{
  static const int ESCAPE = 27;

  if (key == ESCAPE || key == 'q') {
    glutDestroyWindow(main_window);
    exit(0);
  }

  else if (key =='-' ) {AngleZ += .05;AngleUpdate();}
  else if (key == '=' ) {AngleZ -= .05;AngleUpdate();}
  else if (key == 'a') {
    obj_pos[0] += .1;
  }
  else if (key == 'd') {
    obj_pos[0] -= .1;
  }
  else if (key == 'w') {
    obj_pos[2] -= .1;
  }
  else if (key == 's') {
    obj_pos[2] += .1;
  }
  else if (key == 'r') {
    obj_pos[1] -= .1;
  }
  else if (key == 'f') {
    obj_pos[1] += .1;
  }

  else if (key == 'm') {
    if (NumberToDraw * 2 <= Nc){NumberToDraw*=2 ;
      ChangeDraw(0);}
      glui->sync_live();
  }
  else if (key == 'l') {
    NumberToDraw = NumberToDraw / 2;
    ChangeDraw(0);
    glui->sync_live();
  }


  //Density Cuts
  else if (key == 'P') {
    DensityCut *=1.1;
    if(VALEDIT){Recolor(0);}
    else{ChangeDraw(0);}
    glui->sync_live();
  }
  else if (key == 'p') {
    DensityCut /=1.1;
    if(VALEDIT){Recolor(0);}
    else{ChangeDraw(0);}
    glui->sync_live();
  }
  else if (key == 'O') {
    DensityWidth *=1.01;
    if(VALEDIT){Recolor(0);}
    else{ChangeDraw(0);}
    glui->sync_live();
  }
  else if (key == 'o') {
    DensityWidth /=1.01;
    if(VALEDIT){Recolor(0);}
    else{ChangeDraw(0);}
    glui->sync_live();
  }




  else if (key == 'D') {
    Delaunay=!Delaunay;
    Recolor(0);
    glui->sync_live();
  }


  //Spacial Cuts
  else if (key == 'C') {
    CUT += .01;
    ChangeDraw(0);
    glui->sync_live();
  }
  else if (key == 'c') {
    CUT -= .01;
    ChangeDraw(0);
    glui->sync_live();
  }
  else if (key == 'V') {
    WIDTH += .01;
    ChangeDraw(0);
    glui->sync_live();
  }
  else if (key == 'v') {
    WIDTH -= .01;
    ChangeDraw(0);
    glui->sync_live();
  }

  //ColorBar

  else if (key == '[') {
    COLORBAR++;
    Recolor(0);
    glui->sync_live();
  }
  else if (key == ']') {
    COLORBAR--;
    Recolor(0);
    glui->sync_live();
  }

  else if (key == '?') {
    POINTSIZE++;
    glui->sync_live();
  }
  else if (key == '/') {
    POINTSIZE--;
    glui->sync_live();
  }
  else if (key == 'L') {
    loggy = !loggy;
    Recolor(0);
    glui->sync_live();
  }
  else if (key == 'S') {
    SHOWSTRING = !SHOWSTRING;
    glui->sync_live();
  }
  else if (key == 'H') {
    SHOWHELP = !SHOWHELP;
    glui->sync_live();
  }
  else if (key == 'M') {
    VALEDIT = !VALEDIT;
    Recolor(0);
    glui->sync_live();
  }

  /* else if (key == '!') {
     GLUI::hide( glui );
     }
     else if (key == '@') {
     GLUI::show( glui );
     }*/

  //number keys for variables
  else if (key == '0'){varC = 0; Recolor(0);glui->sync_live();}
  else if (key == '1'){varC = 1; Recolor(0);glui->sync_live();}
  else if (key == '2'){varC = 2; Recolor(0);glui->sync_live();}
  else if (key == '3'){varC = 3; Recolor(0);glui->sync_live();}
  else if (key == '4'){varC = 4; Recolor(0);glui->sync_live();}
  else if (key == '5'){varC = 5; Recolor(0);glui->sync_live();}
  else if (key == '6'){varC = 6; Recolor(0);glui->sync_live();}
  else if (key == '7'){varC = 7; Recolor(0);glui->sync_live();}
  else if (key == '8'){varC = 8; Recolor(0);glui->sync_live();}
  else if (key == '9'){varC = 9; Recolor(0);glui->sync_live();}
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

