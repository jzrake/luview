EXE  = vbin

#GLUT = /usr/X11R6
LIBS = -L/usr/lib -lglut -lGLU -lGL -lXmu -lXext -lX11
INCL = -I/usr/include

CC = g++

default: $(EXE)

$(EXE): $(EXE).cpp $(SRCS)
	$(CC) -c colorbar.cpp
	$(CC) -O3 -Wall -o $@ $< colorbar.o $(INCL) $(LIBS)

clean:
	rm -f *.o $(EXE)
