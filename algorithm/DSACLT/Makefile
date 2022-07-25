
##
CC = gcc
CXX = g++
LIBS = 
CFLAGS = --std=c++11 -W -Wall -Wno-sign-compare -O3 -s -pipe -mmmx -msse -msse2 -msse3 # -mcmodel=medium
MEM = -D WATCH_MEM
DIM = 

all: TestDSA
global.o: global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.cpp $(LIBS)
	
MemoryManager.o: MemoryManager.h MemoryManager.cpp global.o global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.o MemoryManager.cpp $(LIBS)
	
basics.o: basics.h basics.c obj.h global.o global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.o basics.c $(LIBS)
	
bucket.o: bucket.h bucket.c basics.o basics.h basics.c obj.h global.o global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.o basics.o bucket.c $(LIBS)
	
DSACLT.o: DSACLT.h DSACLT.c index.h bucket.o bucket.h bucket.c basics.o basics.h basics.c obj.h global.o global.h global.cpp MemoryManager.o MemoryManager.h MemoryManager.cpp 
	$(CXX) $(CFLAGS) -c global.o MemoryManager.o basics.o bucket.o DSACLT.c $(LIBS)
	
TestDSA: TestDSACLT.cpp DSACLT.o DSACLT.h DSACLT.c index.h bucket.o bucket.h bucket.c basics.o basics.h basics.c obj.h global.o global.h global.cpp MemoryManager.o MemoryManager.h MemoryManager.cpp 
	$(CXX) $(CFLAGS) -o TestDSA DSACLT.o global.o MemoryManager.o basics.o bucket.o TestDSACLT.cpp $(LIBS)
		
.PHONY: clean
clean:
		-@rm *.o *.gcno *~ 2> /dev/null || true
		-@rm TestDSA 2> /dev/null || true
		