
#include "LiteHST_BASE.h"
#include "global.h"
#ifdef WATCH_MEM
#include "monitor.h"
#endif

const int MAX_HEIGHT = 55;
int H = 0;
int alpha = 2;
double logAlpha = 1.0;
int* pi = NULL;
int* reverse_pi = NULL;
double dmax = -1.0;
double* expks = NULL;
double* sumks = NULL;
Node_t* rt = NULL;
Node_t** leaves = NULL;
double beta = 1.0;
double distortion = 1.0;

inline double log2(double x) {
	return log(x) / logAlpha;
}

inline double pow2(int i) {
	return expks[i];
}

int whichLevel(double l) {
	double radius = pow2(H)*beta;
	for (int lev=1; lev<=H+1; ++lev,radius/=alpha) {
		if (l > radius) return lev-1;
	}
	return 1;
}

void dumpNode(Node_t* p) {
	for (int i=1; i<p->lev; ++i)
		putchar(' ');
	if (p->child.size() == 0)
		printf("nomIdx=%d\n", p->nomIdx);
	else
		printf("nid=%d, nomIdx=%d, wei=%.4lf, |child|=%d\n", p->nid, p->nomIdx, p->wei, (int)p->child.size());
}

void dumpTree(Node_t* rt) {
	if (rt != NULL) {
		dumpNode(rt);
		for (int i=0; i<rt->child.size(); ++i)
			dumpTree(rt->child[i]);
	}
}

int countTree(Node_t* rt) {
	int ret = 0;
	if (rt != NULL) {
		++ret;
		rt->rad = (rt->child.size()==0) ? 1.0 : 0.0;
		for (int i=0; i<rt->child.size(); ++i) {
			ret += countTree(rt->child[i]);
			rt->rad += rt->child[i]->rad;
		}
	}
	return ret;
}

double memoryTree(Node_t* rt) {
	double ret = 0;
	if (rt != NULL) {
		ret += rt->getMemCost();
		for (int i=0; i<rt->child.size(); ++i)
			ret += memoryTree(rt->child[i]);
	}
	return ret;	
}

inline int getLevel(double H, double dist) {
	if (dist < 1.0) return H+1;
	
	int k = ceil(log2(dist/beta));
	
	if (expks[k]*beta == dist)
		++k;
	
	return H+1-k;
}

void initMemory_HST(int n) {
	// srand(time(NULL));
	nV = n;
	pi = new int[nV];
	reverse_pi = new int[nV];
	expks = new double[MAX_HEIGHT];
	expks[0] = 1.0;
	for (int i=1; i<MAX_HEIGHT; ++i)
		expks[i] = expks[i-1] * alpha;
	
	sumks = new double[MAX_HEIGHT+1];
	sumks[0] = 0.0;
	for (int i=1; i<=MAX_HEIGHT; ++i)
		sumks[i] = sumks[i-1] + expks[i];
	
	leaves = new Node_t*[nV];
	
	logAlpha = log(alpha);
}

void freeMemory_HST() {
	delete[] pi;
	delete[] reverse_pi;
	delete[] expks;
	delete[] sumks;
    delete[] leaves;
	freeHST(rt);
}

void freeHST(Node_t*& rt) {
	if (rt == NULL) return ;
	
	Node_t* chi;
	
	for (int i=0; i<rt->child.size(); ++i) {
		chi = rt->child[i];
		freeHST(chi);
	}
	
	delete rt;
	rt = NULL;
}

void initVariable() {
	alpha = 2;
	int base = alpha*10000;
	beta = (rand()%(base-10000)+10000) * 1.0 / base;
	beta = min(beta, 1.0);
	beta = max(1.0/alpha, beta);
}

void initVariable(string &fileName) {
	ifstream fin(fileName.c_str(), ios::in);   
    
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", fileName.c_str());
		exit(1);
	}
	
	fin >> beta >> alpha;
    fin.close();	
}

void randomization() {
	for (int i=0; i<nV; ++i){
		pi[i] = i;
	}
	random_shuffle(pi, pi+nV);
	for (int i=0; i<nV; ++i){
		reverse_pi[pi[i]] = i;
	}
}

