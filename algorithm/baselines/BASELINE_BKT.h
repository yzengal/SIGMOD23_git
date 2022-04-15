#ifndef BASELINE_MVPT
#define BASELINE_MVPT

#include <bits/stdc++.h>
using namespace std;
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

struct Node_bkt {
    int idx;
    bool isleaf;
    int step;
    vector <int> bucket;
    vector <double> lower;
    vector <Node_bkt*> child;

    Node_bkt(int _step) {
        step = _step;
        isleaf = true;
    }
};

extern Node_bkt *rt;

void addbkt(Node_bkt *node, int cand, int step, int bsize);
void buildBKT(int step, int bsize);
void freeBKT(Node_bkt *node);

void RangeQuery(location_t &loc, double radius, vector<int> &answers);
void _RangeQuery(Node_bkt *node, location_t &loc, double radius, vector<int> &answers);
void KnnQuery(location_t &loc, int kth, vector<int> &answers);
void _KnnQuery(Node_bkt *node, location_t &loc, int kth, priority_queue<pair<double, int> > &st);


void addCand(priority_queue<pair<double, int> > &q, int kth, pair<double, int> tmp);
bool outCand(priority_queue<pair<double, int> > &q, int kth, double dis);




#endif