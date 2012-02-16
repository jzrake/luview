
GL_L = -framework OpenGL -framework Cocoa

LUA = $(LUA_HOME)/bin/lua

default : $(LUVIEW_A) $(LUVIEW_SO)

%.o : %.c
	$(CC) $(CFLAGS) -std=c99 -c $<

luview.o : luview.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I$(LUA_HOME)/include -I$(GLFW_HOME)/include

$(LUVIEW_A)  : luview.o $(GLFW_HOME)/lib/libglfw.a
	$(AR) $@ $?

$(LUVIEW_SO) : luview.o $(GLFW_HOME)/lib/libglfw.a
	$(SO) -o $(LUVIEW_SO) $^ $(GL_L)

clean :
	rm -f *.o $(LUVIEW_A) $(LUVIEW_SO) *.lc
