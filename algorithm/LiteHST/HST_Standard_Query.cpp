#include "HST_Standard_Query.h"
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


void HST_Lite_t::_Embed_1nnQuery(location_t& loc, vector<int>& answers) {
	
}

void HST_Lite_t::_Embed_RangeQuery(location_t& loc, double radius, vector<int>& answers) {

}

void HST_Lite_t::_Embed_KnnQuery(location_t& loc, int kth, vector<int>& answers) { 

}

void HST_Lite_t::_Embed_RangeQuery(int queryLocID, double radius, vector<int>& answers) {
	HST_Node_t *p = leavesPtr[queryLocID];
	double radiusT = radius * distortion;
	double rad;
	
	answers.clear();
	if (p == NULL) return ;
	
	while (p->far != NULL) {
		rad = radAtLevel(p->lev);
		if (rad >= radiusT) 
			break;
		p = p->far;
	}
	
	int pid, rhs = p->rhs;
	for (int i=p->lhs; i<rhs; ++i) {
		pid = leavesID[i];
		if (dist(V[pid], V[queryLocID], tag) <= radius) {
			answers.push_back(pid);
		}
	}
}

void HST_Lite_t::_Embed_KnnQuery(int queryLocID, int kth, vector<int>& answers) {
	HST_Node_t *p = leavesPtr[queryLocID];
	typedef pair<double,int> nnpair_t;
	priority_queue<nnpair_t, vector<nnpair_t>, less<nnpair_t> > NNU;
	double dk, dkT;
	double rad, tmp;
	int lhs = p->lhs, rhs = p->rhs, pid;
	
	answers.clear();
	if (p == NULL) return ;
	
	rad = radAtLevel(1);
	dk = rad + rad;
	dkT = dk * distortion;
	for (int i=1; i<kth; ++i)
		NNU.push(make_pair(dk,-1));
	NNU.push(make_pair(0.0, queryLocID));
	p = p->far;
	dk = NNU.top().first;
	dkT = dk * distortion;
	
	while (p->far != NULL) {
		rad = radAtLevel(p->lev);
		if (rad >= dkT) 
			break;
		for (int i=p->lhs; i<lhs; ++i) {
			pid = leavesID[i];
			tmp = dist(V[queryLocID], V[pid], tag);
			NNU.push(make_pair(tmp,pid));
			NNU.pop();
		}
		for (int i=rhs; i<p->rhs; ++i) {
			pid = leavesID[i];
			tmp = dist(V[queryLocID], V[pid], tag);
			NNU.push(make_pair(tmp,pid));
			NNU.pop();
		}
		lhs = p->lhs;
		rhs = p->rhs;
		dk = NNU.top().first;
		dkT = dk * distortion;
		p = p->far;
	}
	
	while (!NNU.empty()) {
		pid = NNU.top().second;
		NNU.pop();
		if (pid >= 0)
			answers.push_back(pid);
	}
}

