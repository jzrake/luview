

LUA = $(LUA_HOME)/bin/lua
LIB = -L$(BUILD_TOP)/dep/lib $(GL_LIB) -lglfw -ltet -llunum $(H5_LIB)

OBJ = 	luview.o \
	shader.o \
	dsource.o \
	artists.o \
	ren2tex.o \
	colorbar.o \
	pyplotcm.o \
	tesselate.o \
	h5lua.o \
	nbody.o \
	icrv.o \
	glInfo.o \


LUA_TARGET = $(patsubst %.lua, $(LUA_LIB)/%.lua, $(wildcard *.lua))

default : $(LUVIEW_SO) $(LUA_TARGET)

h5lua.o : h5lua.c
	$(CC) $(CFLAGS) -c $< -std=c99 $(H5_INC) $(GL_INC)

%.o : %.c
	$(CC) $(CFLAGS) -c $< $(GL_INC)

%.o : %.cpp
	$(CXX) $(CFLAGS) -c $< $(GL_INC)

%.lc : %.lua
	$(LUA) bin2c.lua +$< > $@

$(LUA_LIB)/%.lua : %.lua
	cp $< $@

$(LUVIEW_A) : $(OBJ)
	$(AR) $@ $? $(LIB)

$(LUVIEW_SO) : $(OBJ)
	mkdir -p $(LUA_LIB)
	$(SO) -o $@ $^ $(LIB)

clean :
	rm -f *.o *.lc
