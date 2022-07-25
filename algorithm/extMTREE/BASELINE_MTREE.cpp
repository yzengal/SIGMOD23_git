
#include "BASELINE_MTREE.h"
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif


void Mtree_t::__initDisk(Node_t *rt) {
	if (rt->isLeaf()) {
		// dump to File
		rt->pageId = pageIdx;
		assert(rt->entry_N <= pageSize);
		dumpToDisk(pageIdx++, rt->entry, rt->entry_N);
	} else {	
		for (auto iter=rt->child.begin(); iter!=rt->child.end(); ++iter) {
			__initDisk(iter->second);
		}
	}
}

pair<int,int> Mtree_t::promotion(Node_t* p) {
	pair<int,int> ret;
	
	ret.first = p->nomIdx;
	if (p->isLeaf()) {
		vector<int> vtmp;
		
		for (int i=0; i<p->entry_N; ++i) {
			if (p->nomIdx != p->entry[i].first) 
				vtmp.push_back(p->entry[i].first);
		}
		assert(vtmp.size() > 0);
		
		ret.second = vtmp[rand()%vtmp.size()];
	} else {
		vector<int> vtmp;
		
		for (map<int, Node_t*>::iterator iter=p->child.begin(); iter!=p->child.end(); ++iter) {
			if (p->nomIdx != iter->second->nomIdx) 
				vtmp.push_back(iter->second->nomIdx);
		}
		assert(vtmp.size() > 0);
		
		ret.second = vtmp[rand()%vtmp.size()];
	}
	
	return ret;
}

void Mtree_t::balancedLeafPartition(Node_t* p, pair<int,int> promote, set<int>& setA, set<int>& setB) {
	setA.clear();
	setB.clear();
	int n = p->entry_N;
	vector<pair<double,int> > disA, disB;
	
	for (int i=0; i<n; ++i) {
		disA.push_back( make_pair(dist(V[promote.first], V[p->entry[i].first], tag), p->entry[i].first) );
		disB.push_back( make_pair(dist(V[promote.second], V[p->entry[i].first], tag), p->entry[i].first) );
	}
	sort(disA.begin(), disA.end());
	sort(disB.begin(), disB.end());
	
	int i = 0, j = 0;
	while (i<n || j<n) {
		while (i < n) {
			int pid = disA[i].second; 
			if (setB.find(pid) == setB.end()) {
				setA.insert(pid);
				++i;
				break;
			}
			++i;
		}
		
		while (j < n) {
			int pid = disB[j].second; 
			if (setA.find(pid) == setA.end()) {
				setB.insert(pid);
				++j;
				break;
			}
			++j;
		}
	}
}

void Mtree_t::balancedNonLeafPartition(Node_t* p, pair<int,int> promote, set<int>& setA, set<int>& setB) {
	setA.clear();
	setB.clear();
	int n = p->child.size();
	vector<pair<double,int> > disA, disB;
	
	for (auto iter=p->child.begin(); iter!=p->child.end(); ++iter) {
		disA.push_back( make_pair(dist(V[promote.first], V[iter->second->nomIdx], tag), iter->first) );
		disB.push_back( make_pair(dist(V[promote.second], V[iter->second->nomIdx], tag), iter->first) );
	}
	sort(disA.begin(), disA.end());
	sort(disB.begin(), disB.end());
	
	int i = 0, j = 0;
	while (i<n || j<n) {
		while (i < n) {
			int nid = disA[i].second; 
			if (setB.find(nid) == setB.end()) {
				setA.insert(nid);
				++i;
				break;
			}
			++i;
		}
		
		while (j < n) {
			int nid = disB[j].second; 
			if (setA.find(nid) == setA.end()) {
				setB.insert(nid);
				++j;
				break;
			}
			++j;
		}
	}	
}

Node_t* Mtree_t::splitLeaf(Node_t* p) {	
	pair<int,int> promote = promotion(p);
	assert(promote.first == p->nomIdx); // confirmed split strategy
	
	set<int> setA;
	set<int> setB;
	balancedLeafPartition(p, promote, setA, setB);
	
	Node_t* q = createLeaf(p->far, promote.second);
	for (auto iter=setB.begin(); iter!=setB.end(); ++iter) {
		double disTmp = dist(V[*iter], V[promote.second], tag);
		q->addEntry(*iter, disTmp);
	}
	
	p->clearEntry();
	for (auto iter=setA.begin(); iter!=setA.end(); ++iter) {
		double disTmp = dist(V[*iter], V[promote.first], tag);
		p->addEntry(*iter, disTmp);
	}
	
	
	return q;
}

