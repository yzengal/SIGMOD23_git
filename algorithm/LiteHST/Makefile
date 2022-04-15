
CC = gcc
CXX = g++
LIBS =  
CFLAGS = --std=c++11 -W -Wall -Wno-sign-compare -O3 -s -pipe -mmmx -msse -msse2 -msse3 # -mcmodel=medium
MEM = -D WATCH_MEM
DIM = 

all: FastMap Mtree LiteHST LiteHSTO0
monitor.o: monitor.h monitor.cpp
	$(CXX) $(CFLAGS) -c monitor.cpp $(LIBS) $(MEM)
	
global.o: global.h global.cpp 
	$(CXX) $(CFLAGS) -c global.cpp $(LIBS) $(MEM) $(DIM)

LiteHST_BASE.o: LiteHST_BASE.h LiteHST_BASE.cpp global.h global.cpp global.o monitor.h monitor.cpp monitor.o
	$(CXX) $(CFLAGS) -c global.o monitor.o LiteHST_BASE.cpp $(LIBS) $(MEM) $(DIM)	
	
LiteHST_DSU_Query.o: LiteHST_DSU_Query.h LiteHST_DSU_Query.cpp global.h global.cpp global.o LiteHST_BASE.h LiteHST_BASE.cpp LiteHST_BASE.o monitor.h monitor.cpp monitor.o 
	$(CXX) $(CFLAGS) -c global.o monitor.o LiteHST_BASE.o LiteHST_DSU_Query.cpp $(LIBS) $(MEM) $(DIM)
		
LiteHSTO0: TestLiteHST.cpp LiteHST_DSU_Query.h LiteHST_DSU_Query.cpp LiteHST_DSU_Query.o global.h global.cpp global.o LiteHST_BASE.h LiteHST_BASE.cpp LiteHST_BASE.o monitor.h monitor.cpp monitor.o  
	$(CXX) $(CFLAGS) -o LiteHSTO0 global.o monitor.o LiteHST_BASE.o LiteHST_DSU_Query.o TestLiteHST.cpp $(LIBS) $(MEM) $(DIM)

LiteHST: TrainLiteHST.cpp LiteHST_DSU_Query.h LiteHST_DSU_Query.cpp LiteHST_DSU_Query.o global.h global.cpp global.o LiteHST_BASE.h LiteHST_BASE.cpp LiteHST_BASE.o monitor.h monitor.cpp monitor.o  
	$(CXX) $(CFLAGS) -o LiteHST global.o monitor.o LiteHST_BASE.o LiteHST_DSU_Query.o TestLiteHST.cpp $(LIBS) $(MEM) $(DIM)

HST_Standard_Query.o: HST_Standard_Query.h HST_Standard_Query.cpp global.h global.cpp global.o LiteHST_BASE.h LiteHST_BASE.cpp LiteHST_BASE.o monitor.h monitor.cpp monitor.o 
	$(CXX) $(CFLAGS) -c global.o monitor.o LiteHST_BASE.o HST_Standard_Query.cpp $(LIBS) $(MEM) $(DIM)
		
FastMap: TestEmbedHST.cpp HST_Standard_Query.h HST_Standard_Query.cpp HST_Standard_Query.o global.h global.cpp global.o LiteHST_BASE.h LiteHST_BASE.cpp LiteHST_BASE.o monitor.h monitor.cpp monitor.o  
	$(CXX) $(CFLAGS) -o FastMap global.o monitor.o LiteHST_BASE.o HST_Standard_Query.o TestEmbedHST.cpp $(LIBS) $(MEM) $(DIM)

HST_MTree_Query.o: HST_MTree_Query.h HST_MTree_Query.cpp global.h global.cpp global.o LiteHST_BASE.h LiteHST_BASE.cpp LiteHST_BASE.o monitor.h monitor.cpp monitor.o 
	$(CXX) $(CFLAGS) -c global.o monitor.o LiteHST_BASE.o HST_MTree_Query.cpp $(LIBS) $(MEM) $(DIM)
		
Mtree: TestMTreeHST.cpp HST_MTree_Query.h HST_MTree_Query.cpp HST_MTree_Query.o global.h global.cpp global.o LiteHST_BASE.h LiteHST_BASE.cpp LiteHST_BASE.o monitor.h monitor.cpp monitor.o  
	$(CXX) $(CFLAGS) -o Mtree global.o monitor.o LiteHST_BASE.o HST_MTree_Query.o TestMTreeHST.cpp $(LIBS) $(MEM) $(DIM)
	
.PHONY: clean
clean:
		-@rm *.o *.gcno *~ 2> /dev/null || true
		-@rm FastMap Mtree LiteHSTO0 LiteHST 2> /dev/null || true
		