#include "../../include/cache.h"

Cache::Cache(int size_bytes, int block_size_bytes, int lvl) 
    : block_size(block_size_bytes), level(lvl), hits(0), misses(0) {
    
    num_lines = size_bytes / block_size;
    if (num_lines < 1) num_lines = 1; 
    lines.resize(num_lines, {false, 0});
}

int Cache::get_index(size_t addr) { return (addr / block_size) % num_lines; }
size_t Cache::get_tag(size_t addr) { return addr / block_size; }

bool Cache::check(size_t addr) {
    int idx = get_index(addr);
    size_t tag = get_tag(addr);
    if (lines[idx].valid && lines[idx].tag == tag) {
        hits++; return true; 
    }
    misses++; return false;
}

void Cache::insert(size_t addr) {
    int idx = get_index(addr);
    lines[idx] = {true, get_tag(addr)};
}

void Cache::invalidate(size_t addr) {
    int idx = get_index(addr);
    if (lines[idx].valid && lines[idx].tag == get_tag(addr)) {
        lines[idx].valid = false;
    }
}

void Cache::print_stats() {
    int total = hits + misses;
    double ratio = (total > 0) ? ((double)hits / total) * 100.0 : 0.0;
    cout << "L" << level << " Cache: Size=" << (num_lines * block_size) 
            << "B, Block=" << block_size << "B, Slots=" << num_lines << endl;
    cout << "   Hits=" << hits << ", Misses=" << misses 
            << " (Hit Ratio: " << fixed << setprecision(1) << ratio << "%)" << endl;
}

void Cache::dump_cache() {
    cout << "--- L" << level << " Content ---" << endl;
    for (int i = 0; i < num_lines; i++) {
        cout << "Slot " << i << ": ";
        if (lines[i].valid) cout << "[Block " << lines[i].tag << "] ";
        else cout << "[Empty] ";
        cout << endl;
    }
}