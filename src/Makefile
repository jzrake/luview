
GL_L = -framework OpenGL -framework Cocoa

LUA = ../bin/lua

default : $(LUVIEW_A) $(LUVIEW_SO)

%.o : %.c
	$(CC) $(CFLAGS) -std=c99 -c $<

luview.o : luview.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I../include -I../include

$(LUVIEW_A)  : luview.o image.o ../lib/libglfw.a ../lib/liblunum.a
	$(AR) $@ $?

$(LUVIEW_SO) : luview.o image.o ../lib/libglfw.a ../lib/liblunum.a
	$(SO) -o $(LUVIEW_SO) $^ $(GL_L)

clean :
	rm -f *.o $(LUVIEW_A) $(LUVIEW_SO) *.lc
