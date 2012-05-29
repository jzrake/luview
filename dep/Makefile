
WGET ?= wget
UNTAR ?= tar -xvf
RM ?= rm
CP ?= cp
MKDIR ?= mkdir

SERVE_SRC = http://zrake.webfactional.com/media

DEP_LUA    = lua-5.2.0
DEP_GLFW   = glfw-2.7.5
DEP_LUNUM  = lunum-0.5.3
DEP_TETGEN = tetgen-1.4

PKG = $(DEP_LUA) $(DEP_GLFW) $(DEP_TETGEN) $(DEP_LUNUM)

default : all

all : $(PKG)

.PHONY : clean

%.tar.gz :
	$(WGET) $(SERVE_SRC)/$@

$(DEP_LUA) : $(DEP_LUA).tar.gz
	$(UNTAR) $<
	$(MAKE) -C $@ $(ARCH_LUA) install CC=$(CC) INSTALL_TOP=$(PWD)/dep

$(DEP_GLFW) : $(DEP_GLFW).tar.gz
	$(UNTAR) $<
	$(MAKE) -C $@ $(ARCH_GLFW)-install CC=$(CC) PREFIX=$(PWD)/dep

$(DEP_TETGEN) : $(DEP_TETGEN).tar.gz
	$(UNTAR) $<
	$(MAKE) -C $@ tetlib
	$(MKDIR) -p lib include
	$(CP) $@/libtet.a lib
	$(CP) $@/tetgen.h include

$(DEP_LUNUM) : $(DEP_LUNUM).tar.gz $(DEP_LUA)
	$(UNTAR) $<
	$(MAKE) -C $@ install INSTALL_TOP=$(PWD)/dep LUA_HOME=$(PWD)/dep

clean :
	$(RM) -rf $(PKG) *.tar.gz bin lib include share man
