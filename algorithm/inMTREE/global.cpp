#include "global.h"

int nV = 0;
location_t* V = NULL;
const double speed = 1.0;
const double EPS = 1e-6;
const double INF = 1e25;
double usedTime = 0.0;
int usedMemory = 0;
double timeLimit = 12 * 60 * 60; // 12 hours
long long distCnt = 0; 
int tag = 0;
int dim = 112;
size_t dataSize = 0;
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
	++ distCnt;
	if (tag == 0) {
		if (x == y) return 0.0;

		location_t& a = V[x];
		location_t& b = V[y];
		double ret = 0.0;

		for (int i=0; i< dim; ++i) {
			ret += (a.x[i]-b.x[i]) * (a.x[i]-b.x[i]);
		}

		return sqrt(ret) * 1000;
	} else if (tag == 1) {
		int len = min(strlen(V[x].s+1), strlen(V[y].s+1));
		int ret = 0;
		for (int i = 1; i <= len; ++ i) {
			ret += V[x].s[i] != V[y].s[i];
		}
		return ret;
	} else if (tag == 2){
		int lenx = strlen(V[x].s+1), leny = strlen(V[y].s+1);
		f[0][0] = 0;
		for (int i = 1; i <= lenx; ++ i) {
			f[i][0] = i;
		}
		for (int j = 1; j <= leny; ++ j) {
			f[0][j] = j;
		}
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

		for (int i=0; i< dim; ++i) {
			ret += (a.x[i]-b.x[i])*(a.x[i]-b.x[i]);
		}

		return sqrt(ret) * 1000;
	} else if (tag == 1) {
		int len = min(strlen(a.s+1), strlen(b.s+1));
		int ret = 0;
		for (int i = 1; i <= len; ++ i) {
			ret += a.s[i] !=b.s[i];
		}
		return ret;
	} else if (tag == 2) {
		int lenx = strlen(a.s+1), leny = strlen(b.s+1);
		f[0][0] = 0;
		for (int i = 1; i <= lenx; ++ i) {
			f[i][0] = i;
		}
		for (int j = 1; j <= leny; ++ j) {
			f[0][j] = j;
		}
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
	++distCnt;
	if (tag == 0) {
		double ret = 0.0;

		for (int i=0; i< dim; ++i) {
			ret += (a->x[i]-b->x[i])*(a->x[i]-b->x[i]);
		}

		return sqrt(ret) * 1000;
	} else if (tag == 1) {
		int len = min(strlen(a->s+1), strlen(b->s+1));
		int ret = 0;
		for (int i = 1; i <= len; ++ i) {
			ret += a->s[i] !=b->s[i];
		}
		return ret;
	} else if (tag == 2) {
		int lenx = strlen(a->s+1), leny = strlen(b->s+1);
		f[0][0] = 0;
		for (int i = 1; i <= lenx; ++ i) {
			f[i][0] = i;
		}
		for (int j = 1; j <= leny; ++ j) {
			f[0][j] = j;
		}
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

		for (int i=0; i< dim; ++i) {
			ret += (a->x[i]-b.x[i])*(a->x[i]-b.x[i]);
		}

		return sqrt(ret) * 1000;
	} else if (tag == 1) {
		int len = min(strlen(a->s+1), strlen(b.s+1));
		int ret = 0;
		for (int i = 1; i <= len; ++ i) {
			ret += a->s[i] !=b.s[i];
		}
		return ret;
	} else if (tag == 2) {
		int lenx = strlen(a->s+1), leny = strlen(b.s+1);
		f[0][0] = 0;
		for (int i = 1; i <= lenx; ++ i) {
			f[i][0] = i;
		}
		for (int j = 1; j <= leny; ++ j) {
			f[0][j] = j;
		}
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

		for (int i=0; i< dim; ++i) {
			ret += (a.x[i]-b->x[i])*(a.x[i]-b->x[i]);
		}

		return sqrt(ret) * 1000;
	} else if (tag == 1) {
		int len = min(strlen(a.s+1), strlen(b->s+1));
		int ret = 0;
		for (int i = 1; i <= len; ++ i) {
			ret += a.s[i] !=b->s[i];
		}
		return ret;
	} else if (tag == 2) {
		int lenx = strlen(a.s+1), leny = strlen(b->s+1);
		f[0][0] = 0;
		for (int i = 1; i <= lenx; ++ i) {
			f[i][0] = i;
		}
		for (int j = 1; j <= leny; ++ j) {
			f[0][j] = j;
		}
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

void initDNA(const char *fileName) {
	ifstream fin(fileName, ios::in);   
    
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", fileName);
		exit(1);
	}
	fin >> nV;
	fin >> dim;
	V = new location_t[nV];
    for (int i=0; i<nV; ++i) {
		fin >> (V[i].s+1);
    }
    fin.close();
	tag = 1;
	dataSize = (dim+10)/10*10*sizeof(char);
	printf("dim = %d, dataSize = %d\n", (int)dim, (int)dataSize);
}

void initSyn(const char *fileName) {
	ifstream fin(fileName, ios::in);   
    
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", fileName);
		exit(1);
	}
	fin >> nV;
	fin >> dim;
	V = new location_t[nV];
    for (int i=0; i<nV; ++i) {
		fin >> (V[i].s+1);
    }
    fin.close();
	tag = 1;
	dataSize = (dim+10)/10*10*sizeof(char);
	printf("dim = %d, dataSize = %d\n", (int)dim, (int)dataSize);
}

