
#ifndef BASELINE_PMTREE_H
#define BASELINE_PMTREE_H

#include <bits/stdc++.h>
using namespace std;
#include "global.h"
#include "MemoryManager.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif


extern double local_dist(location_t& a, location_t& b);

struct Node_t {
	int nid;			// the ID of the node
	int nomIdx; 		// the id of norminator point
	double dis2par;		// te distance of V[nomIdx] to V[far->nomIdx]
	double radius;		// the radius of the covering area		
	Node_t* far;	// the parent node
	map<int, Node_t*> child; // the child nodes
	int entry_N;
	pair<int, double>* entry;
	int pageId;
	vector<pair<double,double> > HR;
	double **PD;
	// Node_t** child; // the child nodes
	
	Node_t(Node_t* far_) {
		far = far_;
		nomIdx = 0;
		dis2par = 0.0;
		radius = 0.0;
		entry = NULL;
		entry_N = 0;
		PD = NULL;
	}
	
	Node_t(Node_t* far_, int nomIdx_) {
		far = far_;
		nomIdx = nomIdx_;
		dis2par = 0.0;
		radius = 0.0;
		entry = NULL;
		entry_N = 0;
		PD = NULL;
	}
	
	~Node_t() {
		if (PD != NULL) {
			for (int i=0; i<entry_N; ++i) {
				delete[] PD[i];
			}
			delete[] PD;
		}
		if (entry != NULL)
			delete[] entry;
	}
	
	bool isLeaf() {
		return child.empty();
	}
	
	void initLeafPivot(const int PIVOT_N) {
		initNonLeafPivot(PIVOT_N);
		PD = new double*[entry_N];
		for (int i=0; i<entry_N; ++i) {
			PD[i] = new double[PIVOT_N];
		}
	}
	
	void initNonLeafPivot(const int PIVOT_N) {
		HR.resize(PIVOT_N);
		for (int i=0; i<PIVOT_N; ++i) {
			HR[i].first = INF;
			HR[i].second = -INF;
		}
	}
	
	void initLeafNode(int CAP) {
		entry = new pair<int, double>[CAP+1];
		entry_N = 0;
	}
	
	void updateRadius(Node_t* son) {
		radius = max(radius, son->radius+son->dis2par);
	}
	
	void updateRadius(int pid, double dis) {
		radius = max(radius, dis);
	}
	
	void addChild(Node_t* son) {
		assert(child.find(son->nid) == child.end());
		if (child.find(son->nid) == child.end()) {
			child[son->nid] = son;
			updateRadius(son);
		}
	}
	
	size_t childSize() {
		return child.size();
	}
	
	void removeChild(int nid) {
		assert(child.find(nid) != child.end());
		child.erase(nid);
	}
	
	void removeChild(Node_t* son) {
		assert(child.find(son->nid) != child.end());
		child.erase(son->nid);
	}
	
	void clearChild() {
		child.clear();
	}
	
	void addEntry(int pid, double dis) {
		updateRadius(pid, dis);
		entry[entry_N++] = make_pair(pid, dis);
	}
	
	void clearEntry() {
		entry_N = 0;
	}
};


struct PMtree_t {
	typedef pair<double,pair<double,Node_t*> > qpair_t;
	typedef pair<double,int> nnpair_t;
	const int maxNonLeafCapacity = 10;
	const int maxLeafCapacity = 50;
	const int minNonLeafCapacity = (maxNonLeafCapacity+1)/2;
	const int minLeafCapacity = (maxLeafCapacity+1)/2;
	
	Node_t *rt;
	int MTree_nV;
	int MTree_node_n;
	int pageIdx;
	vector<int> pivots;
	vector<double> boundary;
	long long pageAccess;
	
	PMtree_t(int maxNonLeafCapacity_=10, int maxLeafCapacity_=50):
		maxNonLeafCapacity(maxNonLeafCapacity_), maxLeafCapacity(maxLeafCapacity_) {
		printf("maxNonLeafCapacity = %d, maxLeafCapacity = %d\n", maxNonLeafCapacity, maxLeafCapacity);
	}
	
	~PMtree_t() {
		if (rt != NULL)
			freeMTree(rt);
		removeFromFile(pageIdx);
	}
	
