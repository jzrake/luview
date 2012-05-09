EXE  = main

UNAME = $(shell uname)


ifeq ($(UNAME), Linux)
LINOPS = -I/usr/include/GL -lglut
MACOPS = 
GLUT = /usr/
endif

ifeq ($(UNAME), Darwin)
GLUT = /opt/local/
MACOPS = -framework GLUT -framework OpenGL
LINOPS = 
endif


LIBS = -L$(GLUT)lib -lX11 -lglui $(MACOPS) $(LINOPS)
INCL = -I$(GLUT)include


CC = g++

default: viewer

viewer: $(EXE).cpp theGlut.cpp $(SRCS)
	$(CC) -c colorbar.cpp
	$(CC) -O3 -Wall -o $@ $< theGlut.cpp colorbar.o $(INCL) $(LIBS)

clean:
	rm -f *.o viewer
