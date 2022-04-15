
#include "LiteHST_DSU_Query.h"
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

struct RQ_Qnode_t {
	int rid, idx;
	double dis2loc;
	
	RQ_Qnode_t& operator=(const RQ_Qnode_t& oth) {
		if (this != &oth) {
			dis2loc = oth.dis2loc;
			rid = oth.rid;
			idx = oth.idx;
		}
		
		return *this;
	}
	
	void print() const {
		printf("rid=%d, idx=%d, dis2loc=%.4lf\n", rid, idx, dis2loc);
	}
};

struct RQ_Qnode_Lite_t {
	int rid;
	double dis2loc;
	
	RQ_Qnode_Lite_t& operator=(const RQ_Qnode_Lite_t& oth) {
		if (this != &oth) {
			dis2loc = oth.dis2loc;
			rid = oth.rid;
		}
		
		return *this;
	}
	
	void print() const {
		printf("rid=%d, dis2loc=%.4lf\n", rid, dis2loc);
	}
};

struct RQ_Qnode_Lite_WithLB_t {
	int rid;
	double dis2loc;
	double dis2QID_LB;
	double dis2QID_AllLB;
	
	RQ_Qnode_Lite_WithLB_t& operator=(const RQ_Qnode_Lite_WithLB_t& oth) {
		if (this != &oth) {
			rid = oth.rid;
			dis2loc = oth.dis2loc;
			dis2QID_LB = oth.dis2QID_LB;
			dis2QID_AllLB = oth.dis2QID_AllLB;
		}
		
		return *this;
	}
	
	void print() const {
		printf("rid=%d, dis2loc=%.4lf\n", rid, dis2loc);
	}
};

int HST_Lite_t::posDistance(int ridCur, int idxCur, int queryLocPos) {
	DSU_Row_t *rowCur = &Leaves[ridCur];
	if (rowCur->lhs<=queryLocPos && queryLocPos<rowCur->Rhs[idxCur]) return 0;
	return (queryLocPos>=rowCur->Rhs[idxCur]) ? (queryLocPos+1-rowCur->Rhs[idxCur]) : (rowCur->lhs-queryLocPos);
}

double HST_Lite_t::search_dis2QID_LB_InChild(int ridCur, int idxCur, int queryLocPos) {
	DSU_Row_t *rowCur = &Leaves[ridCur];
	if (idxCur+1 == rowCur->m) return 0.0;
	
	double ret = 0.0, rad;
	int lhs = rowCur->lhs, rhs = rowCur->Rhs[idxCur];
	DSU_Row_t *rowNxt;
	int ridNxt, idxNxt = idxCur + 1;
	
	while (lhs < rhs) {
		ridNxt = LeavesID[lhs];
		rowNxt = &Leaves[ridNxt];
		
		if (rowNxt->lhs<=queryLocPos && queryLocPos<rowNxt->Rhs[idxNxt]) {
			ridCur = ridNxt;
			idxCur = idxNxt;
			rowCur = rowNxt;
			break;
		}
		
		lhs = rowNxt->Rhs[idxNxt];
		idxNxt = 0;
	}
	if (lhs >= rhs) return 0.0;
	
	while (idxCur < rowCur->m) {
		rad = radAtLevel(rowCur->Lev[idxCur]);
		if (queryLocPos<rowCur->lhs || queryLocPos>=rowCur->Rhs[idxCur]) {
			return ret;
		} else {
			ret = rad;
		}		
		++idxCur;
	}
	
	return ret;	
}

double HST_Lite_t::search_dis2QID_LB(int ridCur, int idxCur, int queryLocPos) {
	DSU_Row_t *rowCur = &Leaves[ridCur];
	double ret = 0.0, Rad_LB;
	
	while (idxCur < rowCur->m) {
		Rad_LB = radAtLevel(rowCur->Lev[idxCur]+1);
		if (queryLocPos<rowCur->lhs || queryLocPos>=rowCur->Rhs[idxCur]) {
			return ret;
		} else {
			ret = Rad_LB;
		}		
		++idxCur;
	}
	
	return ret;
}

