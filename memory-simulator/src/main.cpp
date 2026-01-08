#include <iostream>
#include <string>
#include "../include/memory_manager.h"

using namespace std;

int main() {
    MemoryManager* mem = nullptr;
    string command;
    
    cout << "=== Memory Simulator: Phase 2 (Modular Version) ===" << endl;
    cout << "Commands: init <size>, malloc <size>, free <id>, access <addr>" << endl;
    cout << "          strategy <1-3>, dump, stats, config <lvl> <total> <blk>" << endl;

    while (true) {
        cout << "> "; cin >> command;
        if (command == "exit") break;
        else if (command == "init") {
            size_t size; cin >> size;
            if (mem) delete mem;
            mem = new MemoryManager(size);
        }
        else if (command == "config") {
            if(!mem) continue;
            int l, s, b; cin >> l >> s >> b;
            mem->config_cache(l, s, b);
        }
        else if (command == "strategy") {
            if(!mem) continue;
            int s; cin >> s;
            mem->set_strategy(s);
        }
        else if (command == "malloc") {
            size_t s; cin >> s; if(mem) mem->my_malloc(s);
        }
        else if (command == "free") {
            int id; cin >> id; if(mem) mem->my_free(id);
        }
        else if (command == "access") {
            size_t addr; cin >> addr; if(mem) mem->access_memory(addr);
        }
        else if (command == "dump") { if(mem) mem->dump_system(); }
        else if (command == "stats") { if(mem) mem->print_stats(); }
    }
    return 0;
}