Node_t* Mtree_t::splitNonLeaf(Node_t* p) {	
	pair<int,int> promote = promotion(p);
	assert(promote.first == p->nomIdx); // confirmed split strategy
	
	set<int> setA;
	set<int> setB;
	balancedNonLeafPartition(p, promote, setA, setB);
	
	Node_t* q = createNonLeaf(p->far, promote.second);
	for (auto iter=setB.begin(); iter!=setB.end(); ++iter) {
		Node_t* son = p->child[*iter];
		son->far = q;
		son->dis2par = dist(V[son->nomIdx], V[q->nomIdx], tag);
		q->addChild(son);
		p->removeChild(son);
	}
	
	p->radius = 0.0;
	for (auto iter=setA.begin(); iter!=setA.end(); ++iter) {
		Node_t* son = p->child[*iter];
		p->updateRadius(son);
	}
	
	return q;
}

void Mtree_t::insert(int pid) {
	if (rt == NULL) {
		rt = createLeaf(NULL, pid);
		rt->addEntry(pid, 0.0);
	} else {
		Node_t* p = rt, *q;
		
		// from top to bottom
		while (p!=NULL && !p->isLeaf()) {
			double minIncDist = 1e20, nearestDist = 1e20;
			map<int, Node_t*>::iterator _iter; 
			for (auto iter=p->child.begin(); iter!=p->child.end(); ++iter) {
				double dis = dist(V[iter->second->nomIdx], V[pid], tag);
				double incDis = max(0.0, dis-p->radius);
				if (incDis < minIncDist) {
					minIncDist = incDis;
					nearestDist = dis;
					_iter = iter;
				} else if (incDis==minIncDist && dis<nearestDist) {
					nearestDist = dis;
					_iter = iter;
				}
			}
			p = _iter->second;
		}
		
		while (p != NULL) {
			double dis = dist(V[p->nomIdx], V[pid], tag);
			p->radius = max(dis, p->radius);
			q = NULL;
			if (p->isLeaf()) {
				p->addEntry(pid, dis);
				if (p->entry_N > maxLeafCapacity) {
					// split the leaf node
					q = splitLeaf(p);
					assert(q->far == p->far);
					if (p->far == NULL)
						assert(q->dis2par == 0.0);
					else
						assert(dcmp(q->dis2par - dist(V[q->nomIdx], V[p->far->nomIdx], tag)) == 0);
				}
			} else {
				if (p->childSize() > maxNonLeafCapacity) {
					// split the non-leaf node
					q = splitNonLeaf(p);
					assert(q->far == p->far);
					if (p->far == NULL)
						assert(q->dis2par == 0.0);
					else
						assert(dcmp(q->dis2par - dist(V[q->nomIdx], V[p->far->nomIdx], tag)) == 0);
				}
			}
			if (q != NULL) {
				if (p->far == NULL) {
					rt = createNonLeaf(NULL, p->nomIdx);
					p->far = q->far = rt;
					rt->addChild(p);
					q->dis2par = dist(V[rt->nomIdx], V[q->nomIdx], tag);
					rt->addChild(q);
				} else {
					// q->dis2par = dist(V[p->far->nomIdx], V[q->nomIdx], tag);
					p->far->addChild(q);
					q->far = p->far;
				}
			}
			p = p->far;
		}
		
		assert(rt->far == NULL);
	}
}

void Mtree_t::_searchLeafNode_RQ(double disToCenter, Node_t* p, location_t& loc, double radius, vector<int>& answers) {
	const int n_out = p->entry_N;
	const int pageId = p->pageId;
	if (n_out == 0) return ;
	assert(n_out <= pageSize);
	
	location_t *locs =  new location_t[n_out];
	double *dists = new double[n_out];
	readFromDisk(pageId, locs, dists, n_out);
	pageAccess++;
	
	for (int i=0; i<n_out; ++i) {
		if (fabs(disToCenter-dists[i]) <= radius) {
			if (dist(locs[i], loc, tag) <= radius) {
				answers.push_back(p->entry[i].first);
			}
		}
	}
	
	delete[] locs;
	delete[] dists;
}

void Mtree_t::_MTree_RangeQuery(location_t& loc, double radius, vector<int>& answers) {
	typedef pair<double,Node_t*> qpair_t;
	queue<qpair_t> Q;
	qpair_t tmp, cur;
	double r;
	
	answers.clear();
	if (rt != NULL) {
		tmp.second = rt;
		tmp.first = dist(V[rt->nomIdx], loc, tag);
		r = rt->radius;
		if (tmp.first <= r+radius)
			Q.push(tmp);
	}
		
	Node_t *p, *q;
	int pid;
	while (!Q.empty()) {
		cur = Q.front();
		Q.pop();
		p = cur.second;
		if (false == p->isLeaf()) {// rt is an internal node
			for (auto iter=p->child.begin(); iter!=p->child.end(); ++iter) {
				q = tmp.second = iter->second;
				pid = q->nomIdx;
				r = q->radius;
				if (fabs(cur.first-q->dis2par) <= r+radius) {
					tmp.first = dist(V[pid], loc, tag);
					if (tmp.first <= r+radius) {
						Q.push(tmp);
					}
				}
			}
		} else {// rt is a leaf node
			_searchLeafNode_RQ(cur.first, p, loc, radius, answers);
		}
	}
}

