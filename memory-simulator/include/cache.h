#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;

struct CacheLine {
    bool valid;
    size_t tag;
};

class Cache {
private:
    vector<CacheLine> lines;
    int num_lines;
    int block_size;
    int level;
    int hits;
    int misses;

public:
    Cache(int size_bytes, int block_size_bytes, int lvl);
    int get_index(size_t addr);
    size_t get_tag(size_t addr);
    bool check(size_t addr);
    void insert(size_t addr);
    void invalidate(size_t addr); // Using the standard invalidate from Phase 2
    void print_stats();
    void dump_cache();
};

#endif