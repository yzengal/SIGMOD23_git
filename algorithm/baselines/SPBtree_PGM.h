#ifndef _SPBTREE_PGM_H_
#define _SPBTREE_PGM_H_

#include <bits/stdc++.h>
using namespace std;

#define PIVOT_N 5
#define PGM_EPSILON 32 // space-time trade-off parameter

#include "pgm/pgm_index_variants.hpp"
typedef uint64_t tuple_type;
#ifndef PIVOT_N
	typedef tuple<tuple_type,tuple_type,tuple_type> tuple2_t;
#elif PIVOT_N==2
	typedef tuple<tuple_type,tuple_type,tuple_type> tuple2_t;
#elif PIVOT_N==3
	typedef tuple<tuple_type,tuple_type,tuple_type,tuple_type> tuple2_t;
#elif PIVOT_N==5
	typedef tuple<tuple_type,tuple_type,tuple_type,tuple_type,tuple_type,tuple_type> tuple2_t;
	// typedef tuple<tuple_type,tuple_type,tuple_type,tuple_type,tuple_type> tuple2_t;
#else
	typedef tuple<tuple_type,tuple_type,tuple_type> tuple2_t;
#endif


double local_dist(location_t& va, location_t& vb) {
	return dist(va, vb, tag);
}


struct SPBtree_t {
    pgm::MultidimensionalPGMIndex<PIVOT_N+1, tuple_type, PGM_EPSILON> pgm_3d;
	
	int n;
	static location_t* loc;
	static vector<int> mark;
	static vector<int> pivots;
	static vector<tuple2_t> data;
	static vector<tuple_type> PHI_q;
	static int RRqr[PIVOT_N][2];
	static int queryID, queryKth;
	static double queryRadius;
	
	vector<tuple_type> PHI(location_t& la, vector<int>& pivots, int pivot_n) {
		vector<tuple_type> ret(pivot_n);
		
		for (int i=0; i<pivot_n; ++i) {
			ret[i] = local_dist(la, loc[pivots[i]]);
		}
		
		return ret;
	}
	
	int distLB(location_t& la, location_t& lb, vector<int>& pivots, int pivot_n) {
		vector<tuple_type> phiA = PHI(la, pivots, pivot_n);
		vector<tuple_type> phiB = PHI(lb, pivots, pivot_n);
		int ret = 0;
		
		for (int i=0; i<pivot_n; ++i) {
			int tmpa = phiA[i], tmpb = phiB[i];
			ret = max(ret, abs(tmpa-tmpb));
		}
		
		return ret;
	}
	
	void bulk_load(location_t* _loc, int _n) {
		loc = _loc;
		n = _n;
		data.resize(n);
		mark.resize(n);
		HFI(PIVOT_N, pivots);
		unsigned long long coord[PIVOT_N];
		for (int i=0; i<n; ++i) {
			vector<tuple_type> coordTmp = PHI(loc[i], pivots, PIVOT_N);
#ifndef PIVOT_N
			data[i] = make_tuple((tuple_type)coordTmp[0], (tuple_type)coordTmp[1], (tuple_type)i);
#elif PIVOT_N==2
			data[i] = make_tuple((tuple_type)coordTmp[0], (tuple_type)coordTmp[1], (tuple_type)i);
#elif PIVOT_N==3
			data[i] = make_tuple((tuple_type)coordTmp[0], (tuple_type)coordTmp[1], (tuple_type)coordTmp[2], (tuple_type)i);
#elif PIVOT_N==5
			data[i] = make_tuple((tuple_type)coordTmp[0], (tuple_type)coordTmp[1], (tuple_type)coordTmp[2], (tuple_type)coordTmp[3], (tuple_type)coordTmp[4], (tuple_type)i);
			// data[i] = make_tuple((tuple_type)coordTmp[0], (tuple_type)coordTmp[1], (tuple_type)coordTmp[2], (tuple_type)coordTmp[3], (tuple_type)coordTmp[4]);
#else
			data[i] = make_tuple((tuple_type)coordTmp[0], (tuple_type)coordTmp[1], (tuple_type)i);
#endif
		}
		pgm_3d = pgm::MultidimensionalPGMIndex<PIVOT_N+1, tuple_type, PGM_EPSILON>(data.begin(), data.end());
	}
	