void Mtree_t::_searchLeafNode_KnnQ(double disToCenter, Node_t* p, double& dk, location_t& loc, int kth, vector<int>& answers, set<nnpair_t, greater<nnpair_t> >& NNU) {
	nnpair_t nntmp;
	int NNU_sz = NNU.size();
	const int n_out = p->entry_N;
	const int pageId = p->pageId;
	if (n_out == 0) return ;
	assert(n_out <= pageSize);
	
	location_t *locs =  new location_t[n_out];
	double *dists = new double[n_out];
	readFromDisk(pageId, locs, dists, n_out);
	pageAccess++;
	
	for (int i=0; i<p->entry_N; ++i) {
		if (fabs(disToCenter-dists[i]) > dk)
			continue;
		int pid = p->entry[i].first;
		nntmp.first = dist(locs[i], loc, tag);
		nntmp.second = -1 - pid;	
		if (nntmp.first <= dk) {
			NNU.insert(nntmp);
			++NNU_sz;
			if (NNU_sz > kth) {
				--NNU_sz;
				NNU.erase(NNU.begin());
			}
			if (NNU_sz >= kth)
				dk = NNU.begin()->first;
		}
	}	
	
	delete[] locs;
	delete[] dists;
}

void Mtree_t::_MTree_KnnQuery(location_t& loc, int kth, vector<int>& answers) { 
	priority_queue<qpair_t, vector<qpair_t>, greater<qpair_t> > PQ;
	nnpair_t nntmp;
	int NNU_sz = 1;
	set<nnpair_t, greater<nnpair_t> > NNU;
	Node_t *p, *q;
	qpair_t tmp, cur;
	int pid;
	double r, dk = INF;
	
	answers.clear();
	if (kth == 0) return ;
	if (MTree_nV == 1) {
		answers.push_back(0);
		return ;
	}
	
	if (rt != NULL) {
		tmp.second.second = rt;
		tmp.second.first = dist(V[rt->nomIdx], loc, tag);
		r = rt->radius;
		tmp.first = max(tmp.second.first-r, 0.0);
		dk = tmp.second.first + r + 1.0;
		PQ.push(tmp);
		NNU.insert(make_pair(tmp.second.first+r,rt->nid));
	}
	
	while (!PQ.empty()) {
		/* Choose Next Node */
		cur = PQ.top();
		PQ.pop();
		p = cur.second.second;
		/* Choose Next Node */
		
		if (NNU.find(make_pair(cur.second.first+p->radius,p->nid)) != NNU.end()) {
			NNU.erase(make_pair(cur.second.first+p->radius,p->nid));
			--NNU_sz;
		}
		
		/* p is a leaf node */
		if (p->isLeaf()) {
			_searchLeafNode_KnnQ(cur.second.first, p, dk, loc, kth, answers, NNU);
			NNU_sz = NNU.size();
			continue;
		}
		
		/* p is an internal node */
		for (auto iter=p->child.begin(); iter!=p->child.end(); ++iter) {
			q = iter->second, tmp.second.second = iter->second;
			pid = q->nomIdx;
			r = q->radius;
			if (fabs(cur.second.first-q->dis2par) <= r+dk) {
				tmp.second.first = dist(V[pid], loc, tag);
				tmp.first = max(tmp.second.first-r, 0.0);
				if (tmp.first <= dk) {
					PQ.push(tmp);
					
					nntmp.first = tmp.second.first + r;
					nntmp.second = q->nid;	
					if (nntmp.first <= dk) {
						NNU.insert(nntmp);
						++NNU_sz;
						if (NNU_sz > kth) {
							--NNU_sz;
							NNU.erase(NNU.begin());
						}
						if (NNU_sz >= kth)
							dk = NNU.begin()->first;
					}
				}
			}
		}

		if (!PQ.empty() && PQ.top().first>dk) break;
	}

	answers.clear();
	for (auto iter=NNU.begin(); iter!=NNU.end(); ++iter) {
		if (iter->second < 0) {
			pid = -1-iter->second;
			answers.push_back(pid);
		}
	}
	reverse(answers.begin(), answers.end());
}

