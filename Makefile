
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

# HDF5 (optional)
HDF5_HOME ?=


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
ARCH_LUA  = linux
ARCH_GLFW = x11
GL_LIB    = -lXrandr -lX11 -lGLU -lGL
GL_INC    = -I/usr/X11/include
endif

ifeq ($(UNAME), Darwin)
SO     = $(CC) -O -bundle -undefined dynamic_lookup
AR     = ar rcu
CLIBS  =
ARCH_LUA  = macosx
ARCH_GLFW = cocoa
GL_LIB    = -framework OpenGL -framework Cocoa
GL_INC    = -I/usr/X11/include
endif


BUILD_TOP = $(shell pwd)
LUA_LIB = $(BUILD_TOP)/lib/lua/5.2
LUVIEW_SO = $(LUA_LIB)/luview.so

GL_INC += -I$(BUILD_TOP)/dep/include
GL_LIB += -L$(BUILD_TOP)/dep/lib

ifneq ($(HDF5_HOME), )
H5_INC = -I$(HDF5_HOME)/include -D__LUVIEW_USE_HDF5
H5_LIB = -L$(HDF5_HOME)/lib -lz -lhdf5
endif




# -------------------------------------------------
# Ensure these values are passed to child Makefiles
# -------------------------------------------------
export CC
export CXX
export SO
export AR
export CLIBS
export CFLAGS
export BUILD_TOP
export LUA_HOME
export LUA_LIB
export ARCH_LUA
export ARCH_GLFW
export GL_LIB
export GL_INC
export H5_INC
export H5_LIB
export LUVIEW_SO
# -------------------------------------------------


default : $(LUVIEW_SO)

.PHONY : $(LUVIEW_SO) dep clean

dep : 
	$(MAKE) -C dep

$(LUVIEW_SO) : dep
	$(MAKE) -C src

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

clean :
	$(MAKE) -C src clean
