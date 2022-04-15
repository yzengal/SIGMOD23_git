#include <bits/stdc++.h>
using namespace std;
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

#include "LiteHST_BASE.h"
#include "LiteHST_DSU_Query.h"

string srcFileName, desFileName;

void TestRange() {
	clock_t t0, t1;
	for (int i = 0; i < RAN.size(); ++ i) {
		vector<int> answers;
		double tim = 0, sumCnt = 0;
		
		for (int j = 0; j < QRY.size(); ++ j) {
			answers.clear();
			distCnt = 0;
			
			t0 = clock();
			HSTlite._DSU_RangeQuery_v1f(V[QRY[j]], RAN[i], answers);	// non-learning
			t1 = clock();
			
			tim += 1. * (t1 - t0) / CLOCKS_PER_SEC;
			sumCnt += distCnt;
		}
		
		printf("%.6lf %.6lf\n", 1. * sumCnt / QRY.size(), tim / QRY.size());
		fflush(stdout);
	}
}

void TestKnn() {
	clock_t t0, t1;
	
	for (int i = 0; i < KTH.size(); ++ i) {
		vector<int> answers;
		double tim = 0, sumCnt = 0;
		
		for (int j = 0; j < QRY.size(); ++ j) {
			answers.clear();
			distCnt = 0;
			
			t0 = clock();
			HSTlite._DSU_KnnQuery_v0g(V[QRY[j]], KTH[i], answers); // non-learning
			t1 = clock();
			
			tim += 1. * (t1 - t0) / CLOCKS_PER_SEC;
			sumCnt += distCnt;
		}
		printf("%.6lf %.6lf\n", 1. * sumCnt / QRY.size(), tim / QRY.size());
	}
}

void test() {
	double timeOfHSTp, timeOfHSTa;
	int nidOfHSTp, nidOfHSTa; 
	double memOfHSTp, memOfHSTa;
	
	initVariable();
	initMemory_HST(nV);
	
	clock_t t0, t1;
	puts("init done");
	fflush(stdout);
	
	t0 = clock();
	distCnt = 0;
	constructHST(srcFileName);
	t1 = clock();
	timeOfHSTp = 1. * (t1 - t0) / CLOCKS_PER_SEC;
	
	nidOfHSTp = countTree(rt);
	memOfHSTp = memoryTree(rt);
	memOfHSTp /= (1024*1024);
	
	printf("%.6lf %.6lf %lld %d\n", timeOfHSTp, memOfHSTp, distCnt, nidOfHSTp);
	fflush(stdout);
	
	HSTlite.init(H, nV, distortion);
	t0 = clock();
	HSTlite.compressHST(rt);
	t1 = clock();
	timeOfHSTa = timeOfHSTp + 1. * (t1 - t0) / CLOCKS_PER_SEC;
	nidOfHSTa = nV;
	memOfHSTa = HSTlite.memoryCost();
	memOfHSTa /= (1024*1024);
	printf("%.6lf %.6lf %lld %d\n", timeOfHSTa, memOfHSTa, distCnt, nidOfHSTa);
	fflush(stdout);
	
	TestRange();
	TestKnn();
}

int main(int argc, char **argv) {
    srcFileName = "data_0.txt";
	if (argc > 1) {
		srcFileName = string(argv[1]);
	}
	if (argc > 2) {
		desFileName = string(argv[2]);
		freopen(desFileName.c_str(), "w", stdout);
	}
	
	initLocation(srcFileName);
	test();
	freeMemory_HST();
	freeLocation();
    
	return 0;
}

