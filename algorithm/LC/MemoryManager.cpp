#include <unistd.h>
#include "MemoryManager.h"

static string genDiskFileName(int pageID) {
	string ret;
	stringstream ss;
	
	ss << "tmp_" << pageID << ".txt";
	
	return ss.str();
}

void removeFromFile(int pageN) {
	for (int pageId=0; pageId<pageN; ++pageId) {
		string fileName = genDiskFileName(pageId);
		remove(fileName.c_str());
	}
}

void readFromDisk(int pageID, location_t* V, size_t n_out) {
	string fileName = genDiskFileName(pageID);
	
	FILE *f = fopen(fileName.c_str(), "rb");
    if(!f) {
        fprintf(stderr, "could not open %s\n", fileName.c_str());
        perror("");
        abort();
    }
	
    size_t nr = fread(V, sizeof(location_t), n_out, f);
    assert(nr == n_out || !"could not read whole file");
	
	fclose(f);
}

void dumpToDisk(int pageID, location_t* V, size_t n_out) {
	string fileName = genDiskFileName(pageID);
	
	FILE *f = fopen(fileName.c_str(), "wb");
    if(!f) {
        fprintf(stderr, "could not open %s\n", fileName.c_str());
        perror("");
        abort();
    }
	
    size_t nr = fwrite(V, sizeof(location_t), n_out, f);
    assert(nr == n_out || !"could not write whole file");
	
	fclose(f);
}

void dumpToDisk(int pageID, int* pids, size_t n_out) {
	string fileName = genDiskFileName(pageID);
	
	FILE *f = fopen(fileName.c_str(), "wb");
    if(!f) {
        fprintf(stderr, "could not open %s\n", fileName.c_str());
        perror("");
        abort();
    }
	
    size_t nr = 0;
	
	for (size_t i=0; i<n_out; ++i) {
		nr += fwrite(V+pids[i], sizeof(location_t), 1, f);
	}
    assert(nr == n_out || !"could not write whole file");
	
	fclose(f);
}