pair<double,double> HST_Lite_t::calc_dis2QID_LB_and_AllLB(int pRid, int pIdx, int qRid, int qIdx, int queryLocPos, double _dis2QID_LB, double _dis2QID_AllLB, double _dis2QID_LB_InChild) {
	DSU_Row_t *pRow = &Leaves[pRid];
	DSU_Row_t *qRow = &Leaves[qRid];
	double Rad_LB = radAtLevel(pRow->Lev[pIdx]+1);
	
	if (qRow->lhs<=queryLocPos && queryLocPos<qRow->Rhs[qIdx]) {
		if (qRid == pRid) {
			return make_pair(_dis2QID_LB, 0.0);
		} else {
			return make_pair(search_dis2QID_LB(qRid, qIdx, queryLocPos), 0.0);	
		}
	}
	
	double dis2QID_LB, dis2QID_AllLB;
	
	if (queryLocPos < qRow->lhs) {
		
		if (qRid == pRid) {
			
			return make_pair(_dis2QID_LB, _dis2QID_AllLB);
			
		} else if (pRow->lhs<=queryLocPos && queryLocPos<pRow->Rhs[pIdx]) {
			
			if (pRow->lhs<=queryLocPos && queryLocPos<pRow->Rhs[pIdx+1]) {
				dis2QID_LB = max(0.0, Rad_LB-_dis2QID_LB*HST_alpha);
			} else {
				dis2QID_LB = max(0.0, Rad_LB-_dis2QID_LB_InChild);
			}
			return make_pair(dis2QID_LB, dis2QID_LB);
			
		} else {
			
			return make_pair(_dis2QID_LB, _dis2QID_AllLB);
			
		}	
		
	} else {// queryLocPos >= q->rhs
		
		if (qRid == pRid) {
			
			dis2QID_LB = _dis2QID_LB;
			dis2QID_AllLB = max(_dis2QID_AllLB, dis2QID_LB-radAtLevel(qRow->Lev[qIdx]));
			
		} else if (pRow->lhs<=queryLocPos && queryLocPos<pRow->Rhs[pIdx]) {
			
			dis2QID_LB = Rad_LB;
			dis2QID_AllLB = max(_dis2QID_AllLB, dis2QID_LB-radAtLevel(qRow->Lev[qIdx]));
			
		} else {
		
			dis2QID_LB = max(_dis2QID_AllLB, _dis2QID_LB-qRow->dis2par);
			dis2QID_AllLB = max(_dis2QID_AllLB, dis2QID_LB-radAtLevel(qRow->Lev[qIdx]));
			
		}		

		return make_pair(dis2QID_LB, dis2QID_AllLB);
	}
}

void HST_Lite_t::testAll_calc_dis2QID_LB_and_AllLB() {
	for (int i=0; i<HST_nV; ++i) {
		fill(visit.begin(), visit.end(), false);
		fill(visit2.begin(), visit2.end(), false);
		test_calc_dis2QID_LB_and_AllLB(i);
	}
}

void HST_Lite_t::test_calc_dis2QID_LB_and_AllLB(int queryLocID) {
	queue<pair<pair<double,double>,pair<int,int> > > Q;
	pair<pair<double,double>,pair<int,int> > cur, tmp;
	pair<double,double> pddTmp;
	
	int queryLocPos = Leaves[queryLocID].lhs;
	cur.first.second = search_dis2QID_LB(LeavesID[0], 0, queryLocPos);
	cur.first.first = 0.0;
	cur.second.first = LeavesID[0];
	cur.second.second = 0;
	Q.push(cur);
	
	while (!Q.empty()) {
		cur = Q.front();
		Q.pop();
		int ridCur = cur.second.first, idxCur = cur.second.second, ridNxt, idxNxt;
		double dis2QID_AllLB = cur.first.first, dis2QID_LB = cur.first.second, dis2QID_LB_InChild = 0.0;
		DSU_Row_t *rowCur = &Leaves[ridCur], *rowNxt;
		
		if (idxCur+1<rowCur->m && (rowCur->lhs<=queryLocPos&&queryLocPos<rowCur->Rhs[idxCur]) && !(rowCur->lhs<=queryLocPos&&queryLocPos<rowCur->Rhs[idxCur+1])) {
			dis2QID_LB_InChild = search_dis2QID_LB_InChild(ridCur, idxCur, queryLocPos);
		} else {
			dis2QID_LB_InChild = 0.0;
		}
		
		if (idxCur+1 == rowCur->m)
			continue;
		
		ridNxt = ridCur;
		idxNxt = idxCur+1;
		int lhs = rowCur->lhs, rhs = rowCur->Rhs[idxCur];
		
		while (lhs < rhs) {
			ridNxt = LeavesID[lhs];
			rowNxt = &Leaves[ridNxt];
			
			pddTmp = calc_dis2QID_LB_and_AllLB(ridCur, idxCur, ridNxt, idxNxt, queryLocPos, dis2QID_LB, dis2QID_AllLB, dis2QID_LB_InChild);
			tmp.first.first = pddTmp.second;
			tmp.first.second = pddTmp.first;
			tmp.second.first = ridNxt;
			tmp.second.second = idxNxt;
			Q.push(tmp);
			
			lhs = rowNxt->Rhs[idxNxt];
			idxNxt = 0;
		}
	}
}


