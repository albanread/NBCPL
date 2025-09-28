#include <iostream>
#include <chrono>
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
}

// Simple test object with just 3 slots
struct TestObject3 {
    void* vtable;      // 8 bytes - vtable pointer
    int64_t slot1;     // 8 bytes
    int64_t slot2;     // 8 bytes  
    int64_t slot3;     // 8 bytes
    // Total: 32 bytes
};

// Mock function implementations matching BCPL calling convention
extern "C" void mock_create_function() {
    // Safe no-op constructor
}

extern "C" void mock_release_function() {
    // Safe no-op destructor - HeapManager handles object memory
}

// Proper vtable structure matching BCPL compiler expectations
struct MockVTable {
    void (*create_func)();
    void (*release_func)();
};

// Global vtable instance with proper function pointers
static MockVTable mock_vtable = { mock_create_function, mock_release_function };

TestObject3* create_test_object3(int64_t base_value) {
    std::cout << "Creating object with size: " << sizeof(TestObject3) << " bytes\n";
    TestObject3* obj = (TestObject3*)Heap_allocObject(sizeof(TestObject3));
    if (obj) {
        std::cout << "Object allocated at: " << obj << "\n";
        obj->vtable = &mock_vtable;  // Point to vtable structure, not null
        obj->slot1 = base_value;
        obj->slot2 = base_value + 1;
        obj->slot3 = base_value + 2;
        std::cout << "Object initialized with values: " << obj->slot1 << ", " << obj->slot2 << ", " << obj->slot3 << "\n";
        std::cout << "Object vtable set to: " << obj->vtable << "\n";
    } else {
        std::cout << "Object allocation FAILED!\n";
    }
    return obj;
}

int main() {
    std::cout << "=== Minimal Object Test ===\n";
    
    // Disable HeapManager tracing to reduce noise
    HeapManager::getInstance().setTraceEnabled(false);
    
    try {
        std::cout << "Step 1: Creating a single object...\n";
        HeapManager_enterScope();
        TestObject3* obj = create_test_object3(42);
        if (!obj) {
            std::cout << "FAILED: Could not create object\n";
            return 1;
        }
        std::cout << "SUCCESS: Object created\n";
        
        std::cout << "Step 2: Creating empty list...\n";
        ListHeader* header = BCPL_LIST_CREATE_EMPTY();
        if (!header) {
            std::cout << "FAILED: Could not create list\n";
            return 1;
        }
        std::cout << "SUCCESS: List created at " << header << "\n";
        std::cout << "List type: " << header->type << ", length: " << header->length << "\n";
        
        std::cout << "Step 3: Appending object to list...\n";
        BCPL_LIST_APPEND_OBJECT(header, obj);
        std::cout << "SUCCESS: Object appended to list\n";
        std::cout << "List length after append: " << header->length << "\n";
        
        std::cout << "Step 4: Verifying list contents...\n";
        if (header->head && header->head->type == ATOM_OBJECT) {
            TestObject3* retrieved = (TestObject3*)header->head->value.ptr_value;
            if (retrieved == obj) {
                std::cout << "SUCCESS: Object correctly stored in list\n";
                std::cout << "Retrieved object values: " << retrieved->slot1 << ", " << retrieved->slot2 << ", " << retrieved->slot3 << "\n";
                std::cout << "Retrieved object vtable: " << retrieved->vtable << "\n";
            } else {
                std::cout << "ERROR: Retrieved object pointer mismatch\n";
            }
        } else {
            std::cout << "ERROR: List head is null or wrong type: " << (header->head ? header->head->type : -1) << "\n";
        }
        
        std::cout << "Step 5: Creating multiple objects...\n";
        for (int i = 1; i <= 5; ++i) {
            TestObject3* obj2 = create_test_object3(i * 10);
            if (obj2) {
                BCPL_LIST_APPEND_OBJECT(header, obj2);
                std::cout << "Object " << i << " appended, list length: " << header->length << "\n";
            } else {
                std::cout << "FAILED to create object " << i << "\n";
                break;
            }
        }
        
        std::cout << "Step 6: Cleaning up scope...\n";
        HeapManager_exitScope();
        std::cout << "SUCCESS: Scope cleanup completed\n";
        
    } catch (const std::exception& e) {
        std::cout << "EXCEPTION: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cout << "UNKNOWN EXCEPTION occurred\n";
        return 1;
    }
    
    std::cout << "=== All tests completed successfully ===\n";
    return 0;
}