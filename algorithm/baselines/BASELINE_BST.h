#ifndef BASELINE_BST
#define BASELINE_BST

#include <bits/stdc++.h>
using namespace std;
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

struct sgnode {
    int idx;     // the index of point, i.e. V[idx]
    double rad; // subtree's min dist to V[idx]
    sgnode(int _idx = 0) {
        idx = _idx;
        rad = 0;
    }
};

struct Node_t {
    vector <sgnode> pivot;
    vector <Node_t*> child;
    
};

extern Node_t *rt;

void build(Node_t *node, vector <int> &cand, int ary);
void buildBST(int ary);
void freeBST(Node_t *node);

void select(Node_t *node, vector <int> &cand, int ary);

void RangeQuery(location_t &loc, double radius, vector<int> &answers);
void _RangeQuery(Node_t *node, location_t &loc, double radius, vector<int> &answers);
void KnnQuery(location_t &loc, int kth, vector<int> &answers);
void _KnnQuery(Node_t *node, location_t &loc, int kth, priority_queue<pair<double, int> > &st);


void addCand(priority_queue<pair<double, int> > &q, int kth, pair<double, int> tmp);
bool outCand(priority_queue<pair<double, int> > &q, int kth, double dis);


#endif