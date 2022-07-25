
#ifndef BASELINE_MTREE_H
#define BASELINE_MTREE_H

#include <bits/stdc++.h>
using namespace std;
#include "global.h"
#include "MemoryManager.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

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
	// Node_t** child; // the child nodes
	
	Node_t(Node_t* far_) {
		far = far_;
		nomIdx = 0;
		dis2par = 0.0;
		radius = 0.0;
		entry = NULL;
		entry_N = 0;
	}
	
	Node_t(Node_t* far_, int nomIdx_) {
		far = far_;
		nomIdx = nomIdx_;
		dis2par = 0.0;
		radius = 0.0;
		entry = NULL;
		entry_N = 0;
	}
	
	~Node_t() {
		if (entry != NULL)
			delete[] entry;
	}
	
	bool isLeaf() {
		return child.empty();
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

struct Mtree_t {
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
	long long pageAccess;
	
	Mtree_t(int maxNonLeafCapacity_=10, int maxLeafCapacity_=50):
		maxNonLeafCapacity(maxNonLeafCapacity_), maxLeafCapacity(maxLeafCapacity_) {
		printf("maxNonLeafCapacity = %d, maxLeafCapacity = %d\n", maxNonLeafCapacity, maxLeafCapacity);
	}
	
	~Mtree_t() {
		if (rt != NULL)
			freeMTree(rt);
		removeFromFile(pageIdx);
	}
	
	size_t countTree() {
		return MTree_node_n;
	}
	
	double _memoryTree(Node_t *rt) {
		double ret = 0;
		if (rt != NULL) {
			if (rt->isLeaf()) {
				ret += sizeof(int)*5+sizeof(double)*2+sizeof(Node_t*)*3;
			} else {
				ret += sizeof(int)*5+sizeof(double)*2+sizeof(Node_t*)*2+(sizeof(int)+sizeof(Node_t*))*maxNonLeafCapacity;
			}
			for (auto iter=rt->child.begin(); iter!=rt->child.end(); ++iter)
				ret += _memoryTree(iter->second);
		}
		return ret;	
	}
	
	double memoryTree() {
		return _memoryTree(rt);	
	}
	
	bool _checkLink(Node_t *rt) {
		if (rt->isLeaf()) return rt->entry_N>0 && rt->entry_N<=maxLeafCapacity;
		
		if (rt->child.size() > maxNonLeafCapacity)
			return false;
		for (auto iter=rt->child.begin(); iter!=rt->child.end(); ++iter) {
			if (iter->second->far != rt) return false;
			if (iter->second->nid != iter->first) return false;
			if (0 != dcmp(iter->second->dis2par - dist(V[rt->nomIdx], V[iter->second->nomIdx], tag))) 
				return false;
			if (false == _checkLink(iter->second)) return false;
		}
		
		return true;
	}
	
	bool _checkEntry(Node_t *rt, vector<int>& arr, vector<bool>& visit) {
		if (rt->isLeaf()) {
			for (int i=0; i<rt->entry_N; ++i) {
				int pid = rt->entry[i].first;
				if (true == visit[pid]) return false;
				visit[pid] = true;
				arr.push_back(pid);
				double dis = dist(V[pid], V[rt->nomIdx], tag);
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
				// if (dcmp(rt->radius - dist(V[pid], V[rt->nomIdx], tag)) < 0)
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
			ret->dis2par = dist(V[far->nomIdx], V[nomIdx], tag);
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
			ret->dis2par = dist(V[far->nomIdx], V[nomIdx], tag);
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
	
	void RangeQuery(location_t& loc, double radius, vector<int>& answers) {
		pageAccess = 0;
		_MTree_RangeQuery(loc, radius, answers);
	}
	
	void KnnQuery(location_t& loc, int kth, vector<int>& answers) {
		pageAccess = 0;
		_MTree_KnnQuery(loc, kth, answers);
	}
	
	void RangeQuery(int queryLocID, double radius, vector<int>& answers) {
		pageAccess = 0;
		_MTree_RangeQuery(V[queryLocID], radius, answers);
	}
	
	void KnnQuery(int queryLocID, int kth, vector<int>& answers) {
		pageAccess = 0;
		_MTree_KnnQuery(V[queryLocID], kth, answers);
	}
	
	void constructIndex(int nV) {
		init(nV);
		for (int i=0; i<MTree_nV; ++i)
			insert(i);
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
	
	void _MTree_RangeQuery(location_t& loc, double radius, vector<int>& answers);
	void _MTree_KnnQuery(location_t& loc, int kth, vector<int>& answers);
	void _searchLeafNode_RQ(double disToCenter, Node_t* p, location_t& loc, double radius, vector<int>& answers);
	void _searchLeafNode_KnnQ(double disToCenter, Node_t* p, double& dk, location_t& loc, int kth, vector<int>& answers, set<nnpair_t, greater<nnpair_t> >& NNU);
};

#endif
