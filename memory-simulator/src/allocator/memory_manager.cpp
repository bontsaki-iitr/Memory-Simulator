#include "../../include/memory_manager.h"

MemoryManager::MemoryManager(size_t size) {
    total_memory = size;
    next_block_id = 1;
    strategy = 1; 
    success_allocs = 0; failed_allocs = 0; word_size = 4;

    l1 = new Cache(32, 8, 1);
    l2 = new Cache(64, 16, 2);

    head = new Block(0, size, 0, 0, true, nullptr); 
    cout << "System Ready (RAM: " << size << ")" << endl;
}

MemoryManager::~MemoryManager() {
    delete l1; delete l2;
    Block* c = head; while(c) { Block* n = c->next; delete c; c=n; }
}

void MemoryManager::config_cache(int level, int size, int blk) {
    if(level == 1) { delete l1; l1 = new Cache(size, blk, 1); cout << "L1 Reconfigured." << endl; } 
    else { delete l2; l2 = new Cache(size, blk, 2); cout << "L2 Reconfigured." << endl; }
}

void MemoryManager::set_strategy(int s) {
    strategy = s;
    cout << "Strategy set to: ";
    if (strategy == 1) cout << "First Fit" << endl;
    else if (strategy == 2) cout << "Best Fit" << endl;
    else if (strategy == 3) cout << "Worst Fit" << endl;
    else cout << "Unknown (Defaulting to First Fit)" << endl;
}

void MemoryManager::access_memory(size_t addr) {
    bool valid = false;
    Block* c = head;
    while(c) { 
        if(!c->is_free && addr >= c->start_addr && addr < (c->start_addr + c->size)) { 
            valid=true; break; 
        }
        c = c->next; 
    }
    if(!valid) { cout << "SegFault: " << addr << endl; return; }

    cout << "CPU Read: " << addr << endl;
    
    if (l1->check(addr)) { cout << " -> L1 HIT" << endl; return; }
    cout << " -> L1 Miss." << endl;
    
    if (l2->check(addr)) { 
        cout << " -> L2 HIT. Promoting to L1." << endl;
        l1->insert(addr);
        return;
    }

    cout << " -> L2 Miss. RAM Fetch." << endl;
    l2->insert(addr);
    l1->insert(addr);
}

void MemoryManager::my_malloc(size_t request_size) {
    size_t aligned = request_size;
    if (request_size % word_size != 0) aligned = request_size + (word_size - (request_size % word_size));

    Block* curr = head; 
    Block* chosen = nullptr;

    if (strategy == 1) { // First Fit
        while (curr) { 
            if (curr->is_free && curr->size >= aligned) { chosen = curr; break; } 
            curr = curr->next; 
        }
    }
    else if (strategy == 2) { // Best Fit
        while (curr) {
            if (curr->is_free && curr->size >= aligned) {
                if (chosen == nullptr || curr->size < chosen->size) {
                    chosen = curr;
                }
            }
            curr = curr->next;
        }
    }
    else if (strategy == 3) { // Worst Fit
        while (curr) {
            if (curr->is_free && curr->size >= aligned) {
                if (chosen == nullptr || curr->size > chosen->size) {
                    chosen = curr;
                }
            }
            curr = curr->next;
        }
    }

    if (!chosen) { cout << "Alloc Fail " << aligned << endl; failed_allocs++; return; }

    if (chosen->size > aligned) {
        Block* hole = new Block(0, chosen->size - aligned, 0, chosen->start_addr + aligned, true, chosen->next);
        chosen->next = hole; chosen->size = aligned;
    }
    chosen->is_free = false; chosen->id = next_block_id++; chosen->requested = request_size;
    success_allocs++;
    cout << "Alloc ID " << chosen->id << " @ " << chosen->start_addr << endl;
}

void MemoryManager::my_free(int id) {
    Block* c = head; bool found = false;
    while(c) {
        if(!c->is_free && c->id == id) {
            c->is_free = true; c->id = 0; found = true;
            for(size_t i=0; i<c->size; i++) { 
                l1->invalidate(c->start_addr+i); 
                l2->invalidate(c->start_addr+i); 
            }
            break;
        }
        c = c->next;
    }
    if(!found) { cout << "ID " << id << " not found." << endl; return; }
    
    Block* m = head;
    while(m && m->next) {
        if(m->is_free && m->next->is_free) {
            Block* n = m->next; m->size += n->size; m->next = n->next; delete n;
        } else m = m->next;
    }
    cout << "Freed ID " << id << endl;
}

void MemoryManager::dump_system() {
    cout << "\n=== SYSTEM DUMP ===" << endl;
    l1->dump_cache();
    l2->dump_cache();
    
    cout << "--- RAM Dump ---" << endl;
    Block* current = head;
    while (current != nullptr) {
        cout << "[" << current->start_addr << "-" 
                << (current->start_addr + current->size - 1) << "] ";
        if (current->is_free) cout << "FREE (" << current->size << " bytes)" << endl;
        else cout << "USED ID:" << current->id << " (Req:" << current->requested << ", Alloc:" << current->size << ")" << endl;
        current = current->next;
    }
    cout << "===================" << endl;
}

void MemoryManager::print_stats() { 
    size_t used_mem = 0;
    size_t requested_mem = 0;
    size_t free_mem = 0;
    size_t largest_free = 0;

    Block* curr = head;
    while(curr) {
        if (curr->is_free) {
            free_mem += curr->size;
            if(curr->size > largest_free) largest_free = curr->size;
        } else {
            used_mem += curr->size;
            requested_mem += curr->requested;
        }
        curr = curr->next;
    }

    double util = ((double)used_mem / total_memory) * 100.0;
    double ext_frag = (free_mem > 0) ? (1.0 - ((double)largest_free / free_mem)) * 100.0 : 0.0;
    double int_frag = (used_mem > 0) ? ((double)(used_mem - requested_mem) / used_mem) * 100.0 : 0.0;

    cout << "\n=== STATISTICS ===" << endl;
    cout << "Memory Utilization:    " << fixed << setprecision(2) << util << "%" << endl;
    cout << "External Fragmentation:" << fixed << setprecision(2) << ext_frag << "%" << endl;
    cout << "Internal Fragmentation:" << fixed << setprecision(2) << int_frag << "%" << endl;
    cout << "Successful Allocs:     " << success_allocs << endl;
    cout << "Failed Allocs:         " << failed_allocs << endl;
    cout << "------------------" << endl;
    l1->print_stats(); 
    l2->print_stats(); 
    cout << "==================" << endl;
}