void HST_Lite_t::_DSU_RangeQuery_v1f(location_t& loc, double radius, vector<int>& answers) {
	queue<RQ_Qnode_Lite_t> Q;
	RQ_Qnode_Lite_t tmp, cur;
	double r;
	
	answers.clear();
	if (HST_nV <= 1) {
		for (int i=0; i<HST_nV; ++i) {
			if (dist(V[i], loc, tag) <= radius)
				answers.push_back(i);
		}
		return ;
	}
	
	tmp.rid = LeavesID[0];
	tmp.dis2loc = dist(V[tmp.rid], loc, tag);
	r = radAtLevel(Leaves[tmp.rid].Lev[0]);
	if (tmp.dis2loc <= r+radius)
		Q.push(tmp);	
	
	DSU_Row_t *rowCur, *rowNxt;
	int ridCur, ridNxt;
	int idxEnd;
	double dis2locMin_Local, dis2loc;
	double Rad_LB, Distor_LB, LB;
	int lhs, rhs;
	
	while (!Q.empty()) {
		cur = Q.front();
		Q.pop();
		ridCur = cur.rid;
		dis2loc = cur.dis2loc;
		rowCur = &Leaves[ridCur];
		
		dis2locMin_Local = dis2loc;
		
		if (cur.dis2loc > radius) {
			idxEnd = 1;
			while (idxEnd<rowCur->m-1 && cur.dis2loc<=radius+radAtLevel(rowCur->Lev[idxEnd])) {
				++idxEnd;
			}
		} else {
			idxEnd = rowCur->m - 2;
			while (idxEnd>0 && cur.dis2loc+radAtLevel(rowCur->Lev[idxEnd])<=radius) {
				--idxEnd;
			}
			answers.insert(answers.end(), LeavesID+rowCur->lhs, LeavesID+rowCur->Rhs[idxEnd+1]);
			++idxEnd;
		}
		
		for (--idxEnd; idxEnd>=0; --idxEnd) {
			Rad_LB = radAtLevel(rowCur->Lev[idxEnd]+1) - radius;
			Distor_LB = distAtLevel(rowCur->Lev[idxEnd])/rowCur->Distor[idxEnd] - radius;
			LB = max(Rad_LB, Distor_LB);
			lhs = rowCur->Rhs[idxEnd+1], rhs = rowCur->Rhs[idxEnd];	
	
			if (Distor_LB>dis2locMin_Local || Rad_LB>dis2loc)
				continue;	
			
			while (lhs < rhs) {
				ridNxt = LeavesID[lhs];
				rowNxt = &Leaves[ridNxt];
				r = radAtLevel(rowNxt->Lev[0]);
				
				if (rowNxt->m > 1) {// it is an internal node
					if (fabs(dis2loc-rowNxt->dis2par) <= r+radius) {
						if (dis2loc+rowNxt->dis2par+r <= radius) {
							answers.insert(answers.end(), LeavesID+rowNxt->lhs, LeavesID+rowNxt->Rhs[0]);
							double dis2loc_ = dist(V[ridNxt], loc, tag);
							dis2locMin_Local = min(dis2locMin_Local, dis2loc_);
							if (LB > dis2loc_) break;
						} else {
							tmp.dis2loc = dist(V[ridNxt], loc, tag);
							tmp.rid = ridNxt;
							if (tmp.dis2loc <= r+radius) {
								if (tmp.dis2loc+r <= radius) {
									answers.insert(answers.end(), LeavesID+rowNxt->lhs, LeavesID+rowNxt->Rhs[0]);
								} else {
									Q.push(tmp);
								}
							}
							dis2locMin_Local = min(dis2locMin_Local, tmp.dis2loc);
							if (LB > tmp.dis2loc) break;
						}
					}									
				} else {
					if (fabs(cur.dis2loc-rowNxt->dis2par) <= radius) {
						if (cur.dis2loc+rowNxt->dis2par <= radius) {
							answers.push_back(ridNxt);
						} else {
							r = dist(V[ridNxt], loc, tag);
							if (r <= radius)
								answers.push_back(ridNxt);
							dis2locMin_Local = min(dis2locMin_Local, r);
							if (LB > r) break;
						}
					}			
				}
				
				lhs = rowNxt->Rhs[0];
			}
		}
	}
}