void initDutch(const char *fileName) {
	ifstream fin(fileName, ios::in);   
    
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", fileName);
		exit(1);
	}
	char ch;
	fin >> nV >> dim;
	fin.get(ch);
	V = new location_t[nV];
	dim = 0;
    for (int i=0; i<nV; ++i) {
		fin.getline(V[i].s+1, 1000);
		dim = max(dim, (int)strlen(V[i].s+1));
    }
    fin.close();
	tag = 2;
	dataSize = (dim+10)/10*10*sizeof(char);
	printf("dim = %d, dataSize = %d\n", (int)dim, (int)dataSize);
}

void initColor(const char *fileName) {
	ifstream fin(fileName, ios::in);   
    
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", fileName);
		exit(1);
	}
	fin >> nV;
	fin >> dim;
	V = new location_t[nV];
    for (int i=0; i<nV; ++i) {
		for (int j = 0; j < dim; ++ j) {
			fin >> V[i].x[j];
		}
    }
    fin.close();
	tag = 0;
	dataSize = dim*sizeof(double);
	printf("dim = %d, dataSize = %d\n", (int)dim, (int)dataSize);
}

void initData(const char *fileName) {
	ifstream fin(fileName, ios::in);   
    
	if (!fin.is_open()) {
		fprintf(stderr, "FILE %s IS INVALID.", fileName);
		exit(1);
	}
	fin >> nV;
	fin >> dim;
	V = new location_t[nV];
    for (int i=0; i<nV; ++i) {
		for (int j = 0; j < dim; ++ j) {
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
		initColor("./color/dataset_color");
		initR("./color/r.txt");
		initK("./color/k.txt");
		initQ("./color/q.txt");
	} else if (fileName[1] == 'u') {
		initDutch("./dataset_dutch"); 
		initR("./r.txt");
		initK("./k.txt");
		initQ("./q.txt");
	} else if (fileName[1] == 'n') {
		initDutch("./english/dataset_english");
		initR("./english/r.txt");
		initK("./english/k.txt");
		initQ("./english/q.txt");
	} else if (fileName[1] == 'y') {
		initSyn("./synthetic/dataset_synthetic");
		initR("./synthetic/r.txt");
		initK("./synthetic/k.txt");
		initQ("./synthetic/q.txt");
	} else if (fileName[1] == 'a') {
		puts("large");
		initSyn("./large/dataset_synthetic"); 
		initR("./large/r.txt");
		initK("./large/k.txt");
		initQ("./large/q.txt");
	}
}

double genTestRadius() {
	if (DIM_V < 10) return 3;
	if (DIM_V < 50) return 7e6;
	return 2e7;
}

int genTestKth() {
	return 3;
}
