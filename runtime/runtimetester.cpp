#include <iostream>
#include <cstdint>
#include "HeapManager/heap_manager_defs.h" // Ensure this is included first
#include "HeapManager.h"

// Forward declaration of the global OBJECT_HEAP_ALLOC function
extern "C" void* OBJECT_HEAP_ALLOC(void* class_ptr);

// Dummy structure to simulate a BCPL object for testing
// In a real BCPL object, the first few words might contain metadata
// like a class pointer or size. For this test, we'll just simulate
// some data members.
struct DummyBCPLObject {
    // Typically, a pointer to the class definition or vtable might be here
    void* class_definition_ptr; 
    int32_t member1;
    int32_t member2;
    // ... other members as needed
};

// A dummy class definition to pass to OBJECT_HEAP_ALLOC
// In a real scenario, this would be a pointer to the BCPL compiler's
// internal representation of a class.
struct DummyClassDefinition {
    const char* name;
    size_t size_in_bytes;
    // ... other class metadata
};

int main() {
    std::cout << "--- Runtime Tester: OBJECT_HEAP_ALLOC ---\n";

    // Create a dummy class definition
    DummyClassDefinition dummyClass = {"TestClass", sizeof(DummyBCPLObject)};

    std::cout << "Attempting to allocate a DummyBCPLObject of size " << dummyClass.size_in_bytes << " bytes...\n";

    // Call the OBJECT_HEAP_ALLOC function
    void* allocated_ptr = OBJECT_HEAP_ALLOC(&dummyClass); // Pass address of dummy class definition

    if (allocated_ptr == nullptr) {
        std::cout << "TEST FAILED: OBJECT_HEAP_ALLOC returned nullptr. Allocation failed.\n";
        return 1; // Indicate failure
    } else {
        std::cout << "TEST PASSED: OBJECT_HEAP_ALLOC returned a valid pointer: " << allocated_ptr << "\n";

        // Attempt to use the allocated memory
        DummyBCPLObject* obj = static_cast<DummyBCPLObject*>(allocated_ptr);

        std::cout << "Attempting to write to object members...\n";
        obj->class_definition_ptr = &dummyClass; // Set the dummy class pointer
        obj->member1 = 123;
        obj->member2 = 456;

        std::cout << "Verifying object members...\n";
        if (obj->member1 == 123 && obj->member2 == 456) {
            std::cout << "TEST PASSED: Object members written and read correctly.\n";
        } else {
            std::cout << "TEST FAILED: Object members mismatch (member1: " << obj->member1 << ", member2: " << obj->member2 << ").\n";
            return 1; // Indicate failure
        }
    }

    std::cout << "--- Runtime Tester: OBJECT_HEAP_ALLOC Complete ---\n";
    return 0; // Indicate success
}