struct PQE_t {
	int rid, idx;
	double dmin, dmax, dis2loc;
	
	PQE_t& operator=(const PQE_t& oth) {
		if (this != &oth) {
			rid = oth.rid;
			idx = oth.idx;
			dmin = oth.dmin;
			dmax = oth.dmax;
			dis2loc = oth.dis2loc;
		}
		
		return *this;
	}
	
	bool operator<(const PQE_t& oth) const {
		if (dmin == oth.dmin) {
			return (dmax==oth.dmax) ? (dis2loc<oth.dis2loc) : (dmax<oth.dmax);
		} else {
			return dmin < oth.dmin;
		}
	}
	
	bool operator>(const PQE_t& oth) const {
		if (dmin == oth.dmin) {
			return (dmax==oth.dmax) ? (dis2loc>oth.dis2loc) : (dmax>oth.dmax);
		} else {
			return dmin > oth.dmin;
		}		
	}
	
	bool operator==(const PQE_t& oth) const {
		return rid==oth.rid && idx==oth.idx;
	}
};

struct PQE_WithLB_t {
	int rid, idx;
	double dmin, dmax, dis2loc;
	double dis2QID_LB, dis2QID_AllLB;
	
	PQE_WithLB_t& operator=(const PQE_WithLB_t& oth) {
		if (this != &oth) {
			rid = oth.rid;
			idx = oth.idx;
			dmin = oth.dmin;
			dmax = oth.dmax;
			dis2loc = oth.dis2loc;
			dis2QID_LB = oth.dis2QID_LB;
			dis2QID_AllLB = oth.dis2QID_AllLB;
		}
		
		return *this;
	}
	
	bool operator<(const PQE_WithLB_t& oth) const {
		if (dmin == oth.dmin) {
			return (dmax==oth.dmax) ? (dis2loc<oth.dis2loc) : (dmax<oth.dmax);
		} else {
			return dmin < oth.dmin;
		}
	}
	
	bool operator>(const PQE_WithLB_t& oth) const {
		if (dmin == oth.dmin) {
			return (dmax==oth.dmax) ? (dis2loc>oth.dis2loc) : (dmax>oth.dmax);
		} else {
			return dmin > oth.dmin;
		}		
	}
	
	bool operator==(const PQE_WithLB_t& oth) const {
		return rid==oth.rid && idx==oth.idx;
	}
};

struct PQE_Lite_t {
	int rid;
	double dmin, dmax, dis2loc;
	
	PQE_Lite_t& operator=(const PQE_Lite_t& oth) {
		if (this != &oth) {
			rid = oth.rid;
			dmin = oth.dmin;
			dmax = oth.dmax;
			dis2loc = oth.dis2loc;
		}
		
		return *this;
	}
	
	bool operator<(const PQE_Lite_t& oth) const {
		if (dmin == oth.dmin) {
			return (dmax==oth.dmax) ? (dis2loc<oth.dis2loc) : (dmax<oth.dmax);
		} else {
			return dmin < oth.dmin;
		}
	}
	
	bool operator>(const PQE_Lite_t& oth) const {
		if (dmin == oth.dmin) {
			return (dmax==oth.dmax) ? (dis2loc>oth.dis2loc) : (dmax>oth.dmax);
		} else {
			return dmin > oth.dmin;
		}		
	}
	
	bool operator==(const PQE_Lite_t& oth) const {
		return rid == oth.rid;
	}
};

struct NNUE_t {
	double dmax;
	int id, n;
	
	NNUE_t& operator=(const NNUE_t& oth) {
		if (this != &oth) {
			dmax = oth.dmax;
			id = oth.id;
			n = oth.n;
		}
		
		return *this;
	}
	
	bool operator<(const NNUE_t& oth) const {
		if (dmax == oth.dmax) {
			return (id==oth.id) ? (n<oth.n) : (id<oth.id);
		} else {
			return dmax < oth.dmax;
		}
	}
	
	bool operator<=(const NNUE_t& oth) const {
		if (dmax == oth.dmax) {
			return (id==oth.id) ? (n<=oth.n) : (id<oth.id);
		} else {
			return dmax < oth.dmax;
		}
	}
	