inline void addChild(Node_t* far, Node_t* u) {
	if (far != NULL) {
		u->far = far;
		u->cid = far->child.size();
		far->child.push_back(u);
	}
}

inline void mergeChild(Node_t* far, Node_t* u) {
	if (far->child.size() != 1) 
		return ;
	
	if (far != rt) {
		Node_t* gfar = far->far;
		int gcid = far->cid;
		
		u->far = gfar;
		u->cid = gcid;
		u->wei += far->wei;
		u->lev = far->lev;
		u->dis2cp = far->dis2cp;
		u->nomIdx = far->nomIdx;
		u->distortion = far->distortion;
		
		gfar->child[gcid] = u;
		delete far;
	} else {
		u->far = NULL;
		u->cid = far->cid;
		delete far;
		rt = u;
	}
}

double distAtLevel(int lev) {
	if (lev >= H+1) return 0.0;
	if (lev <= 1) lev = 1;
	return sumks[H-lev+1] * beta * 2.0;

}

double distOnHST(int u, int v) {
	int level = levelOfLCA(u, v);
	return distAtLevel(level);
}

double distOnHST(Node_t* u, Node_t* v) {
	int level = levelOfLCA(u, v);
	return distAtLevel(level);
}

int levelOfLCA(int u, int v) {
	return levelOfLCA(leaves[u], leaves[v]);
}

int levelOfLCA(Node_t* u, Node_t* v) {
	if (u==NULL || v==NULL)
		return -1;
	
	while (u!=NULL && v!=NULL && u!=v) {
		if (u->lev > v->lev) {
			u = u->far;
		} else {
			v = v->far;
		}
	}
	
	if (u==NULL || v==NULL)
		return -1;
	
	return u->lev;
}

pair<Node_t*,int> getLCA(int u, int v) {
	return getLCA(leaves[u], leaves[v]);
}

pair<Node_t*,int> getLCA(Node_t* u, Node_t* v) {
	if (u==NULL || v==NULL)
        return make_pair(rt, -1);
	
	while (u!=NULL && v!=NULL && u!=v) {
		if (u->lev > v->lev) {
			u = u->far;
		} else {
			v = v->far;
		}
	}
	
	if (u==NULL || v==NULL)
		return make_pair(rt, -1);
	
	return make_pair(u, u->lev);
}

void calcDmax(string& filename) {
	if (tag == 0) {
		dmax = 610;
	} else if (tag == 1) {
		 if (filename[1] == 'n') {
			dmax = DIM_K+0.5;
		} else if (filename[1] == 'y') {
			dmax = DIM_K+0.5;
		} else {
			dmax = DIM_K+0.5;
		}
	} else if (tag == 2) {
		dmax = DIM_K+0.5;
		dmax = 0;
		for (int i=0; i<nV; ++i) {
			for (int j=0; j<i; ++j)
				dmax = max(dmax, dist(V, i, j, tag));
		}
		cout << "dmax: " << dmax << endl;
	} else {
		// initialize the parameters for decomposition
		dmax = 0;
		for (int i=0; i<nV; ++i) {
			dmax = max(dmax, dist(V, i, pi[0], tag));
		}
		dmax += dmax;
	}
}

