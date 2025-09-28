// debug_deadlock.cpp
// Minimal test to debug potential deadlock issues with SAMM and object lists
// Progressive complexity to isolate deadlock causes

#include <iostream>
#include <chrono>
#include <thread>
#include "../../HeapManager/HeapManager.h"
#include "../../runtime/ListDataTypes.h"

// Global trace variables required by the runtime
bool g_enable_heap_trace = false;
bool g_enable_lexer_trace = false;
bool g_enable_symbols_trace = false;

extern "C" {
    void BCPL_LIST_APPEND_OBJECT(ListHeader* header, void* object_ptr);
    void* Heap_allocObject(size_t size);
    ListHeader* BCPL_LIST_CREATE_EMPTY(void);
    void HeapManager_enterScope(void);
    void HeapManager_exitScope(void);
    void HeapManager_setSAMMEnabled(int enabled);
    int HeapManager_isSAMMEnabled(void);
}

// Simple mock functions
extern "C" void mock_create_function() { }
extern "C" void mock_release_function() { }

// Simple vtable and object
struct SimpleVTable {
    void (*create_func)();
    void (*release_func)();
};

struct SimpleObject {
    void* vtable;
    int64_t value;
};

static SimpleVTable simple_vtable = { mock_create_function, mock_release_function };

SimpleObject* create_simple_object(int64_t value) {
    SimpleObject* obj = (SimpleObject*)Heap_allocObject(sizeof(SimpleObject));
    if (obj) {
        obj->vtable = &simple_vtable;
        obj->value = value;
    }
    return obj;
}

void test_phase(const char* phase_name, int iterations, int objects_per_iteration) {
    std::cout << "\n=== " << phase_name << " ===\n";
    std::cout << "Iterations: " << iterations << ", Objects per iteration: " << objects_per_iteration << "\n";
    
    auto start = std::chrono::steady_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        std::cout << "Starting iteration " << (i + 1) << "...";
        std::cout.flush();
        
        HeapManager_enterScope();
        {
            ListHeader* header = BCPL_LIST_CREATE_EMPTY();
            
            for (int j = 0; j < objects_per_iteration; ++j) {
                SimpleObject* obj = create_simple_object(i * 1000 + j);
                BCPL_LIST_APPEND_OBJECT(header, obj);
            }
        }
        std::cout << " scope entered and populated...";
        std::cout.flush();
        
        HeapManager_exitScope();
        std::cout << " scope exited.\n";
        std::cout.flush();
        
        // Small delay to let cleanup happen
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<double, std::milli>(end - start).count();
    
    std::cout << "Phase completed in " << elapsed << " ms\n";
    
    // Check memory state
    auto stats = HeapManager::getInstance().getSAMMStats();
    std::cout << "Objects cleaned so far: " << stats.objects_cleaned << "\n";
    std::cout << "Current scope depth: " << stats.current_scope_depth << "\n";
}

int main() {
    std::cout << "=== DEADLOCK DEBUG TEST ===\n";
    
    // Enable SAMM
    std::cout << "Enabling SAMM...\n";
    HeapManager_setSAMMEnabled(1);
    std::cout << "SAMM enabled: " << (HeapManager_isSAMMEnabled() ? "YES" : "NO") << "\n";
    
    // Disable tracing
    HeapManager::getInstance().setTraceEnabled(false);
    
    try {
        // Phase 1: Very simple - just 5 iterations, 1 object each
        test_phase("Phase 1: Minimal", 5, 1);
        
        // Phase 2: Small scale - 10 iterations, 10 objects each  
        test_phase("Phase 2: Small Scale", 10, 10);
        
        // Phase 3: Medium scale - 20 iterations, 50 objects each
        test_phase("Phase 3: Medium Scale", 20, 50);
        
        // Phase 4: Higher scale - 50 iterations, 100 objects each
        test_phase("Phase 4: Higher Scale", 50, 100);
        
        // Phase 5: Stress scale - 100 iterations, 200 objects each
        std::cout << "\n!!! ENTERING POTENTIAL DEADLOCK ZONE !!!\n";
        test_phase("Phase 5: Stress Scale", 100, 200);
        
        std::cout << "\n=== ALL PHASES COMPLETED SUCCESSFULLY ===\n";
        
        // Final cleanup wait
        std::cout << "Waiting 5 seconds for final cleanup...\n";
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        // Final stats
        auto final_stats = HeapManager::getInstance().getSAMMStats();
        std::cout << "Final objects cleaned: " << final_stats.objects_cleaned << "\n";
        std::cout << "Final scope depth: " << final_stats.current_scope_depth << "\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "\nEXCEPTION CAUGHT: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cout << "\nUNKNOWN EXCEPTION CAUGHT\n";
        return 1;
    }
}