	bool operator>(const NNUE_t& oth) const {
		if (dmax == oth.dmax) {
			return (id==oth.id) ? (n>oth.n) : (id>oth.id);
		} else {
			return dmax > oth.dmax;
		}
	}
	
	bool operator==(const NNUE_t& oth) const {
		return dmax==oth.dmax && id==oth.id;
	}
};

struct NNU_t {
	NNUE_t* NNUEs;
	int sz, k, kth;

	NNU_t(int kth_=0) {
		kth = kth_;
		sz = k = 0;
		if (kth_ > 0) {
			NNUEs = new NNUE_t[kth_+3];
		} else {
			NNUEs = NULL;
		}
	}
	
	~NNU_t() {
		if (NNUEs != NULL) 
			delete[] NNUEs;
	}
	
	void clear() {
		if (NNUEs != NULL) 
			delete[] NNUEs;
		sz = k = kth = 0;
	}
	
	void resize(int kth_) {
		if (NNUEs != NULL) 
			delete[] NNUEs;
		kth = kth_;
		sz = k = 0;
		if (kth_ > 0) {
			NNUEs = new NNUE_t[kth_+3];
		} else {
			NNUEs = NULL;
		}		
	}
	
	double calcDk() {
		return (k>=kth) ? NNUEs[sz-1].dmax : INF;
	}
	
	void erase(const NNUE_t& e) {
		int pos = upper_bound(NNUEs, NNUEs+sz, e) - NNUEs;
		if (pos<sz && NNUEs[pos].id==e.id) {
			k -= NNUEs[pos].n;
			while (++pos < sz) 
				NNUEs[pos-1] = NNUEs[pos];
			--sz;
		}		
	}
	
	void insert(const NNUE_t& e) {
		if (k>=kth && NNUEs[sz-1]<=e) return ;
		int pos = upper_bound(NNUEs, NNUEs+sz, e) - NNUEs;
		for (int idx=sz++; idx>pos; --idx) {
			NNUEs[idx] = NNUEs[idx-1];
		}
		NNUEs[pos] = e;
		k += e.n;
		if (k > kth) _pop_back();
	}

	void _pop_back() {
		/* some element may need to be popped out */
		for (int idx=sz-1; idx>=0; --idx) {
			if (k-NNUEs[idx].n >= kth) {
				k -= NNUEs[idx].n;
				--sz;
			} else {
				NNUEs[idx].n -= (k - kth);
				k = kth;
				break;
			}
		}		
	}
	
	void push_back(const NNUE_t& e) {
		NNUEs[sz++] = e;
		k += e.n;
	}
	
	void dumpAnswers(vector<int>& answers) {
		int pid;
		for (int i=0; i<sz; ++i) {
			if (NNUEs[i].id < 0) {
				pid = -1 - NNUEs[i].id;
				answers.push_back(pid);
			}			
		}
	}
};


