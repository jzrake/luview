


LUA = ../bin/lua
LIB = ../lib/libglfw.a ../lib/liblunum.a ../lib/libtet.a
OBJ = luview.o tesselate.o shader.o

default : $(LUVIEW_A) $(LUVIEW_SO)

%.o : %.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I../include

%.o : %.cpp
	$(CXX) $(CFLAGS) -c $< -I../include

%.lc : %.lua
	$(LUA) bin2c.lua +$< > $@

../lib/libtet.a :
	make -C tetgen tetlib
	cp tetgen/libtet.a ../lib

$(LUVIEW_A)  : $(LIB) $(OBJ)
	$(AR) $@ $?

$(LUVIEW_SO) : $(LIB) $(OBJ)
	$(SO) -o $(LUVIEW_SO) $^ $(GL_L)

clean :
	rm -f *.o $(LUVIEW_A) $(LUVIEW_SO) *.lc
	make -C tetgen clean
