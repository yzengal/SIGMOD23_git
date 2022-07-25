#ifndef _SPBTREE_H_
#define _SPBTREE_H_

#include <bits/stdc++.h>
using namespace std;

#include "global.h"
#include "btree.h"
#include "btree_multimap.h"
#include "hilbert4.h"

double local_dist(location_t& va, location_t& vb) {
	return dist(va, vb, tag);
}


struct SPBtree_t {
	typedef stx::btree_multimap<unsigned long long, int, less<unsigned long long> > btree_type;
	
	btree_type bt;
	int n;
	static location_t* loc;
	static vector<int> mark;
	static vector<int> pivots;
	static vector<unsigned long long> keys;
	static vector<pair<unsigned long long,int> > data;
	static vector<int> PHI_q;
	static int RRqr[PIVOT_N][2];
	static int queryID, queryKth;
	static double queryRadius;
	
	~SPBtree_t() {
		removeFromFile(bt.getPageNum());
	}
	
	long long getPageAccess() {
		return bt.getPageAccess();
	}
	
	vector<int> PHI(location_t& la, vector<int>& pivots, int pivot_n) {
		vector<int> ret(pivot_n);
		
		for (int i=0; i<pivot_n; ++i) {
			ret[i] = local_dist(la, loc[pivots[i]]);
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
	
	void bulk_load(location_t* _loc, int _n) {
		loc = _loc;
		n = _n;
		data.resize(n);
		keys.resize(n);
		mark.resize(n);
		HFI(PIVOT_N, pivots);
		unsigned long long coord[PIVOT_N];
		for (int i=0; i<n; ++i) {
			vector<int> coordTmp = PHI(loc[i], pivots, PIVOT_N);
			for (int j=0; j<PIVOT_N; ++j) {
				coord[j] = coordTmp[j]; 
			}
			data[i].first = keys[i] = hilbert_c2i(PIVOT_N, SFC_nBits, coord);
			data[i].second = i;
		}
		sort(data.begin(), data.end());
		bt.bulk_load(data.begin(), data.end());
		bt.init_Key_Minmax();
		bt.initDisk();
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
		unsigned long long coord[PIVOT_N];
		hilbert_i2c(PIVOT_N, SFC_nBits, keys[pid], coord);
		reverse(coord, coord+PIVOT_N);
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
	
	void mapQueryRange(unsigned long long& key_min, unsigned long long& key_max) {
		unsigned long long SFC_value;
		
		key_min = numeric_limits<unsigned long long>::max();
		key_max = numeric_limits<unsigned long long>::min();
		unsigned long long status_N = (1ULL<<(PIVOT_N-1));
		unsigned long long coord[PIVOT_N];
		for (int i=0; i<PIVOT_N; ++i) {
			for (unsigned long long status=0; status<status_N; ++status) {
				unsigned long long tmp = status;
				for (int j=0; j<PIVOT_N; ++j) {
					if (j == i) continue;
					coord[j] = RRqr[j][tmp&1];
					tmp >>= 1;
				}
				for (coord[i]=RRqr[i][0]; coord[i]<=RRqr[i][1]; ++coord[i]) {
					SFC_value = hilbert_c2i(PIVOT_N, SFC_nBits, coord);
					key_min = min(key_min, SFC_value);
					key_max = max(key_max, SFC_value);
				}
			}
		}
	} 
	
	void RangeQuery(int qid, double radius, vector<int>& answers) {
		queryID = qid, queryRadius = radius;
		PHI_q = PHI(loc[qid], pivots, PIVOT_N);
		for (int i=0; i<PIVOT_N; ++i) {
			RRqr[i][0] = PHI_q[i] - (radius+2); // +2 is used to handle rounding of double type
			if (RRqr[i][0] < 0) RRqr[i][0] = 0;
			RRqr[i][1] = PHI_q[i] + (radius+2); // +2 is used to handle rounding of double type
			if (RRqr[i][1] > GRID_L) RRqr[i][1] = GRID_L;
		}
		bt.rangeQuery(loc[qid], radius, answers, RRqr);
	}
	
	static double MIND(int mbr[][2]) {
		/// check whether the query point is covered by mbr, if yes, MIND = 0.0;
		bool flag = true;
		for (int i=0; flag&&i<PIVOT_N; ++i) {
			long long coord_q = PHI_q[i];
			if (coord_q<mbr[i][0] || coord_q>mbr[i][1])
				flag = false;
		}
		if (flag) return 0.0;
		
		double ret = 1e20;
		
		/// check the corners
		unsigned long long status_N = (1ULL<<PIVOT_N);
		long long coord[PIVOT_N];
		for (unsigned long long status=0; status<status_N; ++status) {
			unsigned long long tmp = status;
			long long disTmp = 0;
			for (int i=0; i<PIVOT_N; ++i,tmp>>=1) {
				coord[i] = mbr[i][tmp&1];
				long long coord_q = PHI_q[i];
				disTmp = max(disTmp, abs(coord_q - coord[i]));
			}
			if (disTmp < ret) ret = disTmp;
		}

		/// check the inner project point
		long long coord_[PIVOT_N];
		for (int i=0; i<PIVOT_N; ++i) coord[i] = coord_[i] = PHI_q[i];
		
		for (int i=0; i<PIVOT_N; ++i) {
			for (int j=0; j<2; ++j) {
				coord_[i] = mbr[i][j];
				bool flag = true;
				long long disTmp = 0;
				for (int k=0; flag&&k<PIVOT_N; ++k) {
					if (coord_[k]<mbr[k][0] || coord_[k]>mbr[k][1])
						flag = false;
					disTmp = max(disTmp, abs(coord_[k] - coord[k]));
				}
				if (flag && disTmp<ret) {
					ret = disTmp;
				}
			}
			coord_[i] = coord[i];
		}
		
		return ret;
	}
	
	static double DistKQ(int qid) {
		return local_dist(loc[qid], loc[queryID]);
	}
	
	void KnnQuery(int qid, int kth, vector<int>& answers) {
		queryID = qid, queryKth = kth;
		PHI_q = PHI(loc[qid], pivots, PIVOT_N);
		bt.knnQuery(loc[qid], kth, answers, MIND);
	}
	
	int countTree() {
		btree_type::tree_stats m_stats = bt.get_stats();
		return m_stats.leaves +  m_stats.innernodes;
	}
	
	double memoryTree() {
		btree_type::tree_stats m_stats = bt.get_stats();
		double n_leaf = m_stats.leaves;
		double n_inner = m_stats.innernodes;
		double n_node = n_leaf + n_inner;
		
		double ret = 0.0;
		
		ret += sizeof(short) * 2.0 * n_node;
		ret += sizeof(unsigned long long) * 2.0 * n_node;
		ret += sizeof(int) * 2.0 * PIVOT_N * n_node;
		
		ret += 1.0 * sizeof(unsigned long long) * bt.innerslotmax * n_inner;
		ret += 1.0 * sizeof(int*) * (1+bt.innerslotmax) * n_inner;
		
		ret += 2.0 * sizeof(int*) * n_leaf;
		ret += 1.0 * sizeof(unsigned long long) * bt.leafslotmax * n_leaf;
		ret += 1.0 * sizeof(int) * bt.leafslotmax * n_leaf;
		ret += 1.0 * (sizeof(unsigned long long)+sizeof(int)) * n;
		
		return ret;
	}
};

location_t* SPBtree_t::loc;
vector<int> SPBtree_t::mark;
vector<int> SPBtree_t::pivots;
vector<unsigned long long> SPBtree_t::keys;
vector<pair<unsigned long long,int> > SPBtree_t::data;
vector<int> SPBtree_t::PHI_q;
int SPBtree_t::RRqr[PIVOT_N][2];
int SPBtree_t::queryID;
int SPBtree_t::queryKth;
double SPBtree_t::queryRadius;

#endif