void HST_Lite_t::_DSU_KnnQuery_v0f(location_t& loc, int kth, vector<int>& answers) {
	const int threshold_N = kth;
	DSU_Row_t *rowCur, *rowNxt;
	int idxCur, idxNxt, ridCur, ridNxt;
	double r, dk, dis2loc;
	priority_queue<PQE_t, vector<PQE_t>, greater<PQE_t> > PQ;
	PQE_t tmpPQE, curPQE;
	NNUE_t nnueTmp;
	NNU_t NNU(kth);

	answers.clear();
	if (kth == 0) return ;
	if (HST_nV <= kth) {
		answers.resize(HST_nV);
		for (int i=0; i<HST_nV; ++i)
			answers[i] =i;
		return ;
	}
	
	dk = _DSU_initDk(loc, kth);
	tmpPQE.dmax = dk;
	dis2loc = dist(V[LeavesID[0]], loc, tag);
	rowCur = &Leaves[LeavesID[0]];
	r = radAtLevel(rowCur->Lev[0]);
	tmpPQE.rid = LeavesID[0];
	tmpPQE.idx = 0;
	tmpPQE.dis2loc = dis2loc;
	tmpPQE.dmin = max(dis2loc-r, 0.0);
	PQ.push(tmpPQE);
	
	dk = tmpPQE.dmax;
	nnueTmp.dmax = dk;
	nnueTmp.id = tmpPQE.rid;
	nnueTmp.n = kth;
	NNU.push_back(nnueTmp);
	
	while (!PQ.empty() && PQ.top().dmin<=dk) {
		curPQE = PQ.top();
		PQ.pop();
		dis2loc = curPQE.dis2loc;
		ridCur = curPQE.rid;
		idxCur = curPQE.idx;
		rowCur = &Leaves[ridCur];
		
		// remove the bound of curPQE in NNU
		nnueTmp.dmax = curPQE.dmax;
		nnueTmp.id = curPQE.rid;
		nnueTmp.n = -1;
		NNU.erase(nnueTmp);
		
		double dis2parNxt = 0.0;
		double LB = max(radAtLevel(rowCur->Lev[idxCur]+1), distAtLevel(rowCur->Lev[idxCur])/rowCur->Distor[idxCur]);
		double BOUND = 0.0;
		int lhs = rowCur->lhs, rhs = rowCur->Rhs[idxCur];
		
		ridNxt=ridCur, idxNxt=idxCur+1, rowNxt = &Leaves[ridNxt];
		while (lhs < rhs) {
			r = radAtLevel(rowNxt->Lev[idxNxt]);
			if (fabs(dis2loc-dis2parNxt) <= r+dk) {
				if (idxNxt+1 == rowNxt->m) {// it is a leaf node
					nnueTmp.dmax = (ridCur==ridNxt) ? dis2loc : dist(V[ridNxt], loc, tag);
					if (nnueTmp.dmax <= dk) {
						nnueTmp.id = -1 - ridNxt;
						nnueTmp.n = 1;	
						NNU.insert(nnueTmp);
						dk = min(dk, NNU.calcDk());
					}
					BOUND = max(BOUND, LB-nnueTmp.dmax);
					if (BOUND > dk) break;
				} else {
					double dis2locTmp = (ridCur==ridNxt) ? dis2loc : dist(V[ridNxt], loc, tag);
					int gf_idx = idxNxt, f_idx = idxNxt + 1, idx = idxNxt + 2;
					bool skiped = dis2locTmp<=radAtLevel(rowNxt->Lev[f_idx]) && rowNxt->Rhs[f_idx]-lhs>=kth;
					double rr;
					
					while (idx < rowNxt->m) {
						rr = radAtLevel(rowNxt->Lev[idx]);
						if (dis2locTmp<=rr && rowNxt->Rhs[idx]-lhs>=kth) {
							gf_idx = f_idx;
							f_idx = idx++;
						} else {
							break;
						}
					}
					
					rr = (skiped) ? radAtLevel(rowNxt->Lev[f_idx]) : r;
					tmpPQE.dis2loc = dis2locTmp;
					tmpPQE.dmin = max(dis2locTmp-rr, BOUND);
					tmpPQE.dmax = dis2locTmp+rr;
					tmpPQE.rid = ridNxt;
					tmpPQE.idx = gf_idx;
					
					if (tmpPQE.dmin <= dk) {
						PQ.push(tmpPQE);
						
						nnueTmp.dmax = tmpPQE.dmax;
						nnueTmp.id = tmpPQE.rid;
						nnueTmp.n = rowNxt->Rhs[gf_idx] - lhs;
						if (nnueTmp.dmax < dk) {
							//// update NNU
							NNU.insert(nnueTmp);
							dk = min(dk, NNU.calcDk());
						}			
					}
					
					BOUND = max(BOUND, LB-dis2locTmp);
					if (skiped || BOUND>dk) break;
				}
			}
				
			// move to the next right sibling
			lhs = rowNxt->Rhs[idxNxt];
			if (lhs < rhs) {
				ridNxt = LeavesID[lhs];
				rowNxt = &Leaves[ridNxt];
				dis2parNxt = rowNxt->dis2par;
				idxNxt = 0;
			}	
		}
	}
	
	NNU.dumpAnswers(answers);
}

