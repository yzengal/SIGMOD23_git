#ifndef GLOBAL_H
#define GLOBAL_H

#include <bits/stdc++.h>
using namespace std;

#define DIM_V 128

// typedef unorderd_map<int,int> hii;
typedef long long LL;
typedef pair<int,int> pii;
typedef pair<double,double> pdd;
typedef pair<int,double> pid;
typedef pair<double,int> pdi;

struct location_t {
	double x[DIM_V];
	char s[DIM_V];
};

extern int nV, DIM_K;
extern location_t* V;
extern const double speed;
extern const double EPS;
extern const double INF;
extern int tag;
extern double usedTime;
extern int usedMemory;
extern double timeLimit;
extern long long distCnt;
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
void initDutch(const char *fileName);
void initEnglish(const char *fileName);
void initSyn(const char *fileName);
void initColor(const char *fileName);
void initR(const char *fileName);
void initK(const char *fileName);
void initQ(const char *fileName);

#endif
