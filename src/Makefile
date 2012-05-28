

GL_INC = -I/usr/X11/include
H5_INC = -I/Library/Science/hdf5/include
H5_LIB = -L/Library/Science/hdf5/lib -lz -lhdf5

LUA = ../bin/lua
LIB = ../lib/libglfw.a ../lib/liblunum.a ../lib/libtet.a

OBJ = 	luview.o \
	shader.o \
	dsource.o \
	artists.o \
	ren2tex.o \
	colorbar.o \
	pyplotcm.o \
	tesselate.o \
	h5lua.o \
	glInfo.o \


default : $(LUVIEW_A) $(LUVIEW_SO)

volume.o : volume.cpp
	$(CXX) $(CFLAGS) -c $< -I../include $(GL_INC)

ren2tex.o : ren2tex.c
	$(CC) $(CFLAGS) -c $< -I../include $(GL_INC)

h5lua.o : h5lua.c
	$(CC) $(CFLAGS) -c $< -std=c99 -I../include -D__LUVIEW_USE_HDF5 $(H5_INC)

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
	$(SO) -o $(LUVIEW_SO) $^ $(GL_L) $(H5_LIB)

clean :
	rm -f *.o $(LUVIEW_A) $(LUVIEW_SO) *.lc
	make -C tetgen clean
