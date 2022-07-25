#include <bits/stdc++.h>
#include <sys/stat.h>
using namespace std;


void fvecs_read_stat (const char *fname,
                    size_t *d_out, size_t *n_out)
{
	FILE *f = fopen(fname, "r");
	if(!f) {
		fprintf(stderr, "could not open %s\n", fname);
		perror("");
		abort();
	}
	int d;
	fread(&d, 1, sizeof(int), f);
	assert((d > 0 && d < 1000000) || !"unreasonable dimension");
	fseek(f, 0, SEEK_SET);
    struct stat st;
    fstat(fileno(f), &st);
    size_t sz = st.st_size;
	// size_t sz = 0;
	assert(sz % (d * sizeof(unsigned char) + sizeof(int)) == 0 || !"weird file size");
	size_t n = sz / (d * sizeof(unsigned char) + sizeof(int));
	
	*d_out = d; *n_out = n;
	cout << (*d_out) << " " << (*n_out) << " " << sz << endl;
	
	fclose(f);
}			

float * fvecs_read_float (const char *fname,
                    size_t *d_out, size_t *n_out)
{
    FILE *f = fopen(fname, "r");
    if(!f) {
        fprintf(stderr, "could not open %s\n", fname);
        perror("");
        abort();
    }
    int d;
    fread(&d, 1, sizeof(int), f);
    assert((d > 0 && d < 1000000) || !"unreasonable dimension");
    fseek(f, 0, SEEK_SET);
    struct stat st;
    fstat(fileno(f), &st);
    size_t sz = st.st_size;
	// size_t sz = 0;
    assert(sz % ((d + 1) * 4) == 0 || !"weird file size");
    size_t n = sz / ((d + 1) * 4);

    *d_out = d; *n_out = n;
    float *x = new float[n * (d + 1)];
    size_t nr = fread(x, sizeof(float), n * (d + 1), f);
    assert(nr == n * (d + 1) || !"could not read whole file");

    fclose(f);
    return x;
}

unsigned char * fvecs_read_uchar (const char *fname,
                    size_t *d_out, size_t *n_out)
{
    FILE *f = fopen(fname, "r");
    if(!f) {
        fprintf(stderr, "could not open %s\n", fname);
        perror("");
        abort();
    }
    int d;
    fread(&d, 1, sizeof(int), f);
    assert((d > 0 && d < 1000000) || !"unreasonable dimension");
    fseek(f, 0, SEEK_SET);
    struct stat st;
    fstat(fileno(f), &st);
    size_t sz = st.st_size;
	// size_t sz = 0;
	assert(sz % (d * sizeof(unsigned char) + sizeof(int)) == 0 || !"weird file size");
	size_t n = sz / (d * sizeof(unsigned char) + sizeof(int));

    *d_out = d; *n_out = n;
    unsigned char *x = new unsigned char[n * (d + sizeof(int))];
    size_t nr = fread(x, sizeof(unsigned char), n * (d + sizeof(int)), f);
    assert(nr == n * (d + sizeof(int)) || !"could not read whole file");
	
    // shift array to remove row headers
    // for(size_t i = 0; i < n; i++)
        // memmove(x + i * d, x + 1 + i * (d + 1), d * sizeof(*x));

    fclose(f);
    return x;
}

void transform_uchar(unsigned char *x, size_t dimA, size_t dimB, size_t nV) {
	const size_t sz = dimB * nV;
	const size_t offset = sizeof(int);
	printf("dimA = %lld, dimB = %lld, nV = %lld, sz = %lld, offset = %lld\n", dimA, dimB, nV, sz, offset);
	fflush(stdout);
	for(size_t i = 0; i < 5; i++) {
		for (size_t j=0; j < 5; j++) {
			printf("%d ", (int)x[i*(dimA+offset)+j+offset]);
		}
		putchar('\n');
	}
	fflush(stdout);
	
	// shift array to remove row headers
	for(size_t i = 0; i < nV; ++i) {
        memmove(x + i * dimB, x + offset + i * (dimA + offset), dimB * sizeof(unsigned char));
	}
	// for (size_t i=0,baseA=0,baseB=0; i<nV; ++i,baseA+=dimA+offset,baseB+=dimB) {
		// for (size_t j=0; j<dimB; ++j) {
			// assert(baseB+j < baseA+j+offset);
			// assert((128+4)*100000000LL > baseA+j+offset);
			// x[baseB+j] = x[baseA+j+offset];
		// }
	// }
}

unsigned char* transform_float(float *x, int dimA, int dimB, int nV) {
	const int sz = dimB * nV;
	unsigned char* ret = new unsigned char[sz];
	for (size_t i=0,baseA=0,baseB=0; i<nV; ++i,baseA+=dimA,baseB+=dimB) {
		for (size_t j=0; j<dimB; ++j) {
			ret[baseB+j] = int(x[baseA+j+1]) % 256;
		}
	}
	return ret;
}

size_t dumpFile(const char *fname, unsigned char *x, size_t dim, size_t nV) {
    FILE *f = fopen(fname, "wb");
    if(!f) {
        fprintf(stderr, "could not open %s\n", fname);
        perror("");
        abort();
    }
	
	size_t ret = 0;
	
	// S1. write nV
	ret += fwrite(&nV, sizeof(size_t), 1, f);
	
	// S2. write dim
	ret += fwrite(&dim, sizeof(size_t), 1, f);
	
	// S3. write data
	ret += fwrite(x, sizeof(unsigned char), dim*nV, f);

	// S3. close file
    fclose(f);
	
    return ret;	
}

unsigned char * fvecs_read_uchar2 (const char *fname, size_t *d_out, size_t *n_out)
{
    FILE *f = fopen(fname, "rb");
    if(!f) {
        fprintf(stderr, "could not open %s\n", fname);
        perror("");
        abort();
    }
   
    fread(n_out, sizeof(size_t), 1, f);
    fread(d_out, sizeof(size_t), 1, f);
	
	size_t sz = (*d_out) * (*n_out);
	printf("dim = %d, nV = %d, sz = %d\n", *d_out, *n_out, sz);
	fflush(stdout);
    unsigned char *x = new unsigned char[sz];
	printf("[BEFORE] fread()\n");
	fflush(stdout);
    size_t nr = fread(x, sizeof(unsigned char), sz, f);
    assert(nr == sz || !"could not read whole file");
	cout << nr << endl;
	printf("[AFTER] fread()\n");
	fflush(stdout);

    fclose(f);
    return x;
}

int main(int argc, char **argv) {
	string srcFileName("bigann_learn.bvecs");
	string desFileName("sift.txt");
	if (argc > 1) {
		srcFileName = string(argv[1]);
	}
	
	size_t dim, nV;
	
	fvecs_read_stat(srcFileName.c_str(), &dim, &nV);
	printf("dim = %d, nV = %d\n", dim, nV);
	fflush(stdout);
	
	unsigned char *arr = fvecs_read_uchar(srcFileName.c_str(), &dim, &nV);
	printf("dim = %d, nV = %d\n", dim, nV);
	fflush(stdout);
	
	const size_t myDim = 128;
	const size_t myNV = nV;
	transform_uchar(arr, dim, myDim, myNV);
	
	size_t cnt = dumpFile(desFileName.c_str(), arr, myDim, myNV);
	printf("%lld (=%lld*%lld+2) elements has been written.\n", cnt, myDim, myNV);
	
	delete[] arr;
	
	return 0;
}