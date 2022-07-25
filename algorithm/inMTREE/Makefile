##
CC = gcc
CXX = g++
LIBS = 
CFLAGS = --std=c++11 -W -Wall -Wunused-parameter -Wunused-variable -Wno-sign-compare -O3 -s -pipe -mmmx -msse -msse2 -msse3 # -mcmodel=medium
MEM = -D WATCH_MEM
DIM = 

all: TestMtree
monitor.o: monitor.h monitor.cpp
	$(CXX) $(CFLAGS) -c monitor.cpp $(LIBS) $(MEM)
	
global.o: global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.cpp $(LIBS) $(MEM) 

TestMtree: TestMtree.cpp global.h global.cpp global.o MtreeAPI.h mtree.h functions.h
	$(CXX) $(CFLAGS) -o TestMtree global.o TestMtree.cpp $(LIBS)
	
.PHONY: clean
clean:
		-@rm *.o *.gcno *~ 2> /dev/null || true
		-@rm TestMtree 2> /dev/null || true
		
		
