

GL_INCL = -I/usr/X11/include
LUA = ../bin/lua
LIB = ../lib/libglfw.a ../lib/liblunum.a ../lib/libtet.a
#OBJ = 	luview.o \
	tesselate.o \
	shader.o \
	colorbar.o \
	pyplotcm.o \
	nbody.o \
	volume.o \
	glInfo.o \
	dsource.o \

OBJ = 	luview.o \
	shader.o \
	dsource.o \
	artists.o \
	ren2tex.o \
	glInfo.o \


default : $(LUVIEW_A) $(LUVIEW_SO)

volume.o : volume.cpp
	$(CXX) $(CFLAGS) -c $< -I../include $(GL_INCL)

ren2tex.o : ren2tex.c
	$(CC) $(CFLAGS) -c $< -I../include $(GL_INCL)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

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
