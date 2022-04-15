#include "HST_MTree_Query.h"
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

HST_Lite_t HSTlite;

inline bool intersects(location_t& o1, double r1, location_t& o2, double r2) {
	double l = dist(o1, o2, tag);
	return l <= r1+r2;
}

inline bool within(location_t& oq, double rq, location_t& p) {
	return dist(oq, p, tag) <= rq;
}

inline double distCircle(location_t& o, double r, location_t& q) {
	double tmp = dist(o, q, tag);
	tmp -= r;
	if (tmp < 0) tmp = 0;
	return tmp;
}

void HST_Lite_t::_MTree_RangeQuery(location_t& loc, double radius, vector<int>& answers) {
	typedef pair<double,HST_Node_t*> qpair_t;
	queue<qpair_t> Q;
	qpair_t tmp, cur;
	double r;
	
	answers.clear();
	if (rt != NULL) {
		tmp.second = rt;
		tmp.first = dist(V[rt->nomIdx], loc, tag);
		r = radAtLevel(rt->lev);
		if (tmp.first <= r+radius)
			Q.push(tmp);
	}
		
	HST_Node_t *p, *q;
	int pid;
	while (!Q.empty()) {
		cur = Q.front();
		Q.pop();
		p = cur.second;
		if (p->child_n > 0) {// rt is an internal node
			for (int i=0; i<p->child_n; ++i) {
				q = tmp.second = p->child[i];
				pid = q->nomIdx;
				if (q->child_n > 0) {
					r = radAtLevel(q->lev);
					if (fabs(cur.first-q->dis2par) <= r+radius) {
						tmp.first = dist(V[pid], loc, tag);
						if (tmp.first <= r+radius) {
							Q.push(tmp);
						}
					}
				} else {
					if (fabs(cur.first-q->dis2par) <= radius) {
						if (dist(V[pid], loc, tag) <= radius) {
							answers.push_back(pid);
						}
					}
				}
			}
		} else {// rt is a leaf node
			pid = p->nomIdx;
			if (cur.first <= radius) {
				answers.push_back(pid);
			}
		}
	}
}

void HST_Lite_t::_MTree_KnnQuery(location_t& loc, int kth, vector<int>& answers) { 
	typedef pair<double,pair<double,HST_Node_t*> > qpair_t;
	priority_queue<qpair_t, vector<qpair_t>, greater<qpair_t> > PQ;
	typedef pair<double,int> nnpair_t;
	nnpair_t nntmp;
	vector<nnpair_t> NNU;
	int NNU_sz = kth;
	HST_Node_t *p, *q;
	qpair_t tmp, cur;
	int pid;
	double r, dk = INF;
	
	answers.clear();
	if (kth == 0) return ;
	if (HST_nV == 1) {
		answers.push_back(0);
		return ;
	}
	
	if (rt != NULL) {
		tmp.second.second = rt;
		tmp.second.first = dist(V[rt->nomIdx], loc, tag);
		r = radAtLevel(rt->lev);
		tmp.first = max(tmp.second.first-r, 0.0);
		dk = tmp.second.first + r + 1.0;
		PQ.push(tmp);
		NNU.push_back(make_pair(tmp.second.first+r,rt->nid));
		nntmp.first = dk;
		for (int i=1; i<kth; ++i) {
			nntmp.second = HST_node_n + i;
			NNU.push_back(nntmp);
		}
	}
		
	while (!PQ.empty()) {
		/* Choose Next Node */
		cur = PQ.top();
		PQ.pop();
		p = cur.second.second;
		/* Choose Next Node */
		
		vector<nnpair_t>::iterator iter = lower_bound(NNU.begin(), NNU.end(), make_pair(cur.second.first+radAtLevel(p->lev),p->nid));
		if (iter!=NNU.end() && iter->second==p->nid) {
			NNU.erase(iter);
			--NNU_sz;
		}
		/* p is an internal node */
		for (int i=0; i<p->child_n; ++i) {
			q = p->child[i], tmp.second.second = p->child[i];
			pid = q->nomIdx;
			r = (q->child_n==0) ? 0.0 : radAtLevel(q->lev);
			if (fabs(cur.second.first-q->dis2par) <= r+dk) {
				tmp.second.first = (pid==p->nomIdx) ? cur.second.first : dist(V[pid], loc, tag);
				// tmp.second.first = dist(V[pid], loc);
				tmp.first = max(tmp.second.first-r, 0.0);
				if (tmp.first <= dk) {
					if (q->child_n > 0) {
						PQ.push(tmp);
					}
					
					nntmp.first = (q->child_n>0) ? (tmp.second.first+r) : tmp.second.first;
					nntmp.second = (q->child_n>0) ? q->nid : (-1-pid);	
					if (nntmp.first<dk || (q->child_n==0 && nntmp.first==dk)) {
						//// update NNU
						if (NNU_sz == kth) {
							if (NNU[NNU_sz-1] > nntmp) {
								iter = upper_bound(NNU.begin(), NNU.end(), nntmp);
								NNU.insert(iter, nntmp);
								NNU.pop_back();
								dk = NNU.rbegin()->first;
							}
						} else {
							iter = upper_bound(NNU.begin(), NNU.end(), nntmp);
							NNU.insert(iter, nntmp);
							++NNU_sz;
							dk = NNU.rbegin()->first;
						}
					}
				}
			}
		}

		if (!PQ.empty() && PQ.top().first>dk) break;
	}

	answers.clear();
	for (int i=NNU.size()-1; i>=0; --i) {
		if (NNU[i].second < 0) {
			pid = -1-NNU[i].second;
			answers.push_back(pid);
		}
	}
}
