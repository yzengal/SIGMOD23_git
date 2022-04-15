#include "bits/stdc++.h"
#include "BASELINE_MVPT.h"
using namespace std;


Node_mvpt *rt = NULL;

int mem;

void traverse() {
	queue <pair<Node_mvpt*, int> > q;
	q.push(make_pair(rt, -1));
	while (!q.empty()) {
		pair<Node_mvpt*, int> tmp = q.front();
		q.pop();
		Node_mvpt* u = tmp.first;
		int fa = tmp.second;
		mem += sizeof(int) + sizeof(bool);
		mem += sizeof(int) + u->bucket.size() * sizeof(int);
		mem += sizeof(int) + u->lower.size() * sizeof(double);
		mem += sizeof(int) + u->child.size() * sizeof(Node_mvpt*);
		if (u->isleaf) {
			;
		} else {
			for (int i = 0; i < u->child.size(); ++ i) {
				q.push(make_pair(u->child[i], u->idx));
			}
		}
	}
}

void TestRange() {
	for (int i = 0; i < RAN.size(); ++ i) {
		distCnt = 0;
		vector<int> answers;
		double tim = 0;
		for (int j = 0; j < QRY.size(); ++ j) {
			answers.clear();
			clock_t t0, t1;
			t0 = clock();
			RangeQuery(V[QRY[j]], RAN[i], answers);
			t1 = clock();
			tim += 1. * (t1 - t0) / CLOCKS_PER_SEC;
		}
		printf("%.6f %.6f\n", 1. * distCnt / QRY.size(), tim / QRY.size());
	}
}

void TestKnn() {
	for (int i = 0; i < KTH.size(); ++ i) {
		distCnt = 0;
		vector<int> answers;
		double tim = 0;
		for (int j = 0; j < QRY.size(); ++ j) {
			answers.clear();
			clock_t t0, t1;
			t0 = clock();
			KnnQuery(V[QRY[j]], KTH[i], answers);
			t1 = clock();
			tim += 1. * (t1 - t0) / CLOCKS_PER_SEC;
		}
		printf("%.6f %.6f\n", 1. * distCnt / QRY.size(), tim / QRY.size());
	}
}

int main(int argc, char **argv) {
    string srcFileName = "data_0.txt";
	if (argc > 1) {
		srcFileName = string(argv[1]);
	}
	clock_t t0, t1;
	initLocation(srcFileName);
	puts("init done");
	t0 = clock();
	buildMVPT(5,15);
	t1 = clock();
	printf("%.4f\n", 1. * (t1 - t0) / CLOCKS_PER_SEC);
	mem = 0;
	traverse();
	printf("%d\n", mem);
	TestRange();
	TestKnn();
	freeMVPT(rt);
	freeLocation();
    return 0;
}

