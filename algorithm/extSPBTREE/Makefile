
##
CC = gcc
CXX = g++
LIBS = 
CFLAGS = --std=c++11 -W -Wall -Wno-sign-compare -O3 -s -pipe -mmmx -msse -msse2 -msse3 # -mcmodel=medium
MEM = -D WATCH_MEM
DIM = 

all: TestSPBT
global.o: global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.cpp $(LIBS)
	
MemoryManager.o: MemoryManager.h MemoryManager.cpp global.o global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.o MemoryManager.cpp $(LIBS)
	
TestSPBT: TestSPBtree.cpp SPBtree.h btree.h hilbert4.h btree_multimap.h global.h global.cpp global.o MemoryManager.o MemoryManager.h MemoryManager.cpp
	$(CXX) $(CFLAGS) -o TestSPBT global.o MemoryManager.o TestSPBtree.cpp $(LIBS)

.PHONY: clean
clean:
		-@rm *.o *.gcno *~ 2> /dev/null || true
		-@rm TestSPBT 2> /dev/null || true
		