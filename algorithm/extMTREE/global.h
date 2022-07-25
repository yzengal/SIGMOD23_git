#ifndef GLOBAL_H
#define GLOBAL_H

#include <bits/stdc++.h>
using namespace std;

#define DIM_V 128
#define MEM_PAGE_SIZE 4
#define MEM_CACHE_SIZE 256

// typedef unorderd_map<int,int> hii;
typedef long long LL;
typedef pair<int,int> pii;
typedef pair<double,double> pdd;
typedef pair<int,double> pid;
typedef pair<double,int> pdi;

struct location_t {
	unsigned char x[DIM_V];
};

extern int nV;
extern location_t* V;
extern const double speed;
extern const double EPS;
extern const double INF;
extern int tag;
extern double usedTime;
extern int usedMemory;
extern int pageSize;
extern int bucketSize;
extern int PIVOT_N;
extern double timeLimit;
extern long long distCnt;
extern size_t dataSize;
extern vector <double> RAN;
extern vector <int> KTH;
extern vector <int> QRY;

int dcmp(double x);  
double dist(location_t *V, int x, int y, int tag);
double dist(location_t& a, location_t& b, int tag);
double dist(location_t* a, location_t* b, int tag);   
double dist(location_t* a, location_t& b, int tag);   
double dist(location_t& a, location_t* b, int tag);   
void freeLocation();
void initLocation(string& fileName);
void initSIFT(const char *fileName);
void initR(const char *fileName);
void initK(const char *fileName);
void initQ(const char *fileName);
double genTestRadius();
int genTestKth();

#endif
