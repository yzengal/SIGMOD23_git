
CC = gcc
CXX = g++
LIBS = 
CFLAGS = --std=c++11 -W -Wall -Wno-sign-compare -O3 -s -pipe -mmmx -msse -msse2 -msse3 # -mcmodel=medium
MEM = -D WATCH_MEM
DIM = 

all: TestLC
global.o: global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.cpp $(LIBS)
	
MemoryManager.o: MemoryManager.h MemoryManager.cpp global.o global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.o MemoryManager.cpp $(LIBS)

basics.o: basics.h basics.c obj.h global.o global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.o basics.c $(LIBS)
	
bucket.o: bucket.h bucket.c basics.o basics.h basics.c obj.h global.o global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.o basics.o bucket.c $(LIBS)
	
lcluster.o: lcluster.h lcluster.cpp index.h bucket.o bucket.h bucket.c basics.o basics.h basics.c obj.h global.o global.h global.cpp MemoryManager.o MemoryManager.h MemoryManager.cpp 
	$(CXX) $(CFLAGS) -c global.o MemoryManager.o basics.o bucket.o lcluster.cpp $(LIBS)
	
TestLC: TestLC.cpp lcluster.o lcluster.h lcluster.cpp index.h bucket.o bucket.h bucket.c basics.o basics.h basics.c obj.h global.o global.h global.cpp 
	$(CXX) $(CFLAGS) -o TestLC lcluster.o global.o MemoryManager.o basics.o bucket.o  TestLC.cpp
		
.PHONY: clean
clean:
		-@rm *.o *.gcno *~ 2> /dev/null || true
		-@rm TestLC 2> /dev/null || true
		