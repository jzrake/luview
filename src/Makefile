


LUA = ../bin/lua

default : $(LUVIEW_A) $(LUVIEW_SO)

%.o : %.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I../include

%.o : %.cpp
	$(CXX) $(CFLAGS) -c $< -I../include

%.lc : %.lua
	$(LUA) bin2c.lua +$< > $@

$(LUVIEW_A)  : luview.o tesselate.o ../lib/libglfw.a ../lib/liblunum.a ../lib/libtet.a
	$(AR) $@ $?

$(LUVIEW_SO) : luview.o tesselate.o ../lib/libglfw.a ../lib/liblunum.a ../lib/libtet.a
	$(SO) -o $(LUVIEW_SO) $^ $(GL_L)

clean :
	rm -f *.o $(LUVIEW_A) $(LUVIEW_SO) *.lc