	void _processPivot(Node_t *rt) {
		if (rt == NULL) return ;
		if (rt->isLeaf()) {
			rt->initLeafPivot(PIVOT_N);
			for (int i=0; i<rt->entry_N; ++i) {
				int pid = rt->entry[i].first, qid;
				for (int j=0; j<PIVOT_N; ++j) {
					qid = pivots[j];
					rt->PD[i][j] = local_dist(V[pid], V[qid]);
					rt->HR[j].first = min(rt->HR[j].first, rt->PD[i][j]);
					rt->HR[j].second = max(rt->HR[j].second, rt->PD[i][j]);
				}
			}
		} else {
			rt->initNonLeafPivot(PIVOT_N);
			for (auto iter=rt->child.begin(); iter!=rt->child.end(); ++iter) {
				Node_t* son = iter->second;
				_processPivot(son);
				for (int j=0; j<PIVOT_N; ++j) {
					rt->HR[j].first = min(rt->HR[j].first, son->HR[j].first);
					rt->HR[j].second = max(rt->HR[j].second, son->HR[j].second);
				}
			}
		}
	}
	
	size_t countTree() {
		return MTree_node_n;
	}
	
	double _memoryTree(Node_t *rt) {
		double ret = 0;
		if (rt != NULL) {
			if (rt->isLeaf()) {
				ret += sizeof(int)*5+sizeof(double)*2+sizeof(Node_t*)*5+sizeof(double)*2*1;
			} else {
				ret += sizeof(int)*5+sizeof(double)*2+sizeof(Node_t*)*4+(sizeof(int)+sizeof(Node_t*))*maxNonLeafCapacity+sizeof(double)*2*maxNonLeafCapacity;
			}
			for (auto iter=rt->child.begin(); iter!=rt->child.end(); ++iter)
				ret += _memoryTree(iter->second);
		}
		return ret;	
	}
	
	double memoryTree() {
		return _memoryTree(rt);	
	}
	
	bool _checkPivot(Node_t *rt) {
		if (rt == NULL) return true;
		assert(false == rt->HR.empty());
		if (rt->isLeaf()) {
			assert(rt->PD != NULL);
			for (int i=0; i<rt->entry_N; ++i) {
				int pid = rt->entry[i].first;
				for (int j=0; j<PIVOT_N; ++j) {
					int qid = pivots[j];
					double dis = local_dist(V[pid], V[qid]);
					if (0 != dcmp(rt->PD[i][j] - dis)) 
						return false;
					if (rt->HR[j].first>dis || rt->HR[j].second<dis) 
						return false;
				}
			}
		} else {
			for (auto iter=rt->child.begin(); iter!=rt->child.end(); ++iter) {
				Node_t* son = iter->second;
				for (int j=0; j<PIVOT_N; ++j) {
					if (rt->HR[j].first>son->HR[j].first || rt->HR[j].second<son->HR[j].second) 
						return false;
				}
			}
		}
		return true;
	}
	
	bool _checkLink(Node_t *rt) {
		if (rt == NULL) return true;
		if (rt->isLeaf()) return rt->entry_N>0 && rt->entry_N<=maxLeafCapacity;
		
		if (rt->child.size() > maxNonLeafCapacity)
			return false;
		for (auto iter=rt->child.begin(); iter!=rt->child.end(); ++iter) {
			if (iter->second->far != rt) return false;
			if (iter->second->nid != iter->first) return false;
			if (0 != dcmp(iter->second->dis2par - local_dist(V[rt->nomIdx], V[iter->second->nomIdx]))) 
				return false;
			if (false == _checkLink(iter->second)) return false;
		}
		
		return true;
	}
	
	bool _checkEntry(Node_t *rt, vector<int>& arr, vector<bool>& visit) {
		if (rt == NULL) return true;
		if (rt->isLeaf()) {
			for (int i=0; i<rt->entry_N; ++i) {
				int pid = rt->entry[i].first;
				if (true == visit[pid]) return false;
				visit[pid] = true;
				arr.push_back(pid);
				double dis = local_dist(V[pid], V[rt->nomIdx]);
				if (dcmp(rt->entry[i].second - dis) != 0)
					return false;
				if (dcmp(rt->radius - dis) < 0)
					return false;
			}
			
			return true;
		}
		
		for (auto iter=rt->child.begin(); iter!=rt->child.end(); ++iter) {
			int bt = arr.size();
			if (false == _checkEntry(iter->second, arr, visit)) return false;
			int et = arr.size();
			
			// for (int i=bt; i<et; ++i) {
				// int pid = arr[i];
				// if (dcmp(rt->radius - local_dist(V[pid], V[rt->nomIdx])) < 0)
					// return false;
			// }
		}
		
		return true;
	}
	
