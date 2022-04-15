#include "global.h"

int nV = 0, DIM_K = 1;
location_t* V = NULL;
const double speed = 1.0;
const double EPS = 1e-6;
const double INF = 1e25;
double usedTime = 0.0;
int usedMemory = 0;
double timeLimit = 12 * 60 * 60; // 12 hours
long long distCnt = 0; 
int tag = 0;
vector <double> RAN;
vector <int> QRY;
vector <int> KTH;

int f[200][200];

int dcmp(double x) {
	if (fabs(x) < EPS)
		return 0;
	return x>0 ? 1:-1;
}

double dist(location_t *V, int x, int y, int tag) {
	if (x == y) return 0.0;
	
	++ distCnt;
	if (tag == 0) {
		if (x == y) return 0.0;

		location_t& a = V[x];
		location_t& b = V[y];
		double ret = 0.0;

		for (int i=0; i<DIM_K; ++i) {
			ret += (a.x[i]-b.x[i]) * (a.x[i]-b.x[i]);
		}

		return 1000*sqrt(ret);
	} else if (tag == 1) {
		int len = DIM_K;
		int ret = 0;
		for (int i = 1; i <= len; ++ i) {
			ret += V[x].s[i] != V[y].s[i];
		}
		return ret;
	} else if (tag == 2){
		int lenx = strlen(V[x].s+1), leny = strlen(V[y].s+1);
		f[0][0] = 0;
		for (int i=1; i<=lenx; ++i)	f[i][0] = i;
		for (int j=1; j<=leny; ++j) f[0][j] = j;
		for (int i = 1; i <= lenx; ++ i) {
			for (int j = 1; j <= leny; ++ j) {
				f[i][j] = min(min(f[i-1][j] + 1, f[i][j-1] + 1), f[i-1][j-1] + (V[x].s[i] != V[y].s[j]));
			}
		}
		return f[lenx][leny];
	} else {
		return 0;
	}
}

double dist(location_t &a, location_t &b, int tag) {
	
	++distCnt;
	if (tag == 0) {
		double ret = 0.0;

		for (int i=0; i<DIM_K; ++i) {
			ret += (a.x[i]-b.x[i])*(a.x[i]-b.x[i]);
		}

		return 1000*sqrt(ret);
	} else if (tag == 1) {
		int len = DIM_K;
		int ret = 0;
		for (int i = 1; i <= len; ++ i) {
			ret += a.s[i] !=b.s[i];
		}
		return ret;
	} else if (tag == 2) {
		int lenx = strlen(a.s+1), leny = strlen(b.s+1);
		f[0][0] = 0;
		for (int i=1; i<=lenx; ++i)	f[i][0] = i;
		for (int j=1; j<=leny; ++j) f[0][j] = j;
		for (int i = 1; i <= lenx; ++ i) {
			for (int j = 1; j <= leny; ++ j) {
				f[i][j] = min(min(f[i-1][j] + 1, f[i][j-1] + 1), f[i-1][j-1] + (a.s[i] != b.s[j]));
			}
		}
		return f[lenx][leny];
	} else {
		return 0;
	}
	
}

double dist(location_t *a, location_t *b, int tag) {
	if (a == b) return 0.0;
	
	++distCnt;
	if (tag == 0) {
		double ret = 0.0;

		for (int i=0; i<DIM_K; ++i) {
			ret += (a->x[i]-b->x[i])*(a->x[i]-b->x[i]);
		}

		return 1000*sqrt(ret);
	} else if (tag == 1) {
		int len = DIM_K;
		int ret = 0;
		for (int i = 1; i <= len; ++ i) {
			ret += a->s[i] !=b->s[i];
		}
		return ret;
	} else if (tag == 2) {
		int lenx = strlen(a->s+1), leny = strlen(b->s+1);
		f[0][0] = 0;
		for (int i=1; i<=lenx; ++i)	f[i][0] = i;
		for (int j=1; j<=leny; ++j) f[0][j] = j;
		for (int i = 1; i <= lenx; ++ i) {
			for (int j = 1; j <= leny; ++ j) {
				f[i][j] = min(min(f[i-1][j] + 1, f[i][j-1] + 1), f[i-1][j-1] + (a->s[i] != b->s[j]));
			}
		}
		return f[lenx][leny];
	} else {
		return 0;
	}
}

double dist(location_t *a, location_t &b, int tag) {
	++distCnt;
	if (tag == 0) {
		double ret = 0.0;

		for (int i=0; i<DIM_K; ++i) {
			ret += (a->x[i]-b.x[i])*(a->x[i]-b.x[i]);
		}

		return 1000*sqrt(ret);
	} else if (tag == 1) {
		int len = DIM_K;
		int ret = 0;
		for (int i = 1; i <= len; ++ i) {
			ret += a->s[i] !=b.s[i];
		}
		return ret;
	} else if (tag == 2) {
		int lenx = strlen(a->s+1), leny = strlen(b.s+1);
		f[0][0] = 0;
		for (int i=1; i<=lenx; ++i)	f[i][0] = i;
		for (int j=1; j<=leny; ++j) f[0][j] = j;
		for (int i = 1; i <= lenx; ++ i) {
			for (int j = 1; j <= leny; ++ j) {
				f[i][j] = min(min(f[i-1][j] + 1, f[i][j-1] + 1), f[i-1][j-1] + (a->s[i] != b.s[j]));
			}
		}
		return f[lenx][leny];
	} else {
		return 0;
	}
}

