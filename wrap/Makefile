
CFLAGS = -Wall

default : main

main : main.cpp lua_object.hpp
	$(CXX) $(CFLAGS) -o $@ $< -I../dep/include ../dep/lib/liblua.a