	bool _check() {
		// check Link
		if (rt->far!=NULL || false==_checkLink(rt)) {
			puts("Link has bugs");
			return false;
		}
		// check Entry
		vector<bool> visit(nV, false);
		vector<int> vtmp;
		if (false == _checkEntry(rt, vtmp, visit)) {
			puts("Entry has bugs");
			return false;
		} else {
			for (int i=0; i<nV; ++i) {
				if (!visit[i]) {
					puts("Entry has bugs");
					return false;
				}
			}
		}
		// check Pivot
		if (false == _checkPivot(rt)) {
			puts("Pivot has bugs");
			return false;
		} else {
			for (int i=0; i<nV; ++i) {
				if (!visit[i]) {
					puts("Entry has bugs");
					return false;
				}
			}
		}
		return true;
	}
	
	Node_t *createNonLeaf(Node_t* far, int nomIdx) {
		Node_t *ret;
		
		ret = new Node_t(far, nomIdx);
		ret->nid = MTree_node_n++;
		ret->nomIdx = nomIdx;
		ret->radius = 0.0;
		if (far == NULL) {
			ret->dis2par = 0.0;
		} else {
			ret->dis2par = local_dist(V[far->nomIdx], V[nomIdx]);
		}
		
		return ret;
	}
	
	Node_t *createLeaf(Node_t* far, int nomIdx) {
		Node_t *ret;
		
		ret = new Node_t(far, nomIdx);
		ret->nid = MTree_node_n++;
		ret->nomIdx = nomIdx;
		ret->radius = 0.0;
		if (far == NULL) {
			ret->dis2par = 0.0;
		} else {
			ret->dis2par = local_dist(V[far->nomIdx], V[nomIdx]);
		}
		ret->initLeafNode(maxLeafCapacity);
		
		return ret;
	}
	
	void freeMTree(Node_t* &rt) {
		if (rt == NULL) return ;
	
		Node_t* chi;
		
		for (auto iter=rt->child.begin(); iter!=rt->child.end(); ++iter) {
			chi = iter->second;
			freeMTree(chi);
		}
		
		delete rt;
		rt = NULL;
	}
	
	void dumpTree() {
		_dumpTree(0, rt);
	}
	
	void _dumpNode(int dep, Node_t* p) {
		for (int i=0; i<dep; ++i)
			putchar(' ');
		printf("nomIdx=%d, dis2par=%.4lf, |child|=%d\n", p->nomIdx, p->dis2par, p->child.size());
	}

	void _dumpTree(int dep, Node_t* rt) {
		if (rt != NULL) {
			_dumpNode(dep, rt);
			for (auto iter=rt->child.begin(); iter!=rt->child.end(); ++iter)
				_dumpTree(dep+1, iter->second);
		}
	}
	
	void init(int nV) {
		rt = NULL;
		MTree_nV = nV;
		MTree_node_n = 1;
		pageAccess = 0;
	}
	
	void initBoundary(location_t& loc) {
		boundary.resize(PIVOT_N);
		for (int i=0; i<PIVOT_N; ++i) {
			boundary[i] = local_dist(loc, V[pivots[i]]);
		}
	}
	
	bool checkEntryBoundary(double *PD, double radius) {
		bool flag = true;
		for (int j=0; flag&&j<PIVOT_N; ++j) {
			flag = (fabs(boundary[j] - PD[j]) <= radius);
		}
		return flag;
	}
	
	bool checkNodeBoundary(Node_t* p, double radius) {
		bool flag = true;
		for (int j=0; flag&&j<PIVOT_N; ++j) {
			double mn = boundary[j] - radius;
			double mx = boundary[j] + radius;
			flag = (mn<=p->HR[j].second && mx>=p->HR[j].first);
		}
		return flag;
	}
	
	void RangeQuery(location_t& loc, double radius, vector<int>& answers) {
		pageAccess = 0;
		initBoundary(loc);
		_PMTree_RangeQuery(loc, radius, answers);
	}
	
	void KnnQuery(location_t& loc, int kth, vector<int>& answers) {
		pageAccess = 0;
		initBoundary(loc);
		_PMTree_KnnQuery(loc, kth, answers);
	}
	
	void constructIndex(int nV) {
		init(nV);
		for (int i=0; i<MTree_nV; ++i)
			insert(i);
		
		HFI(PIVOT_N, pivots);
		_processPivot(rt);
		initDisk();
	}
	
	void initDisk() {
		pageIdx = 0;
		if (rt != NULL) {
			__initDisk(rt);
		}
	}
	