	void HFI(int m, vector<int>& pivots) {
		pivots.clear();
		if (n <= m) {
			for (int i=0; i<n; ++i)
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
						double disTmp = local_dist(loc[cand[pid]], loc[cand[qid]]);
						double disLBTmp = distLB(loc[cand[pid]], loc[cand[qid]], pivots, k+1);
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
		if (n <= m) {
			for (int i=0; i<n; ++i)
				pivots.push_back(i);
			return ;
		}
		
		// randomly select si in S
		int pid = rand() % n, qid = pid;
		double disMax = 0.0, disTmp;
		
		// find f1
		for (int i=0; i<n; ++i) {
			disTmp = local_dist(loc[i], loc[pid]);
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
		fill(mark.begin(), mark.end(), 0);
		pid = qid, disMax = 0.0;
		mark[pid] = 1;
		for (int i=0; i<n; ++i) {
			if (mark[i]) continue;
			disTmp = local_dist(loc[i], loc[pid]);
			if (disTmp > disMax) {
				disMax = disTmp;
				qid = i;
			}
		}	
		
		// compute other pivots
		pivots.push_back(pid);
		pivots.push_back(qid);
		m -= 2;
		mark[qid] = 1;
		double edge = local_dist(loc[pid], loc[qid]);
		while (m-- > 0) {
			double errorMin = 1e20, errorTmp = 0.0;
			pid = -1;
			for (int si=0; si<n; ++si) {
				if (mark[si]) continue;
				errorTmp = 0.0;
				for (int g=0; g<pivots.size(); ++g) {
					errorTmp += fabs(edge - local_dist(loc[pivots[g]], loc[si]));
				}
				if (errorTmp < errorMin) {
					errorMin = errorTmp;
					pid = si;
				}
			}
			assert(pid>=0 && pid<n);
			pivots.push_back(pid);
			mark[pid] = 1;
		}
		
		fill(mark.begin(), mark.end(), 0);
	}
	
	static bool verifyRQ(int pid) {
		tuple_type coord[PIVOT_N], pid_;
#ifndef PIVOT_N
		std::tie(coord[0], coord[1], pid_) = data[pid];
#elif PIVOT_N==2
		std::tie(coord[0], coord[1], pid_) = data[pid];
#elif PIVOT_N==3
		std::tie(coord[0], coord[1], coord[2], pid_) = data[pid];
#elif PIVOT_N==5
		std::tie(coord[0], coord[1], coord[2], coord[3], coord[4], pid_) = data[pid];
		// std::tie(coord[0], coord[1], coord[2], coord[3], coord[4]) = data[pid];
#else
		std::tie(coord[0], coord[1], pid_) = data[pid];
#endif
		bool flag = true;
		for (int i=0; flag&&i<PIVOT_N; ++i) {
			flag &= (coord[i]>=RRqr[i][0] && coord[i]<=RRqr[i][1]);
		}
		for (int i=0; flag&&i<PIVOT_N; ++i) {
			if (coord[i] <= queryRadius-PHI_q[i])
				return true;
		}
		return flag && local_dist(loc[pid], loc[queryID])<=queryRadius;
	}
	
	void RangeQuery(int qid, int radius, vector<int>& answers) {
		queryID = qid, queryRadius = radius;
		PHI_q = PHI(loc[qid], pivots, PIVOT_N);
		for (int i=0; i<PIVOT_N; ++i) {
			RRqr[i][0] = (int)PHI_q[i] - (radius+3); // +2 is used to handle rounding of double type
			if (RRqr[i][0] < 0) RRqr[i][0] = 0;
			RRqr[i][1] = PHI_q[i] + (radius+3); // +2 is used to handle rounding of double type
			// if (RRqr[i][1] > GRID_L) RRqr[i][1] = GRID_L;
		}
#ifndef PIVOT_N
		tuple2_t min_range = make_tuple((tuple_type)RRqr[0][0], (tuple_type)RRqr[1][0], (tuple_type)0);
		tuple2_t max_range = make_tuple((tuple_type)RRqr[0][1], (tuple_type)RRqr[1][1], (tuple_type)n);	
#elif PIVOT_N==2
		tuple2_t min_range = make_tuple((tuple_type)RRqr[0][0], (tuple_type)RRqr[1][0], (tuple_type)0);
		tuple2_t max_range = make_tuple((tuple_type)RRqr[0][1], (tuple_type)RRqr[1][1], (tuple_type)n);
#elif PIVOT_N==3
		tuple2_t min_range = make_tuple((tuple_type)RRqr[0][0], (tuple_type)RRqr[1][0], (tuple_type)RRqr[2][0], (tuple_type)0);
		tuple2_t max_range = make_tuple((tuple_type)RRqr[0][1], (tuple_type)RRqr[1][1], (tuple_type)RRqr[2][1], (tuple_type)n);		
#elif PIVOT_N==5
		tuple2_t min_range = make_tuple((tuple_type)RRqr[0][0], (tuple_type)RRqr[1][0], (tuple_type)RRqr[2][0], (tuple_type)RRqr[3][0], (tuple_type)RRqr[4][0], (tuple_type)0);
		tuple2_t max_range = make_tuple((tuple_type)RRqr[0][1], (tuple_type)RRqr[1][1], (tuple_type)RRqr[2][1], (tuple_type)RRqr[3][1], (tuple_type)RRqr[4][1], (tuple_type)n);
		// tuple2_t min_range = make_tuple((tuple_type)RRqr[0][0], (tuple_type)RRqr[1][0], (tuple_type)RRqr[2][0], (tuple_type)RRqr[3][0], (tuple_type)RRqr[4][0]);
		// tuple2_t max_range = make_tuple((tuple_type)RRqr[0][1], (tuple_type)RRqr[1][1], (tuple_type)RRqr[2][1], (tuple_type)RRqr[3][1], (tuple_type)RRqr[4][1]);
#else
		tuple2_t min_range = make_tuple((tuple_type)RRqr[0][0], (tuple_type)RRqr[1][0], (tuple_type)0);
		tuple2_t max_range = make_tuple((tuple_type)RRqr[0][1], (tuple_type)RRqr[1][1], (tuple_type)n);	
#endif	

		answers.clear();
		int pid = 0;
		for (auto it = pgm_3d.range(min_range, max_range); it != pgm_3d.end(); ++it) {
			pid = std::get<PIVOT_N>(*it);
			if (verifyRQ(pid))
				answers.push_back(pid);
		}
	}
	
	void KnnQuery(int qid, int kth, vector<int>& answers) {
		answers.clear();
		if (kth == 0) return ;
		
		queryID = qid, queryKth = kth;
		int radius = -1;
		do {
			++radius;
			RangeQuery(qid, radius, answers);
		} while (answers.size() < kth);
		
		priority_queue<pair<double,int>, vector<pair<double,int> >, less<pair<double,int> > > Q;
		for (int i=0; i<answers.size(); ++i) {
			int pid = answers[i];
			double disTmp = local_dist(loc[pid], loc[qid]);
			if (Q.size() < kth) {
				Q.push(make_pair(disTmp, pid));
			} else if (disTmp < Q.top().first) {
				Q.pop();
				Q.push(make_pair(disTmp, pid));
			}
		}
		answers.clear();
		while (!Q.empty()) {
			answers.push_back(Q.top().second);
			Q.pop();
		}
		reverse(answers.begin(), answers.end());
	}
	
	int countTree() {
		return 0.0;
	}
	
	double memoryTree() {
		double ret = pgm_3d.size_in_bytes();
		
		ret += 1.0 * sizeof(tuple_type) * (PIVOT_N+1) * n;
		
		return ret;
	}
};

location_t* SPBtree_t::loc;
vector<int> SPBtree_t::mark;
vector<int> SPBtree_t::pivots;
vector<tuple2_t> SPBtree_t::data;
vector<tuple_type> SPBtree_t::PHI_q;
int SPBtree_t::RRqr[PIVOT_N][2];
int SPBtree_t::queryID;
int SPBtree_t::queryKth;
double SPBtree_t::queryRadius;

#endif
