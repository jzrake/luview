#ifndef THEGLUT_H
#define THEGLUT_H
 



void DrawGLScene();
void Recolor(int call);
void ChangeDraw(int call);
void ResizeGLScene(int Width, int Height);
void KeyPressed(unsigned char key, int x, int y);
void SpecialKeyPressed(int key, int x, int y);
void glutPrint(float x, float y, float z, const char* text, float r, float g, float b, float a);
void TakeScreenshot(const char *fname);
void get_rgb( double , float * , float * , float * , int);


 
#endif