void constructHST(string& filename) {	
	// label the far[][] with node ID
	int nid = 1;
	Node_t** nodes = NULL;
	Node_t** nodes_ = NULL; 
	bool* visit = NULL;
	
	nodes = new Node_t*[nV];
	nodes_ = new Node_t*[nV];
	visit = new bool[nV];
	
	if (rt != NULL)
		freeHST(rt);
	randomization();

	// Prunning 1: calcDmax()
	calcDmax(filename);
	
	// initialization
	H = ceil(log2(dmax+EPS)) + 1;	
	double radius = pow2(H)*beta;
	const int threshold_n = 0;// ceil(log2(nV));
	
	// construct the root
	rt = new Node_t(nid++, pi[0], 1, NULL, radius);
	rt->dis2cp = 0.0;
	for (int i=0; i<nV; ++i) {
        nodes_[i] = rt;
	}
	
	vector<vector<int> > preC, curC;
	vector<int> vtmp;
	for (int j=0; j<nV; ++j)
		vtmp.push_back(pi[j]);
	preC.push_back(vtmp);
	
	vector<pair<int,double> > DISTs(nV, make_pair(-1,0.0));
	DISTs[pi[0]].first = pi[0];
	
	distortion = 1.0;
	for (int lev=2; lev<=H+1; ++lev) {
		// printf("lev = %d\n", lev);
		fflush(stdout);
		radius /= alpha;
		memset(visit, false, sizeof(bool)*nV);
		for (int cid=0; cid<preC.size(); ++cid) {
			vector<int> cluster = preC[cid];
			int vtmp_sz = 0, local_n = cluster.size();
			double rad;
			while (!cluster.empty()) {
				vector<int> newCluster;
				double dis2cp_backup = DISTs[cluster[0]].second;
				rad = 0.0;
				for (int j=0; j<cluster.size(); ++j) {
					int u = cluster[j];
					double disTmp;
					if (DISTs[u].first == cluster[0]) {
						disTmp = DISTs[u].second;
					} else {
						disTmp = dist(V, u, cluster[0], tag);
					}
					if (!visit[u] && disTmp<=radius) {
						newCluster.push_back(u);
						DISTs[u].first = cluster[0];
						DISTs[u].second = disTmp;
						visit[u] = true;
						rad = max(rad, disTmp);
					}
				}
				if (!newCluster.empty()) {
					curC.push_back(newCluster);
					/*** revise the following lines***/
					Node_t* par = nodes_[newCluster[0]];
					Node_t* child = new Node_t(nid, cluster[0], lev, par, radius);
					child->rad = rad;
					child->dis2cp = dis2cp_backup;
					///// add child
					addChild(par, child);
					for (int vid=newCluster.size()-1; vid>=0; --vid) {
						nodes[newCluster[vid]] = child;
					}
					/*** revise the above lines***/
					++nid;
					int k = 0;
					for (int j=0; j<cluster.size(); ++j) {
						if (!visit[cluster[j]]) {
							cluster[k++] = cluster[j];
						}
					}
					cluster.erase(cluster.begin()+k, cluster.end());
					
					/*** calculate the distortion: begin ***/
					double minDist = dmax + 1.0, tmpDist;
					if (local_n <= threshold_n) {
						for (int i=0; i<vtmp_sz; ++i) {
							for (int vid=newCluster.size()-1; vid>=0; --vid) {
								tmpDist = dist(V, vtmp[i], newCluster[vid], tag);
								minDist = min(minDist, tmpDist);
							}
						}
						par->distortion = max(par->distortion, distAtLevel(lev-1)/minDist);
					} else {
						par->distortion = dmax+0.5;
					}
					distortion = max(par->distortion, distortion);
					for (int vid=newCluster.size()-1; vid>=0; --vid) {
						vtmp[vtmp_sz++] = newCluster[vid];
					}
					/*** calculate the distortion: end ***/
					
					if (newCluster.size() == preC[cid].size()) {
						mergeChild(par, child);
					}
				}
			}
		}
		/*** revise the following lines***/
		if (lev == H+1) {
			for (int j=0; j<nV; ++j) {
				leaves[j] = nodes[j];
			}
		}
		swap(nodes, nodes_);
		preC = curC;
		curC.clear();
	}
 
	nid = countTree(rt);
	printf("distCnt=%lld, nV=%d, nid=%d, H=%d, distortion=%.2lf, mem=%.2lf\n", distCnt, nV, nid, H, distortion, 1.0*nid*sizeof(Node_t)/1024.0);
	usedMemory = 1.0*nid*sizeof(Node_t)/1024.0;
	fflush(stdout);
	
	delete[] visit;
	delete[] nodes;
	delete[] nodes_;
}

static int RMQ_top;
static vector<Node_t*> RMQ_V;
static vector<int> RMQ_D;
static vector<int> RMQ_beg;
static vector<vector<int> > RMQ_dp;

