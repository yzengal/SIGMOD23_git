CC = gcc
CXX = g++
LIBS = 
CFLAGS = --std=c++11 -W -Wall -Wno-sign-compare -O3 -s -pipe -mmmx -msse -msse2 -msse3 # -mcmodel=medium
MEM = -D WATCH_MEM
DIM = 

all: TestBST TestBKT TestMVPT TestGNAT TestSPBT
monitor.o: monitor.h monitor.cpp
	$(CXX) $(CFLAGS) -c monitor.cpp $(LIBS) $(MEM)
	
global.o: global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.cpp $(LIBS) $(MEM) $(DIM)

BASELINE_MVPT.o: BASELINE_MVPT.h BASELINE_MVPT.cpp global.h global.cpp global.o monitor.h monitor.cpp monitor.o 
	$(CXX) $(CFLAGS) -c global.o monitor.o BASELINE_MVPT.cpp $(LIBS) $(MEM) $(DIM)

TestMVPT: TestMVPT.cpp global.h global.cpp global.o BASELINE_MVPT.h BASELINE_MVPT.cpp BASELINE_MVPT.o monitor.h monitor.cpp monitor.o 
	$(CXX) $(CFLAGS) -o TestMVPT global.o monitor.o BASELINE_MVPT.o TestMVPT.cpp $(LIBS) $(MEM) $(DIM)
	
BASELINE_GNAT.o: BASELINE_GNAT.h BASELINE_GNAT.cpp global.h global.cpp global.o monitor.h monitor.cpp monitor.o 
	$(CXX) $(CFLAGS) -c global.o monitor.o BASELINE_GNAT.cpp $(LIBS) $(MEM) $(DIM)

TestGNAT: TestGNAT.cpp global.h global.cpp global.o BASELINE_GNAT.h BASELINE_GNAT.cpp BASELINE_GNAT.o monitor.h monitor.cpp monitor.o 
	$(CXX) $(CFLAGS) -o TestGNAT global.o monitor.o BASELINE_GNAT.o TestGNAT.cpp $(LIBS) $(MEM) $(DIM)

BASELINE_BST.o: BASELINE_BST.h BASELINE_BST.cpp global.h global.cpp global.o monitor.h monitor.cpp monitor.o 
	$(CXX) $(CFLAGS) -c global.o monitor.o BASELINE_BST.cpp $(LIBS) $(MEM) $(DIM)

TestBST: TestBST.cpp global.h global.cpp global.o BASELINE_BST.h BASELINE_BST.cpp BASELINE_BST.o monitor.h monitor.cpp monitor.o 
	$(CXX) $(CFLAGS) -o TestBST global.o monitor.o BASELINE_BST.o TestBST.cpp $(LIBS) $(MEM) $(DIM)

BASELINE_BKT.o: BASELINE_BKT.h BASELINE_BKT.cpp global.h global.cpp global.o monitor.h monitor.cpp monitor.o 
	$(CXX) $(CFLAGS) -c global.o monitor.o BASELINE_BKT.cpp $(LIBS) $(MEM) $(DIM)

TestBKT: TestBKT.cpp global.h global.cpp global.o BASELINE_BKT.h BASELINE_BKT.cpp BASELINE_BKT.o monitor.h monitor.cpp monitor.o 
	$(CXX) $(CFLAGS) -o TestBKT global.o monitor.o BASELINE_BKT.o TestBKT.cpp $(LIBS) $(MEM) $(DIM)

TestSPBT: TestSPBtreePGM.cpp SPBtree_PGM.h global.h global.cpp global.o monitor.h monitor.cpp monitor.o 
	$(CXX) -march=haswell -std=c++17 -W -Wall -Wno-sign-compare -O3 -s -pipe -mmmx -msse -msse2 -msse3 -o TestSPBT global.o monitor.o TestSPBtreePGM.cpp -I../include $(LIBS) $(MEM) $(DIM)

.PHONY: clean
clean:
		-@rm *.o *.gcno *~ 2> /dev/null || true
		-@rm TestBST TestBKT TestMVPT TestGNAT TestSPBT 2> /dev/null || true
		