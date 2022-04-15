#include "bits/stdc++.h"
#include "BASELINE_GNAT.h"
using namespace std;


Node_t *rt = NULL;


int mem;

void traverse() {
	queue <pair<Node_t*, int> > q;
	q.push(make_pair(rt, -1));
	while (!q.empty()) {
		pair<Node_t*, int> tmp = q.front();
		q.pop();
		Node_t* u = tmp.first;
		int fa = tmp.second;
		mem += sizeof(int) + u->pivot.size() * sizeof(sgnode);
		mem += sizeof(int) + u->child.size() * sizeof(Node_t*);
		for (int i = 0; i < u->child.size(); ++ i) {
			q.push(make_pair(u->child[i], u->pivot[i].idx));
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
	buildGNAT(5);
	t1 = clock();
	printf("%.4f\n", 1. * (t1 - t0) / CLOCKS_PER_SEC);
	mem = 0;
	traverse();
	printf("%d\n", mem);
	TestRange();
	TestKnn();
	freeGNAT(rt);
	freeLocation();
    return 0;
}