double dist(location_t &a, location_t *b, int tag) {
	++distCnt;
	if (tag == 0) {
		double ret = 0.0;

		for (int i=0; i<DIM_K; ++i) {
			ret += (a.x[i]-b->x[i])*(a.x[i]-b->x[i]);
		}

		return 1000*sqrt(ret);
	} else if (tag == 1) {
		int len = DIM_K;
		int ret = 0;
		for (int i = 1; i <= len; ++ i) {
			ret += a.s[i] !=b->s[i];
		}
		return ret;
	} else if (tag == 2) {
		int lenx = strlen(a.s+1), leny = strlen(b->s+1);
		f[0][0] = 0;
		for (int i=1; i<=lenx; ++i)	f[i][0] = i;
		for (int j=1; j<=leny; ++j) f[0][j] = j;
		for (int i = 1; i <= lenx; ++ i) {
			for (int j = 1; j <= leny; ++ j) {
				f[i][j] = min(min(f[i-1][j] + 1, f[i][j-1] + 1), f[i-1][j-1] + (a.s[i] != b->s[j]));
			}
		}
		return f[lenx][leny];
	} else {
		return 0;
	}
}

void freeLocation() {
	delete[] V;
}

void initSyn(const char *fileName) {
	ifstream fin(fileName, ios::in);   
    
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", fileName);
		exit(1);
	}
	fin >> nV;
	fin >> DIM_K;
	V = new location_t[nV];
    for (int i=0; i<nV; ++i) {
		fin >> (V[i].s+1);
    }
    fin.close();
	tag = 1;
}

void initEnglish(const char *fileName) {
	ifstream fin(fileName, ios::in);

	if (!fin.is_open()) {
			fprintf(stderr, "FILE %s IS INVALID.", fileName);
			exit(1);
	}
	int dim;
	char ch;
	fin >> nV >> DIM_K;
	fin.get(ch);
	V = new location_t[nV];
	for (int i=0; i<nV; ++i) {
		fin.getline(V[i].s+1, DIM_V-1);
	}
	fin.close();
	tag = 2;
}

void initDutch(const char *fileName) {
	ifstream fin(fileName, ios::in);   
    
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", fileName);
		exit(1);
	}
	int dim;
	char ch;
	fin >> nV >> DIM_K;
	fin.get(ch);
	V = new location_t[nV];
    for (int i=0; i<nV; ++i) {
		fin.getline(V[i].s+1, DIM_V-1);
    }
    fin.close();
	tag = 2;
}

void initColor(const char *fileName) {
	ifstream fin(fileName, ios::in);   
    
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", fileName);
		exit(1);
	}
	fin >> nV;
	fin >> DIM_K;
	V = new location_t[nV];
    for (int i=0; i<nV; ++i) {
		for (int j = 0; j < DIM_K; ++ j) {
			fin >> V[i].x[j];
		}
    }
    fin.close();
	tag = 0;
}

void initR(const char *fileName) {
	ifstream fin(fileName, ios::in);   
    
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", fileName);
		exit(1);
	}
	int n;
	double tmp;
	fin >> n;
	RAN.clear();
    for (int i=0; i<n; ++i) {
		fin >> tmp;
		RAN.push_back(tmp);
    }
    fin.close();
}

void initK(const char *fileName) {
	ifstream fin(fileName, ios::in);   
    
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", fileName);
		exit(1);
	}
	int n, tmp;
	fin >> n;
	KTH.clear();
    for (int i=0; i<n; ++i) {
		fin >> tmp;
		KTH.push_back(tmp);
    }
    fin.close();
}

void initQ(const char *fileName) {
	ifstream fin(fileName, ios::in);   
    
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", fileName);
		exit(1);
	}
	int n, tmp;
	fin >> n;
	QRY.clear();
    for (int i=0; i<n; ++i) {
		fin >> tmp;
		QRY.push_back(tmp);
    }
    fin.close();
}

void initLocation(string &fileName) {
	if (fileName[1] == 'o') {
		initColor("../../dataset/color/dataset_color");
		initR("../../dataset/color/r.txt");
		initK("../../dataset/color/k.txt");
		initQ("../../dataset/color/q.txt");
	} else if (fileName[1] == 'u') {
		initDutch("../../dataset/dutch/dataset_dutch"); 
		initR("../../dataset/dutch/r.txt");
		initK("../../dataset/dutch/k.txt");
		initQ("../../dataset/dutch/q.txt");
	} else if (fileName[1] == 'n') {
		initDutch("../../dataset/english/dataset_english");
		initR("../../dataset/english/r.txt");
		initK("../../dataset/english/k.txt");
		initQ("../../dataset/english/q.txt");
	} else if (fileName[1] == 'y') {
		initSyn("../../dataset/synthetic/dataset_synthetic");
		initR("../../dataset/synthetic/r.txt");
		initK("../../dataset/synthetic/k.txt");
		initQ("../../dataset/synthetic/q.txt");
	}
}