	pair<int,int> promotion(Node_t* p);
	void balancedNonLeafPartition(Node_t* p, pair<int,int> promote, set<int>& PTA, set<int>& PTB);
	void balancedLeafPartition(Node_t* p, pair<int,int> promote, set<int>& PTA, set<int>& PTB);
	Node_t* splitLeaf(Node_t* p);
	Node_t* splitNonLeaf(Node_t* p);
	void insert(int pid);
	void __initDisk(Node_t* p);
	
	void _PMTree_RangeQuery(location_t& loc, double radius, vector<int>& answers);
	void _PMTree_KnnQuery(location_t& loc, int kth, vector<int>& answers);
	void _searchLeafNode_RQ(double disToCenter, Node_t* p, location_t& loc, double radius, vector<int>& answers);
	void _searchLeafNode_KnnQ(double disToCenter, Node_t* p, double& dk, location_t& loc, int kth, vector<int>& answers, set<nnpair_t, greater<nnpair_t> >& NNU);
	
	vector<int> PHI(location_t& la, vector<int>& pivots, int pivot_n) {
		vector<int> ret(pivot_n);
		
		for (int i=0; i<pivot_n; ++i) {
			ret[i] = local_dist(la, V[pivots[i]]);
		}
		
		return ret;
	}
	
	int distLB(location_t& la, location_t& lb, vector<int>& pivots, int pivot_n) {
		vector<int> phiA = PHI(la, pivots, pivot_n);
		vector<int> phiB = PHI(lb, pivots, pivot_n);
		int ret = 0;
		
		for (int i=0; i<pivot_n; ++i) {
			ret = max(ret, abs(phiA[i]-phiB[i]));
		}
		
		return ret;
	}
	
	void HFI(int m, vector<int>& pivots) {
		pivots.clear();
		if (nV <= m) {
			for (int i=0; i<nV; ++i)
				pivots.push_back(i);
			return ;
		}
		
		int CPn = 40;
		vector<int> cand;
		HF(CPn, cand);
		assert(m <= CPn);
		
		pivots.resize(m);
		vector<bool> visit(cand.size(), false);
		for (int k=0; k<m; ++k) {
			double precMax = 0.0, precTmp = 0.0;
			int p = -1;
			for (int i=0; i<cand.size(); ++i) {
				if (visit[i]) continue;
				pivots[k] = cand[i];
				precTmp = 0.0;
				for (int pid=0; pid<cand.size(); ++pid) {
					for (int qid=pid+1; qid<cand.size(); ++qid) {
						double disTmp = local_dist(V[cand[pid]], V[cand[qid]]);
						double disLBTmp = distLB(V[cand[pid]], V[cand[qid]], pivots, k+1);
						if (disTmp > 0.0)
							precTmp += disLBTmp / disTmp;
					}
				}
				if (precTmp > precMax) {
					precMax = precTmp;
					p = i;
				}
			}
			assert(p>=0 && p<cand.size());
			pivots[k] = cand[p];
		}
	}
		
	void HF(int m, vector<int>& pivots) {
		pivots.clear();
		if (nV <= m) {
			for (int i=0; i<nV; ++i)
				pivots.push_back(i);
			return ;
		}
		
		// randomly select si in S
		int pid = rand() % nV, qid = pid;
		double disMax = 0.0, disTmp;
		
		// find f1
		for (int i=0; i<nV; ++i) {
			disTmp = local_dist(V[i], V[pid]);
			if (disTmp > disMax) {
				disMax = disTmp;
				qid = i;
			}
		}
		if (m == 1) {
			pivots.push_back(qid);
			return ;
		}
		
		// find f2
		vector<bool> mark(nV, false);
		
		pid = qid, disMax = 0.0;
		mark[pid] = true;
		for (int i=0; i<nV; ++i) {
			if (mark[i]) continue;
			disTmp = local_dist(V[i], V[pid]);
			if (disTmp > disMax) {
				disMax = disTmp;
				qid = i;
			}
		}	
		
		// compute other pivots
		pivots.push_back(pid);
		pivots.push_back(qid);
		m -= 2;
		mark[qid] = true;
		double edge = local_dist(V[pid], V[qid]);
		while (m-- > 0) {
			double errorMin = 1e20, errorTmp = 0.0;
			pid = -1;
			for (int si=0; si<nV; ++si) {
				if (mark[si]) continue;
				errorTmp = 0.0;
				for (int g=0; g<pivots.size(); ++g) {
					errorTmp += fabs(edge - local_dist(V[pivots[g]], V[si]));
				}
				if (errorTmp < errorMin) {
					errorMin = errorTmp;
					pid = si;
				}
			}
			assert(pid>=0 && pid<nV);
			pivots.push_back(pid);
			mark[pid] = true;
		}
	}
	
};

#endif
