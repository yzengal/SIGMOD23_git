
#ifndef LiteHST_DSU_QUERY_H
#define LiteHST_DSU_QUERY_H

#include <bits/stdc++.h>
using namespace std;
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

#ifdef LEARN
#include "LiteHST_Learn.h"
#else
#include "LiteHST_BASE.h"
#endif

struct DSU_Row_t {
	int lhs, nomIdx;
	int m;
	int* Lev;
	int* Rhs;
	double* Distor;
	double dis2par;
	
	DSU_Row_t() {
		m = 0;
	}
	
	DSU_Row_t(int m_) {
		m = m_;
		if (m > 0) {
			Rhs = new int[m];
			Lev = new int[m];
			Distor = new double[m];
		} else {
			Rhs = NULL;
			Lev = NULL;
			Distor = NULL;
		}
	}
	
	~DSU_Row_t() {
		if (m > 0) {
			delete[] Rhs;
			delete[] Lev;
			delete[] Distor;
		}
	}
	
	void init(int m_) {
		assert(m == 0);
		m = m_;
		Rhs = new int[m];
		Lev = new int[m];
		Distor = new double[m];
	}
	
	void print() const {
		printf("nomIdx = %d, dis2par = %.4lf\n", nomIdx, dis2par);
		printf("lhs = %d, m = %d, rhs in [", lhs, m);
		for (int i=0; i<m; ++i) {
			if (i > 0) putchar(',');
			printf("%d", Rhs[i]);
			
		}
		printf("], lev in [");
		for (int i=0; i<m; ++i) {
			if (i > 0) putchar(',');
			printf("%d", Lev[i]);
			
		}
		printf("], Distor in [");
		for (int i=0; i<m; ++i) {
			if (i > 0) putchar(',');
			printf("%.5lf", Distor[i]);
			
		}
		printf("]\n");
	}
	
	double memoryCost() {
		return sizeof(int)*(3+m*2) + sizeof(double)*(1+m);
	}
};

struct HST_Lite_t {
	DSU_Row_t *Leaves;
	int *LeavesID;
	double* expks;
	double* sumks;	
	int HST_H, HST_nV;
	int HST_alpha;
	int HST_node_n;
	double HST_beta, HST_distortion;
	int leafIdx;
	int _queryLocPos;
	vector<bool> visit;
	vector<bool> visit2;
	double dk;

	~HST_Lite_t() {
		delete[] expks;
		delete[] sumks;
		delete[] Leaves;
		delete[] LeavesID;
	}
	
	void init(int H, int nV, double distortion) {
		HST_H = H, HST_alpha = alpha, HST_beta = beta, HST_nV = nV;
		expks = new double[H+4];
		sumks = new double[H+4];

		expks[0] = HST_beta;
		for (int i=1; i<=H+3; ++i)
			expks[i] = expks[i-1] * HST_alpha;
		sumks[0] = 0.0;
		for (int i=1; i<=H+3; ++i)
			sumks[i] = sumks[i-1] + expks[i];		
		
		HST_distortion = distortion;
		Leaves = new DSU_Row_t[HST_nV];
		LeavesID = new int[HST_nV];
		
		visit.resize(HST_nV);
		visit2.resize(HST_nV);
	}
	
	int invertPos(int rid) {
		return Leaves[rid].lhs;
	}
	
	double radAtLevel(int lev) {
		if (lev >= H+1) return 0.0;
		return expks[H-lev+1];
	}
	
	double distAtLevel(int lev) {
		if (lev >= H+1) return 0.0;
		double tmp = sumks[H-lev+1];
		return tmp + tmp;

	}
	
	void dumpTree() {
		for (int i=0; i<HST_nV; ++i) {
			printf("%d: Row ID = %d\n", i, LeavesID[i]);
			Leaves[LeavesID[i]].print();
			printf("\n\n");
		}
	}
	
	double memoryCost() {
		double ret = 0.0;
		
		for (int i=0; i<HST_nV; ++i) {
			ret += Leaves[i].memoryCost();
		}
		
		return ret;
	}
	
	void compressHST(Node_t* rt) {
		leafIdx = 0;
		_compressHST(rt, 0);
		
		// add information for root
		int rid = rt->nomIdx;
		Leaves[rid].nomIdx = rid;	// root always links to itself, like a disjoint set union (DSU)
		Leaves[rid].dis2par = 0.0;
	}
	
	void _compressHST(Node_t* rt, int m) {
		int child_n = rt->child.size();
		int rid = rt->nomIdx;
		
		if (child_n == 0) {
			Leaves[rid].init(m+1);
			Leaves[rid].lhs = leafIdx;
			
			LeavesID[leafIdx++] = rid;
			
			Leaves[rid].Rhs[m] = leafIdx;
			Leaves[rid].Lev[m] = rt->lev;
			Leaves[rid].Distor[m] = rt->distortion;
			return ;
		}
		
		for (int i=0; i<child_n; ++i) {
			if (rt->child[i]->nomIdx == rid) {
				_compressHST(rt->child[i], m+1);
			} else {
				_compressHST(rt->child[i], 0);
				int rid_ = rt->child[i]->nomIdx;
				Leaves[rid_].nomIdx = rid;
				Leaves[rid_].dis2par = rt->child[i]->dis2cp;
			}
		}
		
		Leaves[rid].Rhs[m] = leafIdx;
		Leaves[rid].Lev[m] = rt->lev;
		Leaves[rid].Distor[m] = rt->distortion;
	}
	
