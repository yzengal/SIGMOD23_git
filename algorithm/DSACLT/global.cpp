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
int pageSize = 4096;
int bucketSize = 40*1024;
vector <double> RAN;
vector <int> QRY;
vector <int> KTH;

int dcmp(double x) {
	if (fabs(x) < EPS)
		return 0;
	return x>0 ? 1:-1;
}

double dist(location_t *V, int x, int y, int tag) {
	++ distCnt;
	if (x == y) return 0.0;

	location_t& a = V[x];
	location_t& b = V[y];
	float ret = 0.0, tmp;

	for (int i=0; i< dim; ++i) {
		tmp = a.x[i];
		tmp -= b.x[i];
		ret += tmp * tmp;
	}

	return sqrt(ret);
}

double dist(location_t &a, location_t &b, int tag) {
	++distCnt;
	float ret = 0.0, tmp;

	for (int i=0; i< dim; ++i) {
		tmp = a.x[i];
		tmp -= b.x[i];
		ret += tmp * tmp;
	}

	return sqrt(ret);
	
}

double dist(location_t *a, location_t *b, int tag) {
	++distCnt;
	float ret = 0.0, tmp;

	for (int i=0; i< dim; ++i) {
		tmp = a->x[i];
		tmp -= b->x[i];
		ret += tmp * tmp;
	}

	return sqrt(ret);
}

double dist(location_t *a, location_t &b, int tag) {
	++distCnt;
	float ret = 0.0, tmp;

	for (int i=0; i< dim; ++i) {
		tmp = a->x[i];
		tmp -= b.x[i];
		ret += tmp * tmp;
	}

	return sqrt(ret);
}

double dist(location_t &a, location_t *b, int tag) {
	++distCnt;
	float ret = 0.0, tmp;

	for (int i=0; i< dim; ++i) {
		tmp = a.x[i];
		tmp -= b->x[i];
		ret += tmp * tmp;
	}

	return sqrt(ret);
}

void freeLocation() {
	delete[] V;
}
void initSIFT(const char *fileName) {
	FILE *f = fopen(fileName, "rb");
    if(!f) {
        fprintf(stderr, "could not open %s\n", fileName);
        perror("");
        abort();
    }
	
	size_t n_out, d_out;
   
    fread(&n_out, sizeof(size_t), 1, f);
    fread(&d_out, sizeof(size_t), 1, f);
	nV = n_out, dim = d_out;
	
	size_t sz = d_out * n_out;
	
	V = new location_t[n_out];
    size_t nr = fread(V, sizeof(unsigned char), sz, f);
    assert(nr == sz || !"could not read whole file");
	
    tag = 0;
	dataSize = dim*sizeof(unsigned char) + sizeof(int);
	pageSize = (MEM_PAGE_SIZE*1024)/dataSize;
	printf("dim = %d, dataSize = %d, nV = %d, sz = %lld\n", dim, dataSize, nV, (long long)sz);
	fflush(stdout);
	
	fclose(f);
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
	initSIFT("../SIFT100M/sift.txt"); 
	initR("../SIFT100M/r.txt");
	initK("../SIFT100M/k.txt");
	initQ("../SIFT100M/q.txt");
}

double genTestRadius() {
	if (DIM_V < 10) return 3;
	if (DIM_V < 50) return 7e6;
	return 2e7;
}

int genTestKth() {
	return 3;
}
