#include <bits/stdc++.h>
using namespace std;
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

#include "DSACLT.h"

string srcFileName, desFileName;

void TestRange(Index& LC) {
	clock_t t0, t1;
	for (int i = 0; i < RAN.size(); ++ i) {
		vector<int> answers;
		double tim = 0, sumCnt = 0, pageCnt = 0;
		
		for (int j = 0; j < QRY.size(); ++ j) {
			QRY[j] %= nV;
			answers.clear();
			distCnt = 0;
			
			t0 = clock();
			int ansSize = search(LC, QRY[j], RAN[i], 0);
			t1 = clock();
			
			tim += 1. * (t1 - t0) / CLOCKS_PER_SEC;
			sumCnt += distCnt;
			pageCnt += pageAccess;
		}
		
		printf("%.6lf %.6lf %.6lf\n", 1. * sumCnt / QRY.size(), tim / QRY.size(), pageCnt / QRY.size());
		fflush(stdout);
	}
}

void TestKnn(Index& LC) {
	clock_t t0, t1;
	
	for (int i = 0; i < KTH.size(); ++ i) {
		vector<int> answers;
		double tim = 0, sumCnt = 0, pageCnt = 0;
		
		for (int j = 0; j < QRY.size(); ++ j) {
			QRY[j] %= nV;
			answers.clear();
			distCnt = 0;
			
			t0 = clock();
			double rad = searchNN(LC, QRY[j], KTH[i], 0);
			t1 = clock();
			
			tim += 1. * (t1 - t0) / CLOCKS_PER_SEC;
			sumCnt += distCnt;
			pageCnt += pageAccess;
		}
		printf("%.6lf %.6lf %.6lf\n", 1. * sumCnt / QRY.size(), tim / QRY.size(), pageCnt / QRY.size());
	}
}


void test() {
	double timeOfHSTp, timeOfHSTa;
	int nidOfHSTp, nidOfHSTa; 
	double memOfHSTp, memOfHSTa;
	
	clock_t t0, t1;
	puts("init done");
	fflush(stdout);
	
	t0 = clock();
	distCnt = 0;
	char fileName[1024];
	strcpy(fileName, srcFileName.c_str());
	Index DSACLT = build(fileName, nV, pageSize);
	t1 = clock();
	timeOfHSTp = 1. * (t1 - t0) / CLOCKS_PER_SEC;
	
	nidOfHSTp = countTree(DSACLT);
	memOfHSTp = memoryTree(DSACLT);
	memOfHSTp /= (1024*1024);
	
	printf("%.6lf %.6lf %lld %d\n", timeOfHSTp, memOfHSTp, distCnt, nidOfHSTp);
	fflush(stdout);
	
	TestRange(DSACLT);
	TestKnn(DSACLT);
	
	freeIndex (DSACLT, 0);
}

int main(int argc, char **argv) {
    srcFileName = "dataset_dutch";
	if (argc > 1) {
		srcFileName = string(argv[1]);
	}
	if (argc > 2) {
		desFileName = string(argv[2]);
		freopen(desFileName.c_str(), "w", stdout);
	}
	
	initLocation(srcFileName);
	test();
	freeLocation();
    
	return 0;
}

