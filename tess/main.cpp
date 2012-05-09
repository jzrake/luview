//TODO:

//Toggle Colorbar
//Get Transparencies to work better
//Do Vectors
//Do Vector Field Lines
//Do surface rendering
//Restructure Code


#include <list>
#include <stdlib.h>
#include <stdio.h>
#include "Cell.h"

void get_rgb( double , float * , float * , float * , int);
void InitGraphics(int argc, char **argv);


struct Cell * theCells;

int Nc, Nf, Nq;
int * Cell0;
int * Cell1;
int CommandMode = 0;
int COLORBAR = 3;
int NumberToDraw=0;
float DensityCut=1.0;
float DensityWidth=1.1;
int Delaunay=0;
float CUT=.1;
float WIDTH=.01;
int varC=1;
int POINTSIZE=1;
int loggy=0;
int VALEDIT=0;
float obj_pos[] = { 0.5, 0.5, 2.5 };
float AngleX = 0;
float AngleY = 0;
float AngleZ = 0;

float * colordata;
float * pointdata;
double minval, maxval;


int main(int argc, char *argv[])
{

  char* DIR_BIN = (char *)"output999.bin";

  if (argc < 2)
  {
    printf("For manual viewing please specifiy file and 0 for manual mode\n Example: './vbin TestOutput.bin 0\n");
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
    POINTSIZE = atoi(argv[10]);
    loggy = atoi(argv[11]);
    obj_pos[0] = atof(argv[12]); //this can be done nicer
    obj_pos[1] = atof(argv[13]);
    obj_pos[2] = atof(argv[14]);
    AngleX = atof(argv[15]);
    AngleY = atof(argv[16]);
    AngleZ = atof(argv[17]);
    COLORBAR = atoi(argv[18]);
    VALEDIT = atoi(argv[19]); //change this variable name, its confusing
  }
  FILE *pFile;
  pFile = fopen(DIR_BIN,"rb");
if (!pFile){ printf("Could not find output file\n");exit(0);}

  //first three numbers in the file
  size_t err;
  err = fread( &Nq , sizeof(int) , 1 , pFile ); //NUmber of doubles per cell
  err = fread( &Nc , sizeof(int) , 1 , pFile ); //Number of cells
  err = fread( &Nf , sizeof(int) , 1 , pFile ); //number of faces

  theCells = new struct Cell[Nc];
  Cell0 = (int *) malloc( Nf*sizeof(int) );//new int[Nf];
  Cell1 = (int *) malloc( Nf*sizeof(int) );//new int[Nf];

  printf("Stage 1 Complete, Nq = %d Nc = %d Nf = %d\n",Nq,Nc,Nf);

  int j = 0;
  while( j<Nc ){
    double Q[Nq];
    int bc;

    err = fread(  Q , sizeof(double) , Nq , pFile);
    err = fread( &bc, sizeof(int)    , 1  , pFile);

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
    err = fread( &(Cell0[j]) , sizeof(int) , 1 , pFile );
    err = fread( &(Cell1[j]) , sizeof(int) , 1 , pFile );
  }


  for( j = 0; j < Nf; ++j ){
    theCells[Cell0[j]].myFriends.push_back( &(theCells[Cell1[j]]) );
    theCells[Cell1[j]].myFriends.push_back( &(theCells[Cell0[j]]) );
  }
  
  //Begin Visuallization
  InitGraphics(argc, argv);

  //It should never actually get here..
  free(Cell0);
  free(Cell1);
  printf("err is %d\n",(int)err);
  return 1;
}
