
# ------------------------------------------------------------------------------
#
# Command line macros:
#
# INSTALL_TOP ... place to put includes and library files
# LUA_HOME    ... where to find lua header and library files 
#
# If this is a local build, then you should set the environment variable
# LUA_CPATH to the local directory. On bash, this will work:
#
# export LUA_CPATH="$LUA_CPATH;`pwd`/lib/?.so"
#
# Absolute paths may be used instead, which will be useful if you are doing a
# remote install.
#
# ------------------------------------------------------------------------------



# System-specific things.
# ------------------------------------------------------------------------------

# C compiler icc, gcc, etc if other than system default
# CC = cc

# C++ compiler icpc, g++, etc if other than system default
# CXX = c++

# sometimes only -fpic works
FPIC = -fPIC

# Warning level flags
WARN = -Wall

# robust optimazation level
OPTIM = -O0

# debug flags, use -g for debug symbols
DEBUG =

# location of Lua install on this system
LUA_HOME ?= $(PWD)/lua

# location of GLFW install on this system
GLFW_HOME = $(PWD)/glfw

# where to install luview library and include
INSTALL_TOP = $(PWD)

# C Flags
CFLAGS = $(WARN) $(OPTIM) $(DEBUG) $(FPIC)


# Configuration for common platforms. If you need to use a different linker,
# archiver, or C libraries then uncomment the UNAME = Custom line below, and
# edit the custom first block following.
# ------------------------------------------------------------------------------
UNAME = $(shell uname)
#UNAME = Custom
# ------------------------------------------------------------------------------
#
#
ifeq ($(UNAME), Custom)
# common for library links on Linux
CLIBS = -lm -ldl
# command for building shared libraries (this works for most Linux systems)
SO = $(CC) -O -shared
# command for generating static archives
AR = ar rcu
endif

ifeq ($(UNAME), Linux)
SO     = $(CC) -O -shared
AR     = ar rcu
CLIBS  = -lm -ldl
ARCH_LUA  = "linux -ULUA_USE_READLINE"
ARCH_GLFW = x11
GL_L      = -lXrandr -lX11 -lGLU -lGL -lglfw -L$(PWD)/lib
endif

ifeq ($(UNAME), Darwin)
SO     = $(CC) -O -bundle -undefined dynamic_lookup
AR     = ar rcu
CLIBS  =
ARCH_LUA  = macosx
ARCH_GLFW = cocoa
GL_L      = -framework OpenGL -framework Cocoa -lglfw
endif



# -------------------------------------------------
# Ensure these values are passed to child Makefiles
# -------------------------------------------------
export CC
export CXX
export CFLAGS
export LUA_HOME
export GLFW_HOME
export SO
export AR
export CLIBS
export GL_L
# -------------------------------------------------


BUILD_TOP   = $(shell pwd)
LIB_SO      = luview.so
LIB_A       = libluview.a
LUA_A   = lib/liblua.a
LUNUM_A = lib/liblunum.a
GLFW_A  = lib/libglfw.a

export LUVIEW_SO = $(BUILD_TOP)/src/$(LIB_SO)
export LUVIEW_A  = $(BUILD_TOP)/src/$(LIB_A)

INSTALL_SO  = $(INSTALL_TOP)/lib/$(LIB_SO)
INSTALL_A   = $(INSTALL_TOP)/lib/$(LIB_A)

# ------------------------------------------------------------------------------
# Library build aliases
# ------------------------------------------------------------------------------
default :  $(INSTALL_SO) $(INSTALL_A)
lua     :  $(LUA_A)
lunum   :  $(LUNUM_A)
glfw    :  $(GLFW_A)

H1 = luview.h
H2 = numarray.h

HEADERS = \
	$(INSTALL_TOP)/include/$(H1) \
	$(INSTALL_TOP)/include/$(H2)

config : 
	@echo "CC           = $(CC)"
	@echo "CXX          = $(CXX)"
	@echo "FPIC         = $(FPIC)"
	@echo "WARN         = $(WARN)"
	@echo "OPTIM        = $(OPTIM)"
	@echo "DEBUG        = $(DEBUG)"
	@echo "AR           = $(AR)"
	@echo "SO           = $(SO)"
	@echo "LUA_HOME     = $(LUA_HOME)"
	@echo "INSTALL_TOP  = $(INSTALL_TOP)"


LUA_VERSION  = lua-5.2.0
GLFW_VERSION = glfw-2.7.2
LUNUM_BRANCH = luview
LUNUM_GIT    = git@github.com:jzrake/lunum.git
SERVE_SRC    = http://zrake.webfactional.com/media

$(LUNUM_A) :
	@echo "Downloading and installing Lunum"
	@mkdir -p bin lib include
	git clone -b $(LUNUM_BRANCH) $(LUNUM_GIT)
	make -C lunum install CC=$(CC) LUA_HOME=$(PWD) INSTALL_TOP=$(PWD)
	rm -rf lunum

$(LUA_A) : 
	@echo "Downloading and installing Lua..."
	@mkdir -p bin lib include
	wget $(SERVE_SRC)/$(LUA_VERSION).tar.gz
	tar xvf $(LUA_VERSION).tar.gz
	make -C $(LUA_VERSION) $(ARCH_LUA) install CC=$(CC) INSTALL_TOP=$(PWD)
	cp src/*.lua lib/lua/5.2
	rm -rf share man $(LUA_VERSION) $(LUA_VERSION).tar.gz

$(GLFW_A) :
	@echo "Downloading and installing glfw..."
	@mkdir -p bin lib include
	wget $(SERVE_SRC)/$(GLFW_VERSION).tar.gz
	tar xvf $(GLFW_VERSION).tar.gz
	make -C $(GLFW_VERSION) $(ARCH_GLFW) CC=$(CC)
	cp -r $(GLFW_VERSION)/lib/$(ARCH_GLFW)/libglfw.a lib
	cp -r $(GLFW_VERSION)/include/* include
	rm -r $(GLFW_VERSION) $(GLFW_VERSION).tar.gz


test : $(LUVIEW_SO) $(LUVIEW_A)

all : default test

install : $(INSTALL_SO) $(INSTALL_A) $(HEADERS)

$(INSTALL_TOP)/include/$(H1) : 
	mkdir -p $(INSTALL_TOP)/include
	cp src/$(H1) $(INSTALL_TOP)/include

$(INSTALL_TOP)/include/$(H2) : 
	mkdir -p $(INSTALL_TOP)/include
	cp src/$(H2) $(INSTALL_TOP)/include

$(LUVIEW_SO) : $(LUA_A) $(LUNUM_A) $(GLFW_A) FORCE
	@make -C src $(LUVIEW_SO)

$(LUVIEW_A)  : $(LUA_A) $(LUNUM_A) $(GLFW_A) FORCE
	@make -C src $(LUVIEW_A)

test : FORCE
	@make -C test

$(INSTALL_SO) : $(LUVIEW_SO)
	mkdir -p $(INSTALL_TOP)/lib
	cp $(LUVIEW_SO) $(INSTALL_TOP)/lib

$(INSTALL_A) :  $(LUVIEW_A)
	mkdir -p $(INSTALL_TOP)/lib
	cp $(LUVIEW_A) $(INSTALL_TOP)/lib

clean :
	make -C test clean
	make -C src clean

realclean : clean
	rm -rf lib include

FORCE :
