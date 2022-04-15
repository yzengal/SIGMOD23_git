#ifndef HST_STANDARD_QUERY_H
#define HST_STANDARD_QUERY_H

#include <bits/stdc++.h>
using namespace std;
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

#include "LiteHST_BASE.h"


struct HST_Node_t {
	int lev;			// the level of the node before compression
	int lhs, rhs;			// the right hand side of partition
	int child_n;		// the number of child
	HST_Node_t* far;	// the parent node
	HST_Node_t** child; // the child nodes
	
	HST_Node_t(int lev_, HST_Node_t* far_) {
		lev = lev_;
		far = far_;
		child_n = 0;
		child = NULL;
	}
	
	HST_Node_t(int lev_, HST_Node_t* far_, const int& child_n_) {
		lev = lev_;
		far = far_;
		child_n = child_n_;
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
	HST_Node_t **leavesPtr;
	int *leavesID;
	double* expks;
	double* sumks;	
	int HST_H, HST_nV;
	int HST_alpha;
	double HST_beta, HST_distortion;
	int leafIdx;
	
	~HST_Lite_t() {
		delete[] leavesPtr;
		delete[] leavesID;
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
		printf("lhs=%d, rhs=%d, |child|=%d\n", p->lhs, p->rhs, p->child_n);
	}

	void _dumpTree(int dep, HST_Node_t* rt) {
		if (rt != NULL) {
			_dumpNode(dep, rt);
			for (int i=0; i<rt->child_n; ++i)
				_dumpTree(dep+1, rt->child[i]);
		}
	}
	
	void compressHST(Node_t* rt_) {
		leafIdx = 0;
		rt = _compressHST(rt_, NULL);
	}
	
	HST_Node_t* _compressHST(Node_t* rt, HST_Node_t* HST_far=NULL) {
		if (rt == NULL) return NULL;
		
		HST_Node_t* HST_rt = new HST_Node_t(rt->lev, HST_far, (int)rt->child.size());
		HST_rt->lhs = leafIdx;
		for (int i=0; i<rt->child.size(); ++i) {
			HST_rt->child[i]  = _compressHST(rt->child[i], HST_rt);
		}
		if (HST_rt->child_n == 0) {
			leavesPtr[rt->nomIdx] = HST_rt;
			leavesID[leafIdx++] = rt->nomIdx;
		}
		HST_rt->rhs = leafIdx;
		
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
		
		leavesPtr = new HST_Node_t*[HST_nV];
		leavesID = new int[HST_nV];
		leafIdx = 0;
		HST_distortion = 96 * log(HST_nV) / log(alpha);
	}
	
	void RangeQuery(location_t& loc, double radius, vector<int>& answers) {
		_Embed_RangeQuery(loc, radius, answers);
	}
	
	void KnnQuery(location_t& loc, int kth, vector<int>& answers) {
		_Embed_KnnQuery(loc, kth, answers);
	}
	
	void RangeQuery(int queryLocID, double radius, vector<int>& answers) {
		_Embed_RangeQuery(queryLocID, radius, answers);
	}
	
	void KnnQuery(int queryLocID, int kth, vector<int>& answers) {
		_Embed_KnnQuery(queryLocID, kth, answers);
	}
	
	void _Embed_RangeQuery(location_t& loc, double radius, vector<int>& answers);
	void _Embed_1nnQuery(location_t& loc, vector<int>& answers);
	void _Embed_KnnQuery(location_t& loc, int kth, vector<int>& answers);
	void _Embed_RangeQuery(int queryLocID, double radius, vector<int>& answers);
	void _Embed_KnnQuery(int queryLocID, int kth, vector<int>& answers);
};

extern HST_Lite_t HSTlite;

double distCircle(location_t& o, double r, location_t& q);
bool intersects(location_t& o1, double r1, location_t& o2, double r2);
bool within(location_t& oq, double rq, location_t& p);


#endif
