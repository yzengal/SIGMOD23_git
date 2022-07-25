#include <bits/stdc++.h>
using namespace std;
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

#include "MtreeAPI.h"

string srcFileName, desFileName;

void LinearScan_KnnQuery(location_t& loc, int kth, vector<int>& answers) {
	priority_queue<pair<double,int>, vector<pair<double,int> >, less<pair<double,int> > > Q;
	double dtmp;
	
	for (int i=0; i<nV; ++i) {
		dtmp = dist(loc, V[i], tag);
		Q.push(make_pair(dtmp, i));
		if (Q.size() > kth) {
			Q.pop();
		}
	}
	
	assert(Q.size() == kth);
	
	answers.clear();
	while (!Q.empty()) {
		answers.push_back(Q.top().second);
		Q.pop();
	}
	reverse(answers.begin(), answers.end());
}

void LinearScan_RangeQuery(location_t& loc, double radius, vector<int>& answers) {
	answers.clear();
	
	for (int i=0; i<nV; ++i) {
		double dtmp = dist(loc, V[i], tag);
		if (dtmp <= radius)
			answers.push_back(i);
	}
}

void TestRange(Index_t& index) {
	clock_t t0, t1;
	for (int i = 0; i < RAN.size(); ++ i) {
		vector<int> answers;
		double tim = 0, sumCnt = 0;
		
		for (int j = 0; j < QRY.size(); ++ j) {
			QRY[j] %= nV;
			answers.clear();
			distCnt = 0;
			
			t0 = clock();
			index.RangeQuery(QRY[j], RAN[i], answers);	// non-learning
			t1 = clock();
			
			tim += 1. * (t1 - t0) / CLOCKS_PER_SEC;
			sumCnt += distCnt;
			
			vector<int> answers_;
			LinearScan_RangeQuery(V[QRY[j]], RAN[i], answers_);
			sort(answers_.begin(), answers_.end());
			if (answers.size() != answers_.size()) {
				puts("Range query has bugs");
			}
		}
		
		printf("%.6lf %.6lf\n", 1. * sumCnt / QRY.size(), tim / QRY.size());
		fflush(stdout);
	}
}

void TestKnn(Index_t& index) {
	clock_t t0, t1;
	
	for (int i = 0; i < KTH.size(); ++ i) {
		vector<int> answers;
		double tim = 0, sumCnt = 0;
		
		for (int j = 0; j < QRY.size(); ++ j) {
			QRY[j] %= nV;
			answers.clear();
			distCnt = 0;
			
			t0 = clock();
			index.KnnQuery(QRY[j], KTH[i], answers); // non-learning
			t1 = clock();
			
			tim += 1. * (t1 - t0) / CLOCKS_PER_SEC;
			sumCnt += distCnt;
			
			vector<int> answers_;
			LinearScan_KnnQuery(V[QRY[j]], KTH[i], answers_);
			if (answers.size() == answers_.size()) {
				double rad_ = dist(V[QRY[j]], V[answers_[KTH[i]-1]], tag);
				double rad = 0;
				for (int i=0; i<answers.size(); ++i) {
					rad = max(rad, dist(V[QRY[j]], V[answers[i]], tag));
				}
				if (dcmp(rad-rad_) != 0) {
					puts("KNN query has bugs");
				}
			} else {
				puts("KNN query has bugs");
			}
		}
		printf("%.6lf %.6lf\n", 1. * sumCnt / QRY.size(), tim / QRY.size());
	}
}


void test() {
	double timeOfHSTp, timeOfHSTa;
	int nidOfHSTp, nidOfHSTa; 
	double memOfHSTp, memOfHSTa;
	Index_t index;
	
	clock_t t0, t1;
	puts("init done");
	fflush(stdout);
	
	t0 = clock();
	distCnt = 0;
	index.constructIndex(nV);
	t1 = clock();
	timeOfHSTp = 1. * (t1 - t0) / CLOCKS_PER_SEC;
	
	nidOfHSTp = index.countTree();
	memOfHSTp = index.memoryTree();
	memOfHSTp /= (1024*1024);
	
	printf("%.6lf %.6lf %lld %d\n", timeOfHSTp, memOfHSTp, distCnt, nidOfHSTp);
	fflush(stdout);
	
	TestRange(index);
	TestKnn(index);
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