	int farIdx(int u) {
		if (u == LeavesID[0]) return -1;
		
		DSU_Row_t* ru = &Leaves[u];
		DSU_Row_t* rfu = &Leaves[ru->nomIdx];
		int idx = lower_bound(rfu->Rhs, rfu->Rhs+rfu->m, ru->lhs, greater<int>()) - rfu->Rhs;
		return idx - 1;
	}
	
	void setFar(int ridCur, int idxCur, int& ridNxt, int& idxNxt) {
		if (idxCur == 0) {
			if (ridCur == LeavesID[0]) {
				ridNxt = idxNxt = -1;
			} else {
				ridNxt = Leaves[ridCur].nomIdx;
				idxNxt = farIdx(ridCur);
			}
		} else {
			ridNxt = ridCur;
			idxNxt = idxCur - 1;
		}
	}
	
	int levelOfLCA(int u, int v) {
		if (u == v) return HST_H+1;
		
		DSU_Row_t* pu = &Leaves[u];
		DSU_Row_t* pv = &Leaves[v];
	
		while (pu != pv) {
			if (pu->Lev[0] > pv->Lev[0]) {
				pu = &Leaves[pu->nomIdx];
			} else {
				pv = &Leaves[pv->nomIdx];
			}
		}
		
		int idx = lower_bound(pu->Rhs, pu->Rhs+pu->m, max(Leaves[u].lhs, Leaves[v].lhs), greater<int>()) - pu->Rhs;
		return pu->Lev[idx-1];
	}
	
	double _Distor_LB_Fast(int u, int v) {
		if (u == v) return 0.0;
		
		DSU_Row_t* pu = &Leaves[u];
		DSU_Row_t* pv = &Leaves[v];
	
		while (pu != pv) {
			if (pu->Lev[0] > pv->Lev[0]) {
				pu = &Leaves[pu->nomIdx];
			} else {
				pv = &Leaves[pv->nomIdx];
			}
		}
		
		int idx = lower_bound(pu->Rhs, pu->Rhs+pu->m, max(Leaves[u].lhs, Leaves[v].lhs), greater<int>()) - pu->Rhs;
		return distAtLevel(pu->Lev[idx-1])/pu->Distor[idx-1];
	}	
	
	int _Distor_LB_Lev_Fast(int u, int v) {
		if (u == v) return HST_H+1;
		
		DSU_Row_t* pu = &Leaves[u];
		DSU_Row_t* pv = &Leaves[v];
	
		while (pu != pv) {
			if (pu->Lev[0] > pv->Lev[0]) {
				pu = &Leaves[pu->nomIdx];
			} else {
				pv = &Leaves[pv->nomIdx];
			}
		}
		
		int idx = lower_bound(pu->Rhs, pu->Rhs+pu->m, max(Leaves[u].lhs, Leaves[v].lhs), greater<int>()) - pu->Rhs;
		return pu->Lev[idx-1];
	}	
	
	bool overlapInterval(double l1, double r1, double l2, double r2) {
		return max(l1, l2) <= min(r1, r2);
	}
	
	void RangeQuery(location_t& loc, double radius, vector<int>& answers) {
		_DSU_RangeQuery(loc, radius, answers);
	}
	
	void _DSU_RangeQuery(location_t& loc, double radius, vector<int>& answers) {
		_DSU_RangeQuery_v1f(loc, radius, answers);
	}
	
	void _DSU_RangeQuery_v1f(location_t& loc, double radius, vector<int>& answers);
	
	
	void KnnQuery(location_t& loc, int kth, vector<int>& answers) {
		_DSU_KnnQuery(loc, kth, answers);
	}
	
	void KnnQuery(int qid, int kth, vector<int>& answers) {
		_DSU_KnnQuery(V[qid], kth, answers);
	}
	
	// This maintains the fastest query processing algorithm
	void _DSU_KnnQuery(location_t& loc, int kth, vector<int>& answers) {
		_DSU_KnnQuery_v0f(loc, kth, answers);
	}
	
	void _DSU_KnnQuery_v0f(location_t& loc, int kth, vector<int>& answers);
	void _DSU_KnnQuery_v0g(location_t& loc, int kth, vector<int>& answers);
	
	double _DSU_initDk(location_t& loc, int kth) {
		return _DSU_initDk_ScanOnce(loc, kth);
	}
	
	double _DSU_initDk_LinearScan(location_t& loc, int kth);
	double _DSU_initDk_ScanOnce(location_t& loc, int kth);
	
	int posDistance(int ridCur, int idxCur, int queryLocPos);
	double search_dis2QID_LB_InChild(int ridCur, int idxCur, int queryLocPos);
	double search_dis2QID_LB(int ridCur, int idxCur, int queryLocPos);
	pair<double,double> calc_dis2QID_LB_and_AllLB(int ridCUr, int idxCur, int ridNxt, int idxNxt, int queryLocPos, double _dis2QID_LB, double _dis2QID_AllLB, double _dis2QID_LB_InChild);
	void testAll_calc_dis2QID_LB_and_AllLB();
	void test_calc_dis2QID_LB_and_AllLB(int queryLocID);	
	double _initDk_ScanOnce(location_t& loc, int queryLocPos, int kth, int& oid, double& dis2loc);
};

extern HST_Lite_t HSTlite;

double distCircle(location_t& o, double r, location_t& q);
bool intersects(location_t& o1, double r1, location_t& o2, double r2);
bool within(location_t& oq, double rq, location_t& p);

#endif
