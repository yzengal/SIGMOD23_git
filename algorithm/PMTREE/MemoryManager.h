#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <bits/stdc++.h>
using namespace std;

#include "global.h"


template<typename key_t, typename value_t>
class cache_t {
public:	
	typedef pair<key_t, value_t> kv_pair_t;
	typedef typename list<kv_pair_t>::iterator list_iter_t;
	typedef typename unordered_map<key_t, list_iter_t>::iterator index_iter_t;

private:
	list<kv_pair_t> values;
	unordered_map<key_t, list_iter_t> index;
	int max_size;
	int sz;
	long long miss, tot;
	
public:
	cache_t(int _max_size=1024) : max_size(_max_size) {
		values.resize(max_size);
		sz = 0;
		miss = tot = 0;
	}
	
	bool empty() {
		return sz == 0;
	}
	
	int size() {
		return sz;
	}
	
	void resize(int _max_size) {
		max_size = _max_size;
		values.resize(max_size);
		index.clear();
		sz = 0;
		miss = tot = 0;		
	}
	
	void clear() {
		sz = 0;
		miss = tot = 0;
		index.clear();
		values.clear();
	}
	
	bool insert(key_t key, value_t value);
	bool query(key_t key, value_t& value);
	bool find(key_t key);
	void dump();
	void dumpCache();
};


template<typename key_t, typename value_t>
bool cache_t<key_t,value_t>::insert(key_t key, value_t value) {
	bool ret;
	index_iter_t iter = index.find(key);
	
	++tot;
	if (iter == index.end()) {
		if (sz == max_size) {
			--sz;
			index.erase(values.rbegin()->first);
			values.pop_back();
		}
		values.push_front(kv_pair_t(key, value));
		index[key] = values.begin();
		++sz;
		++miss;
		ret = false;
	} else {
		values.splice(values.begin(), values, iter->second);	
		ret = true;
	}
	
	return ret;
}

template<typename key_t, typename value_t>
bool cache_t<key_t,value_t>::query(key_t key, value_t& value) {
	index_iter_t iter = index.find(key);
	
	++tot;
	if (iter == index.end()) {
		++miss;
		return false;
	}
	
	value = iter->second->second;
	values.splice(values.begin(), values, iter->second);
	
	return true;
}

template<typename key_t, typename value_t>
bool cache_t<key_t,value_t>::find(key_t key) {
	index_iter_t iter = index.find(key);
	
	++tot;
	if (iter == index.end()) {
		++miss;
		return false;
	}
	
	values.splice(values.begin(), values, iter->second);
	
	return true;
}

template<typename key_t, typename value_t>
void cache_t<key_t,value_t>::dump() {
	if (tot == 0) {
		printf("Miss Rate: %.4lf\n", 0.0);
	} else {
		printf("Miss Rate: %.4lf\n", miss*1.0/tot);	
	}
}

template<typename key_t, typename value_t>
void cache_t<key_t,value_t>::dumpCache() {
	printf("Miss: %lld, Total = %lld\n", miss, tot);
	dump();
	printf("Cache Size: %d, Max Size = %d\n", sz, max_size);
	fflush(stdout);
	for(auto iter=values.begin(); iter!=values.end(); ++iter) {
		cout << "(" << iter->first << "," << iter->second << ") ";
	}
	cout << endl;
}

struct LRU_Manager_t {
	long long TOTAL_SIZE;
	long long LRU_PAGE_SIZE;
	long long PAGE_NUM;
	long long pageAccessNum;
	long long PAGE_N;
	cache_t<int,int> pageCache;
	
	inline int calcPageID(int idx) {
		return idx / PAGE_N; // 1 page can store 1024 locations
	}
	
	LRU_Manager_t() {
		TOTAL_SIZE = MEM_CACHE_SIZE; // unit: KB
		LRU_PAGE_SIZE = MEM_PAGE_SIZE; // unit: KB
		PAGE_NUM = TOTAL_SIZE / LRU_PAGE_SIZE;
		PAGE_N = 1;
		pageCache.resize(PAGE_NUM);
		pageAccessNum = 0;
	}
	
	void init(const int TOTAL_SIZE_, const int LRU_PAGE_SIZE_, const int PAGE_N_) {
		TOTAL_SIZE = TOTAL_SIZE_; // unit: KB
		LRU_PAGE_SIZE = LRU_PAGE_SIZE_; // unit: KB
		PAGE_NUM = TOTAL_SIZE / LRU_PAGE_SIZE;
		PAGE_N = PAGE_N_;
		pageCache.resize(PAGE_NUM);
		pageAccessNum = 0;
	}

	void access(int idx) {
		int pageID = calcPageID(idx);
		
		bool flag = pageCache.insert(pageID, 1);
		if (flag) return ;
		
		++pageAccessNum;
	}
	
	void clear() {
		pageCache.resize(PAGE_NUM);
		pageAccessNum = 0;
	}
};

void readFromDisk(int pageID, location_t* V, size_t n_out);
void readFromDisk(int pageID, location_t* V, double* dists, size_t n_out);
void dumpToDisk(int pageID, location_t* V, size_t n_out);
void dumpToDisk(int pageID, int* pids, size_t n_out);
void dumpToDisk(int pageID, pair<int, double>* entry, size_t n_out);
void removeFromFile(int pageN);


#endif
