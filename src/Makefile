
GL_L = -framework OpenGL -framework Cocoa

LUA = ../bin/lua

default : $(LUVIEW_A) $(LUVIEW_SO)

%.o : %.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I../include -I../include

%.lc : %.lua
	$(LUA) bin2c.lua +$< > $@

types.o : proxy.lc

$(LUVIEW_A)  : luview.o ../lib/libglfw.a ../lib/liblunum.a
	$(AR) $@ $?

$(LUVIEW_SO) : luview.o ../lib/libglfw.a ../lib/liblunum.a
	$(SO) -o $(LUVIEW_SO) $^ $(GL_L)

clean :
	rm -f *.o $(LUVIEW_A) $(LUVIEW_SO) *.lc
