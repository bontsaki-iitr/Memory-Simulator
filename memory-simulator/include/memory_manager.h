#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>
#include <vector>
#include <iomanip>
#include "cache.h" 

using namespace std;

struct Block {
    int id;
    size_t size;
    size_t requested;
    size_t start_addr;
    bool is_free;
    Block* next;

    Block(int i, size_t s, size_t req, size_t addr, bool f, Block* n = nullptr)
        : id(i), size(s), requested(req), start_addr(addr), is_free(f), next(n) {}
};

class MemoryManager {
private:
    Block* head;
    size_t total_memory;
    int next_block_id;
    int strategy; 
    int success_allocs;
    int failed_allocs;
    size_t word_size;

    Cache* l1;
    Cache* l2;

public:
    MemoryManager(size_t size);
    ~MemoryManager();
    void config_cache(int level, int size, int blk);
    void set_strategy(int s);
    void access_memory(size_t addr);
    
    void my_malloc(size_t request_size); 
    void my_free(int id);
    
    void dump_system();
    void print_stats();
};

#endif