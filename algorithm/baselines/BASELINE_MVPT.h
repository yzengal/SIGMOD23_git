#ifndef BASELINE_MVPT
#define BASELINE_MVPT

#include <bits/stdc++.h>
using namespace std;
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

struct Node_mvpt {
    int idx;
    bool isleaf;
    vector <int> bucket;
    vector <double> lower;
    vector <Node_mvpt*> child;

    Node_mvpt() {
        idx = -1;
        isleaf = false;
    }
};

extern Node_mvpt *rt;

void build(Node_mvpt *node, vector <int> &cand, int ary, int bsize);
void buildMVPT(int ary, int bsize);
void freeMVPT(Node_mvpt *node);

void RangeQuery(location_t &loc, double radius, vector<int> &answers);
void _RangeQuery(Node_mvpt *node, location_t &loc, double radius, vector<int> &answers);
void KnnQuery(location_t &loc, int kth, vector<int> &answers);
void _KnnQuery(Node_mvpt *node, location_t &loc, int kth, priority_queue<pair<double, int> > &st);


void addCand(priority_queue<pair<double, int> > &q, int kth, pair<double, int> tmp);
bool outCand(priority_queue<pair<double, int> > &q, int kth, double dis);

#endif