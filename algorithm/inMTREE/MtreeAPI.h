/*
	This is a file header to package the Mtree from Eduardo R. D'Avila (https://github.com/erdavila) 
	into our experimental environment.
*/

#ifndef MTREEAPI_H
#define MTREEAPI_H

#include <bits/stdc++.h>
using namespace std;
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

#include "global.h"
#include "mtree.h"

// struct Data_t {
	// int id;
	// location_t loc;
	
	// Data_t& operator=(const Data_t& d) {
		// if (this != d) {
			// id = d.id;
			// loc = d.loc;
		// }

		// return *this;
	// }
// };

// double local_dist(Data_t& a, Data_t& b) {
	// return dist(V[a.id], V[b.id], tag);
// }

typedef int Data_t;

struct local_dist {
	double operator()(const Data_t& a, const Data_t& b) const {
		return dist(V[a], V[b], tag);
	}
};

struct Index_t {
	typedef mt::mtree<Data_t, local_dist> MTree;
	MTree mtree;
	
	void constructIndex(int nV) {
		for (int i=0; i<nV; ++i) {
			mtree.add(i);
		}
	}
	
	void RangeQuery(int qid, double radius, vector<int>& ans) {
		ans.clear();
		MTree::query query = mtree.get_nearest_by_range(qid, radius);
		for(MTree::query::iterator iter = query.begin(); iter != query.end(); ++iter) {
			ans.push_back(iter->data);
		}
	}
	
	void KnnQuery(int qid, int kth, vector<int>& ans) {
		ans.clear();
		MTree::query query = mtree.get_nearest_by_limit(qid, (size_t)kth);
		for(MTree::query::iterator iter = query.begin(); iter != query.end(); ++iter) {
			ans.push_back(iter->data);
		}
	}
	
	long long memoryTree() {
		return mtree.memoryTree();
	}
	
	size_t countTree() {
		return mtree.countTree();
	}
};


#endif