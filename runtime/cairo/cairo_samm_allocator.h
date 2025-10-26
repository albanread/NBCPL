#pragma once

#include <memory>
#include <utility>

// Forward declarations for BCPL heap allocation functions
extern "C" {
    void* OBJECT_HEAP_ALLOC(void* class_ptr);
    void OBJECT_HEAP_FREE(void* object_ptr);
}

/**
 * SAMM-aware Cairo object allocators
 * 
 * This namespace provides allocation functions that integrate Cairo objects
 * with BCPL's SAMM (Scope Aware Memory Management) system. All Cairo objects
 * allocated through these functions will be automatically cleaned up when
 * BCPL scopes exit.
 */
namespace CairoSAMM {
    
    /**
     * Allocate object using BCPL heap (default constructor)
     */
    template<typename T>
    T* allocate() {
        void* ptr = OBJECT_HEAP_ALLOC(nullptr);
        if (!ptr) {
            throw std::bad_alloc();
        }
        return new(ptr) T();  // Placement new
    }
    
    /**
     * Allocate object using BCPL heap (with constructor arguments)
     */
    template<typename T, typename... Args>
    T* allocate(Args&&... args) {
        void* ptr = OBJECT_HEAP_ALLOC(nullptr);
        if (!ptr) {
            throw std::bad_alloc();
        }
        return new(ptr) T(std::forward<Args>(args)...);  // Placement new with args
    }
    
    /**
     * Deallocate object using BCPL heap
     */
    template<typename T>
    void deallocate(T* obj) {
        if (obj) {
            obj->~T();  // Call destructor
            OBJECT_HEAP_FREE(obj);  // Free through BCPL heap
        }
    }
    
    /**
     * Custom deleter for unique_ptr to work with BCPL heap
     */
    template<typename T>
    struct BCPLDeleter {
        void operator()(T* obj) {
            deallocate(obj);
        }
    };
    
    /**
     * SAMM-aware unique_ptr alias
     * Uses BCPL heap allocation and automatic SAMM cleanup
     */
    template<typename T>
    using unique_ptr = std::unique_ptr<T, BCPLDeleter<T>>;
    
    /**
     * SAMM-aware make_unique equivalent
     * Creates objects allocated through BCPL heap that participate in SAMM
     */
    template<typename T, typename... Args>
    unique_ptr<T> make_unique(Args&&... args) {
        T* obj = allocate<T>(std::forward<Args>(args)...);
        return unique_ptr<T>(obj, BCPLDeleter<T>{});
    }
}