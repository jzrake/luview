
default : main

main : main.cpp lua_object.hpp
	$(CXX) $(CFLAGS) -o $@ $< -I../include ../lib/liblua.a