void HST_Lite_t::_DSU_KnnQuery_v0g(location_t& loc, int kth, vector<int>& answers) {
	const int threshold_N = kth;
	DSU_Row_t *rowCur, *rowNxt;
	int idxCur, idxNxt, ridCur, ridNxt;
	double r, dk, dis2loc;
	priority_queue<PQE_t, vector<PQE_t>, greater<PQE_t> > PQ;
	PQE_t tmpPQE, curPQE;
	NNUE_t nnueTmp;
	NNU_t NNU(kth);

	answers.clear();
	if (kth == 0) return ;
	if (HST_nV <= kth) {
		answers.resize(HST_nV);
		for (int i=0; i<HST_nV; ++i)
			answers[i] =i;
		return ;
	}
	
	dis2loc = dist(V[LeavesID[0]], loc, tag);
	rowCur = &Leaves[LeavesID[0]];
	r = radAtLevel(rowCur->Lev[0]);
	tmpPQE.rid = LeavesID[0];
	tmpPQE.idx = 0;
	tmpPQE.dis2loc = dis2loc;
	tmpPQE.dmin = max(dis2loc-r, 0.0);
	tmpPQE.dmax = dis2loc+r;
	PQ.push(tmpPQE);
	
	dk = tmpPQE.dmax;
	nnueTmp.dmax = dk;
	nnueTmp.id = tmpPQE.rid;
	nnueTmp.n = kth;
	NNU.push_back(nnueTmp);
	
	while (!PQ.empty() && PQ.top().dmin<=dk) {
		curPQE = PQ.top();
		PQ.pop();
		dis2loc = curPQE.dis2loc;
		ridCur = curPQE.rid;
		idxCur = curPQE.idx;
		rowCur = &Leaves[ridCur];
		
		// remove the bound of curPQE in NNU
		nnueTmp.dmax = curPQE.dmax;
		nnueTmp.id = curPQE.rid;
		nnueTmp.n = -1;
		NNU.erase(nnueTmp);
		
		double dis2parNxt = 0.0;
		double LB = max(radAtLevel(rowCur->Lev[idxCur]+1), distAtLevel(rowCur->Lev[idxCur])/rowCur->Distor[idxCur]);
		double BOUND = 0.0;
		int lhs = rowCur->lhs, rhs = rowCur->Rhs[idxCur];
		
		ridNxt=ridCur, idxNxt=idxCur+1, rowNxt = &Leaves[ridNxt];
		while (lhs < rhs) {
			r = radAtLevel(rowNxt->Lev[idxNxt]);
			if (fabs(dis2loc-dis2parNxt) <= r+dk) {
				if (idxNxt+1 == rowNxt->m) {// it is a leaf node
					nnueTmp.dmax = (ridCur==ridNxt) ? dis2loc : dist(V[ridNxt], loc, tag);
					if (nnueTmp.dmax <= dk) {
						nnueTmp.id = -1 - ridNxt;
						nnueTmp.n = 1;	
						NNU.insert(nnueTmp);
						dk = min(dk, NNU.calcDk());
					}
					BOUND = max(BOUND, LB-nnueTmp.dmax);
					if (BOUND > dk) break;
				} else {
					double dis2locTmp = (ridCur==ridNxt) ? dis2loc : dist(V[ridNxt], loc, tag);
					int gf_idx = idxNxt, f_idx = idxNxt + 1, idx = idxNxt + 2;
					bool skiped = dis2locTmp<=radAtLevel(rowNxt->Lev[f_idx]) && rowNxt->Rhs[f_idx]-lhs>=kth;
					double rr;
					
					while (idx < rowNxt->m) {
						rr = radAtLevel(rowNxt->Lev[idx]);
						if (dis2locTmp<=rr && rowNxt->Rhs[idx]-lhs>=kth) {
							gf_idx = f_idx;
							f_idx = idx++;
						} else {
							break;
						}
					}
					
					rr = (skiped) ? radAtLevel(rowNxt->Lev[f_idx]) : r;
					tmpPQE.dis2loc = dis2locTmp;
					tmpPQE.dmin = max(dis2locTmp-rr, BOUND);
					tmpPQE.dmax = dis2locTmp+rr;
					tmpPQE.rid = ridNxt;
					tmpPQE.idx = gf_idx;
					
					if (tmpPQE.dmin <= dk) {
						PQ.push(tmpPQE);
						
						nnueTmp.dmax = tmpPQE.dmax;
						nnueTmp.id = tmpPQE.rid;
						nnueTmp.n = rowNxt->Rhs[gf_idx] - lhs;
						if (nnueTmp.dmax < dk) {
							//// update NNU
							NNU.insert(nnueTmp);
							dk = min(dk, NNU.calcDk());
						}			
					}
					
					BOUND = max(BOUND, LB-dis2locTmp);
					if (skiped || BOUND>dk) break;
				}
			}
				
			// move to the next right sibling
			lhs = rowNxt->Rhs[idxNxt];
			if (lhs < rhs) {
				ridNxt = LeavesID[lhs];
				rowNxt = &Leaves[ridNxt];
				dis2parNxt = rowNxt->dis2par;
				idxNxt = 0;
			}	
		}
	}
	
	NNU.dumpAnswers(answers);
}

