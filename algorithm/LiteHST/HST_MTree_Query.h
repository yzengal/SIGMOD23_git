#ifndef HST_MTREE_QUERY_H
#define HST_MTREE_QUERY_H

#include <bits/stdc++.h>
using namespace std;
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

#include "LiteHST_BASE.h"

struct HST_Node_t {
	int nid;			// the ID of the node
	int lev;			// the level of the node before compression
	int nomIdx; 	// the id of norminator point
	int child_n;		// the number of child
	double dis2par;		// te distance of V[nomIdx] to V[far->nomIdx]
	HST_Node_t* far;	// the parent node
	HST_Node_t** child; // the child nodes
	
	HST_Node_t(int lev_, HST_Node_t* far_) {
		lev = lev_;
		far = far_;
		child_n = 0;
		nomIdx = 0;
		dis2par = 0.0;
		child = NULL;
	}
	
	HST_Node_t(int lev_, HST_Node_t* far_, const int& child_n_) {
		lev = lev_;
		far = far_;
		child_n = child_n_;
		nomIdx = 0;
		dis2par = 0.0;
		if (child_n_ > 0) {
			child = new HST_Node_t*[child_n_];
			memset(child, 0, sizeof(HST_Node_t*)*child_n_);
		} else {
			child = NULL;
		}
	}
};

struct HST_Lite_t {
	HST_Node_t *rt;
	double* expks;
	double* sumks;	
	int HST_H, HST_nV;
	int HST_alpha;
	int HST_node_n;
	double HST_beta, HST_distortion;

	~HST_Lite_t() {
		delete[] expks;
		delete[] sumks;
		if (rt != NULL)
			freeHST(rt);
	}	
	
	void freeHST(HST_Node_t* &rt) {
		if (rt == NULL) return ;
	
		HST_Node_t* chi;
		
		for (int i=0; i<rt->child_n; ++i) {
			chi = rt->child[i];
			freeHST(chi);
		}
		
		delete rt;
		rt = NULL;
	}
	
	double radAtLevel(int lev) {
		if (lev >= H+1) return beta;
		return expks[H-lev+1];
	}
	
	double distAtLevel(int lev) {
		if (lev >= H+1) return 0.0;
		double tmp = sumks[H-lev+1];
		return tmp + tmp;

	}
	
	void dumpTree() {
		_dumpTree(0, rt);
	}
	
	void _dumpNode(int dep, HST_Node_t* p) {
		for (int i=0; i<dep; ++i)
			putchar(' ');
		printf("nomIdx=%d, dis2par=%.4lf, |child|=%d\n", p->nomIdx, p->dis2par, p->child_n);
	}

	void _dumpTree(int dep, HST_Node_t* rt) {
		if (rt != NULL) {
			_dumpNode(dep, rt);
			for (int i=0; i<rt->child_n; ++i)
				_dumpTree(dep+1, rt->child[i]);
		}
	}
	
	void compressHST(Node_t* rt_) {
		HST_node_n = 0;
		rt = _compressHST(rt_, NULL);
	}
	
	HST_Node_t* _compressHST(Node_t* rt, HST_Node_t* HST_far=NULL) {
		if (rt == NULL) return NULL;
		
		HST_Node_t* HST_rt = new HST_Node_t(rt->lev, HST_far, (int)rt->child.size());
		HST_rt->nid = HST_node_n++;
		HST_rt->nomIdx = rt->nomIdx;
		HST_rt->dis2par = (HST_far==NULL||HST_rt->nomIdx==HST_far->nomIdx) ? 0.0 : dist(V[HST_rt->nomIdx], V[HST_far->nomIdx], tag);
		for (int i=0; i<HST_rt->child_n; ++i) {
			HST_rt->child[i] = _compressHST(rt->child[i], HST_rt);
		}
		
		return HST_rt;
	}
	
	void init(int H, int nV, double distortion) {
		rt = NULL;
		HST_H = H, HST_alpha = alpha, HST_beta = beta, HST_nV = nV;
		expks = new double[H+3];
		sumks = new double[H+3];

		expks[0] = beta;
		for (int i=1; i<=H+2; ++i)
			expks[i] = expks[i-1] * alpha;
		sumks[0] = 0.0;
		for (int i=1; i<=H+2; ++i)
			sumks[i] = sumks[i-1] + expks[i];		
		
		HST_distortion = distortion;
	}
	
	void RangeQuery(location_t& loc, double radius, vector<int>& answers) {
		_MTree_RangeQuery(loc, radius, answers);
	}
	
	void KnnQuery(location_t& loc, int kth, vector<int>& answers) {
		_MTree_KnnQuery(loc, kth, answers);
	}
	
	void _MTree_RangeQuery(location_t& loc, double radius, vector<int>& answers);
	void _MTree_KnnQuery(location_t& loc, int kth, vector<int>& answers);
};

extern HST_Lite_t HSTlite;

double distCircle(location_t& o, double r, location_t& q);
bool intersects(location_t& o1, double r1, location_t& o2, double r2);
bool within(location_t& oq, double rq, location_t& p);

#endif