void RMQ_clear() {
	RMQ_V.clear();
	RMQ_D.clear();
	RMQ_beg.clear();
	RMQ_dp.clear();
}

void RMQ_resize() {
	RMQ_top = 0;
	// 1...n instead of 0...n-1
	RMQ_V.push_back((Node_t*)NULL);
	RMQ_D.push_back(0);
	
	int nid = 1;
	for (int i=0; i<nV; ++i)
		nid = max(nid, leaves[i]->nid);
	RMQ_beg.resize(nid+1, 0);
	
	int IN = 2*nid + 5;
	int JN = log(1.0*IN)/log(2.0)+4; // log2(NV)
	RMQ_dp.resize(JN, vector<int>(IN, 0));
	
	// printf("n_node=%d, JN=%d, IN=%d, nV=%d, rt->nid=%d\n", nid, JN, IN, nV, rt->nid);
	// fflush(stdout);
}

void RMQ_dfs(Node_t* rt, int d) {
	if (rt == NULL) return ;
	
	++RMQ_top;
	RMQ_V.push_back(rt);
	RMQ_D.push_back(d);
	RMQ_beg[rt->nid] = RMQ_top;
	
	for (int i=0; i<rt->child.size(); ++i) {
		RMQ_dfs(rt->child[i], d+1);
		++RMQ_top;
		RMQ_V.push_back(rt);
		RMQ_D.push_back(d);
	}
}	

void RMQ_init(int n) {
    int i, j;

    for (i=1; i<=n; ++i)
        RMQ_dp[0][i] = i;
    for (j=1; (1<<j)<=n; ++j)
        for (i=1; i+(1<<j)-1<=n; ++i)
            if (RMQ_D[RMQ_dp[j-1][i]] < RMQ_D[RMQ_dp[j-1][i+(1<<(j-1))]])
                RMQ_dp[j][i] = RMQ_dp[j-1][i];
            else
                RMQ_dp[j][i] = RMQ_dp[j-1][i+(1<<(j-1))];
}

Node_t* RMQ_query(int l, int r) {
    if (l > r)
        swap(l, r);

    int k = floor(log(r-l+1.0)/log(2.0) - 1.0);

    while (1<<(k+1) <= r-l+1)
        ++k;

    if (RMQ_D[RMQ_dp[k][l]] < RMQ_D[RMQ_dp[k][r-(1<<k)+1]])
        return RMQ_V[RMQ_dp[k][l]];
    else
        return RMQ_V[RMQ_dp[k][r-(1<<k)+1]];
}

double RMQ_distOnHST(Node_t* pu, Node_t* pv) {
	Node_t* lca = RMQ_query(RMQ_beg[pu->nid], RMQ_beg[pv->nid]);
	return distAtLevel(lca->lev);
}

double RMQ_distLBOnHST(int u, int v) {
	if (u == v) return 0.0;
	
	Node_t* lca = RMQ_query(RMQ_beg[leaves[u]->nid], RMQ_beg[leaves[v]->nid]);
	return distAtLevel(lca->lev) / lca->distortion;
}

void RMQ_construct() {
	RMQ_resize();
	
	RMQ_dfs(rt, 1);
	
	RMQ_init(RMQ_top);
}

pair<double,double> getDistortion_fast() {
	RMQ_resize();
	RMQ_dfs(rt, 1);
	RMQ_init(RMQ_top);
	
	double cnt = 0;
	double maxDistor = 1.0;
	double avgDistor = 0.0;
	double d, dt, rat;
	
	for (int i=0; i<nV; ++i) {
		for (int j=i+1; j<nV; ++j) {
			d = dist(V, i, j, tag);
			dt = RMQ_distOnHST(leaves[i], leaves[j]);
			if (d > 0) {
				rat = max(dt/d, 1.0);
				maxDistor = max(maxDistor, rat);
				avgDistor += rat;
				cnt += 1.0;
			}
		}
	}
	if (cnt <= 0) {
		avgDistor = 1.0;
	} else {
		avgDistor /= cnt;
	}
	
	RMQ_clear();
	return make_pair(maxDistor, avgDistor);
}
