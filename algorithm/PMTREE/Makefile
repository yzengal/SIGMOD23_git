
##
CC = gcc
CXX = g++
LIBS = 
CFLAGS = --std=c++11 -W -Wall -Wno-sign-compare -O3 -s -pipe -mmmx -msse -msse2 -msse3 # -mcmodel=medium
MEM = -D WATCH_MEM
DIM = 

all: TestPMT	
global.o: global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.cpp $(LIBS)
	
MemoryManager.o: MemoryManager.h MemoryManager.cpp global.o global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.o MemoryManager.cpp $(LIBS)

BASELINE_PMTREE.o: BASELINE_PMTREE.h BASELINE_PMTREE.cpp global.h global.cpp global.o MemoryManager.o MemoryManager.h MemoryManager.cpp 
	$(CXX) $(CFLAGS) -c global.o MemoryManager.o BASELINE_PMTREE.cpp $(LIBS)
	
TestPMT: TestPMtree.cpp global.h global.cpp global.o BASELINE_PMTREE.h BASELINE_PMTREE.cpp BASELINE_PMTREE.o MemoryManager.o MemoryManager.h MemoryManager.cpp 
	$(CXX) $(CFLAGS) -o TestPMT global.o MemoryManager.o BASELINE_PMTREE.o TestPMtree.cpp $(LIBS)


.PHONY: clean
clean:
		-@rm *.o *.gcno *~ 2> /dev/null || true
		-@rm TestPMT 2> /dev/null || true
		