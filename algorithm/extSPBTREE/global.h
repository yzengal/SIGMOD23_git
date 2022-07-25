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
extern int dim;
extern double timeLimit;
extern long long distCnt;
extern int pageSize;
extern int bucketSize;
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

// inline double dist(location_t *V, int x, int y) { 
	// location_t& a = V[x];
	// location_t& b = V[y];
	// return sqrt((a.x[0] - b.x[0])*(a.x[0] - b.x[0]) + (a.x[1] - b.x[1])*(a.x[1] - b.x[1]));
// } 

// inline double dist(location_t &a, location_t &b) {
	// return sqrt((a.x[0] - b.x[0])*(a.x[0] - b.x[0]) + (a.x[1] - b.x[1])*(a.x[1] - b.x[1]));
// }

// inline
// double dist(location_t *V, int x, int y) {
	// if (x == y) return 0.0;

	// location_t& a = V[x];
	// location_t& b = V[y];
	// double ret = 0.0;

	// for (int i=0; i<DIM_V; ++i) {
		// ret += (a.x[i]-b.x[i]) * (a.x[i]-b.x[i]);
	// }

	// return sqrt(ret);
// }

// inline
// double dist(location_t &a, location_t &b) {
	// double ret = 0.0;

	// for (int i=0; i<DIM_V; ++i) {
		// ret += (a.x[i]-b.x[i])*(a.x[i]-b.x[i]);
	// }

	// return sqrt(ret);
// }

#endif