double HST_Lite_t::_DSU_initDk_LinearScan(location_t& loc, int kth) {
	const int threshold = kth;
	int ridCur = LeavesID[_queryLocPos], ridNxt;
	int idxCur = Leaves[ridCur].m - 1, idxNxt;
	int lhs, rhs;
	
	while (true) {
		ridNxt = (idxCur==0) ? Leaves[ridCur].nomIdx : ridCur;
		idxNxt = (idxCur==0) ? (Leaves[ridNxt].m-1) : (idxCur-1);
		if (Leaves[ridNxt].Rhs[idxNxt]-Leaves[ridNxt].lhs > threshold) {
			if (ridNxt == ridCur) {
				lhs = Leaves[ridCur].lhs;
				rhs = lhs + threshold;
			} else {
				int delta = (threshold+1 - (Leaves[ridCur].Rhs[idxCur]-Leaves[ridCur].lhs)) >> 1;
				if (Leaves[ridCur].lhs-delta <= Leaves[ridNxt].lhs) {
					lhs = max(Leaves[ridCur].lhs-delta, Leaves[ridNxt].lhs);
					delta = max(0, threshold-(Leaves[ridCur].Rhs[idxCur]-lhs));
					rhs = min(Leaves[ridNxt].Rhs[idxNxt], Leaves[ridCur].Rhs[idxCur]+delta);
				} else {
					rhs = min(Leaves[ridNxt].Rhs[idxNxt], Leaves[ridCur].Rhs[idxCur]+delta);
					delta = max(0, threshold-(rhs-Leaves[ridCur].lhs));
					lhs = max(Leaves[ridCur].lhs-delta, Leaves[ridNxt].lhs);
				}
			}
			break;
		}
		ridCur = ridNxt;
		idxCur = idxNxt;
	}
	
	vector<double> vtmp(rhs-lhs);
	for (int i=lhs; i<rhs; ++i) {
		vtmp[i-lhs] = dist(V[LeavesID[i]], loc, tag);
	}
	nth_element(vtmp.begin(), vtmp.begin()+(kth-1), vtmp.end());
	distCnt = 0;
	return vtmp[kth-1];
}

double HST_Lite_t::_DSU_initDk_ScanOnce(location_t& loc, int kth) {
	int threshold = kth;
	int ridCur = LeavesID[_queryLocPos], ridNxt, _ridNxt;
	int idxCur = Leaves[ridCur].m - 1, idxNxt, _idxCur;
	double mapDis, coverRad;
	// int lhs, rhs;
	bool flag = false;
	
	while (true) {
		_idxCur = idxCur, _ridNxt = ridNxt;
		ridNxt = (idxCur==0) ? Leaves[ridCur].nomIdx : ridCur;
		idxNxt = (idxCur==0) ? (Leaves[ridNxt].m-1) : (idxCur-1);
		if (Leaves[ridNxt].Rhs[idxNxt]-Leaves[ridNxt].lhs >= threshold) {
			if (!flag) {
				return dist(V[ridNxt],loc,tag)+radAtLevel(Leaves[ridNxt].Lev[idxNxt]+1.0);
			} else {
				mapDis = dist(V[ridCur], loc, tag);
				return mapDis + coverRad + radAtLevel(Leaves[ridNxt].Lev[idxNxt]);
			}
		}
		ridCur = ridNxt;
		idxCur = idxNxt;
	}
	
	return -1.0;
}

double HST_Lite_t::_initDk_ScanOnce(location_t& loc, int queryLocPos, int kth, int& oid, double& dis2loc) {
	const int threshold = kth;
	int ridCur = LeavesID[queryLocPos], ridNxt;
	int idxCur = Leaves[ridCur].m - 1, idxNxt;
	
	while (true) {
		ridNxt = (idxCur==0) ? Leaves[ridCur].nomIdx : ridCur;
		if (idxCur > 0) {
			idxNxt = idxCur - 1;
		} else {
			idxNxt = 0;
			DSU_Row_t* row = &Leaves[ridNxt];
			for (int i=row->m-1; i>0; --i) {
				if (Leaves[ridCur].lhs < row->Rhs[i-1]) {
					idxNxt = i-1;
					break;
				}
			}
		}
		if (Leaves[ridNxt].Rhs[idxNxt]-Leaves[ridNxt].lhs >= threshold) {
			oid = ridNxt;
			dis2loc = dist(V[ridNxt],loc,tag);
			return dis2loc + radAtLevel(Leaves[ridNxt].Lev[idxNxt]);
		}
		ridCur = ridNxt;
		idxCur = idxNxt;
	}
	
	return INF;	
}
