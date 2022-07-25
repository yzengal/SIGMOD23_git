
##
CC = gcc
CXX = g++
LIBS = 
CFLAGS = --std=c++11 -W -Wall -Wno-sign-compare -O3 -s -pipe -mmmx -msse -msse2 -msse3 # -mcmodel=medium
MEM = -D WATCH_MEM
DIM = 

all: TestMT
global.o: global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.cpp $(LIBS)
	
MemoryManager.o: MemoryManager.h MemoryManager.cpp global.o global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.o MemoryManager.cpp $(LIBS)
	
BASELINE_MTREE.o: BASELINE_MTREE.h BASELINE_MTREE.cpp global.h global.cpp global.o MemoryManager.o MemoryManager.h MemoryManager.cpp 
	$(CXX) $(CFLAGS) -c global.o MemoryManager.o BASELINE_MTREE.cpp $(LIBS)

TestMT: TestMtree.cpp global.h global.cpp global.o BASELINE_MTREE.h BASELINE_MTREE.cpp BASELINE_MTREE.o  MemoryManager.o MemoryManager.h MemoryManager.cpp 
	$(CXX) $(CFLAGS) -o TestMT global.o MemoryManager.o BASELINE_MTREE.o TestMtree.cpp $(LIBS)

.PHONY: clean
clean:
		-@rm *.o *.gcno *~ 2> /dev/null || true
		-@rm TestMT 2> /dev/null || true
		