# Existing HeapManager Architecture Documentation

This document thoroughly documents the existing HeapManager implementation before any SAMM modifications, providing a complete reference for the baseline allocation system.

## Overview

The HeapManager is a singleton class that provides thread-safe heap allocation tracking with double-free detection. It manages four types of allocations: objects, vectors, strings, and lists.

## Class Structure

### Header File: `HeapManager.h`

```cpp
class HeapManager {
public:
    // Thread-safe tracking map: base address -> metadata
    std::unordered_map<void*, HeapBlock> heap_blocks_;
    
    // Bloom filter for double-free detection (~0.1% false positive rate)
    DoubleFreeBloomFilter recently_freed_addresses_;
    size_t bloom_filter_items_added_;
    
    // Thread safety mutex
    mutable std::mutex heap_mutex_;

private:
    // Allocation metrics
    size_t totalBytesAllocated;
    size_t totalBytesFreed;
    size_t totalVectorsAllocated;
    size_t totalObjectsAllocated;
    size_t totalStringsAllocated;
    size_t totalVectorsFreed;
    size_t totalStringsFreed;
    size_t totalDoubleFreeAttempts;
    size_t totalBloomFilterFalsePositives;
    
    bool traceEnabled;
    static HeapManager* instance;

public:
    // Core allocation methods
    void* allocVec(size_t numElements);
    void* allocObject(size_t size);
    void* allocString(size_t numChars);
    void* allocList();
    void free(void* payload);
    
    // Singleton access
    static HeapManager& getInstance();
    
    // Configuration and debugging
    void setTraceEnabled(bool enabled);
    bool isTracingEnabled() const;
    static void traceLog(const char* format, ...);
    void dumpHeap() const;
    void dumpHeapSignalSafe();
    void printMetrics() const;
};
```

## Allocation Methods

### 1. Vector Allocation (`allocVec`)

**File**: `Heap_allocVec.cpp`

**Purpose**: Allocates BCPL vectors (arrays of 64-bit words)

**Memory Layout**:
```
[uint64_t length][uint64_t data[0]][uint64_t data[1]]...[uint64_t data[n-1]]
^-- metadata     ^-- payload returned to caller
```

**Implementation**:
```cpp
void* HeapManager::allocVec(size_t numElements) {
    size_t totalSize = sizeof(uint64_t) + numElements * sizeof(uint64_t);
    void* ptr;
    
    // 16-byte aligned allocation
    if (posix_memalign(&ptr, 16, totalSize) != 0) {
        BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "allocVec", "System posix_memalign failed");
        return nullptr;
    }

    // Initialize metadata: first 8 bytes = element count
    uint64_t* vec = static_cast<uint64_t*>(ptr);
    vec[0] = numElements;

    // Thread-safe tracking
    {
        std::lock_guard<std::mutex> lock(heap_mutex_);
        heap_blocks_.emplace(ptr, HeapBlock{ALLOC_VEC, ptr, totalSize, nullptr, nullptr});
        
        // Update signal-safe shadow array if tracing enabled
        if (traceEnabled) {
            g_shadow_heap_blocks[g_shadow_heap_index] = {ALLOC_VEC, ptr, totalSize, nullptr, nullptr};
            g_shadow_heap_index = (g_shadow_heap_index + 1) % MAX_HEAP_BLOCKS;
        }
    }
    
    // Update metrics
    totalBytesAllocated += totalSize;
    totalVectorsAllocated++;
    update_alloc_metrics(totalSize, ALLOC_VEC);

    return static_cast<void*>(vec + 1); // Return payload pointer (skip length)
}
```

**Key Points**:
- Returns pointer to payload (after length header)
- Length stored in first 8 bytes of allocation
- 16-byte aligned for performance
- Thread-safe tracking with mutex

### 2. String Allocation (`allocString`)

**File**: `Heap_allocString.cpp`

**Purpose**: Allocates BCPL strings (arrays of 32-bit characters)

**Memory Layout**:
```
[uint64_t length][uint32_t char[0]][uint32_t char[1]]...[uint32_t char[n-1]][uint32_t \0]
^-- metadata     ^-- payload returned to caller                              ^-- null terminator
```

**Implementation**:
```cpp
void* HeapManager::allocString(size_t numChars) {
    size_t totalSize = sizeof(uint64_t) + (numChars + 1) * sizeof(uint32_t);
    void* ptr;
    
    if (posix_memalign(&ptr, 16, totalSize) != 0) {
        BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "allocString", "System posix_memalign failed");
        return nullptr;
    }

    // Initialize metadata and null terminator
    uint64_t* str = static_cast<uint64_t*>(ptr);
    str[0] = numChars; // Character count (not including null terminator)
    uint32_t* payload = reinterpret_cast<uint32_t*>(str + 1);
    payload[numChars] = 0; // Null terminator

    // Thread-safe tracking (same pattern as vector)
    {
        std::lock_guard<std::mutex> lock(heap_mutex_);
        heap_blocks_.emplace(ptr, HeapBlock{ALLOC_STRING, ptr, totalSize, nullptr, nullptr});
        // ... signal-safe tracking ...
    }
    
    // Update metrics
    totalBytesAllocated += totalSize;
    totalStringsAllocated++;
    update_alloc_metrics(totalSize, ALLOC_STRING);

    return static_cast<void*>(payload); // Return payload pointer (skip length)
}
```

**Key Points**:
- Returns pointer to character array (after length header)
- Length stored as character count (excluding null terminator)
- Automatic null termination
- 32-bit characters (BCPL standard)

### 3. Object Allocation (`allocObject`)

**File**: `Heap_allocObject.cpp`

**Purpose**: Allocates raw memory for BCPL objects (no metadata prefix)

**Memory Layout**:
```
[object data...] 
^-- payload returned to caller (no prefix)
```

**Implementation**:
```cpp
void* HeapManager::allocObject(size_t size) {
    void* ptr;
    
    if (posix_memalign(&ptr, 16, size) != 0) {
        BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "allocObject", "System posix_memalign failed");
        return nullptr;
    }
    
    memset(ptr, 0, size); // Zero-initialize

    // Thread-safe tracking
    {
        std::lock_guard<std::mutex> lock(heap_mutex_);
        heap_blocks_.emplace(ptr, HeapBlock{ALLOC_OBJECT, ptr, size, nullptr, nullptr});
        // ... signal-safe tracking ...
    }

    // Update metrics
    totalBytesAllocated += size;
    totalObjectsAllocated++;
    update_alloc_metrics(size, ALLOC_OBJECT);
    
    return ptr; // Return raw pointer (no offset)
}
```

**Key Points**:
- Returns raw allocated pointer (no metadata prefix)
- Zero-initialized memory
- Size is exactly what caller requested
- Used for objects with vtables set by code generator

### 4. List Allocation (`allocList`)

**File**: `Heap_allocList.cpp`

**Purpose**: Allocates BCPL list headers from a freelist

**Memory Layout**:
```
[ListHeader structure]
^-- managed by freelist allocator
```

**Implementation**:
```cpp
void* HeapManager::allocList() {
    // Delegate to freelist allocator
    ListHeader* header = getHeaderFromFreelist();
    if (!header) {
        safe_print("Error: List header allocation failed (freelist empty)\n");
        return nullptr;
    }

    // Track in HeapManager for consistency
    {
        std::lock_guard<std::mutex> lock(heap_mutex_);
        heap_blocks_.emplace(header, HeapBlock{ALLOC_LIST, header, sizeof(ListHeader), nullptr, nullptr});
        // ... signal-safe tracking ...
    }

    totalBytesAllocated += sizeof(ListHeader);
    return header;
}
```

**Key Points**:
- Uses external freelist allocator (`getHeaderFromFreelist()`)
- Only tracks allocation, doesn't manage the actual allocation
- Fixed size (`sizeof(ListHeader)`)
- Separate allocation strategy for performance

## High-Level Object Allocation API

### `OBJECT_HEAP_ALLOC` Function

**File**: `HeapManager.cpp` (extern "C" function)

**Purpose**: High-level object allocation with vtable setup

**Signature**: `void* OBJECT_HEAP_ALLOC(void* class_ptr)`

**Implementation**:
```cpp
extern "C" void* OBJECT_HEAP_ALLOC(void* class_ptr) {
    size_t object_size = 24;  // Default: 8 bytes vtable + 16 bytes members
    
    // Allocate raw memory using HeapManager
    void* obj = Heap_allocObject(object_size);
    
    if (obj) {
        memset(obj, 0, object_size); // Redundant but safe
        
        // Set vtable pointer if provided
        if (class_ptr != nullptr) {
            *(void**)obj = class_ptr; // First 8 bytes = vtable pointer
        }
    }
    
    return obj;
}
```

**Key Points**:
- Wraps `allocObject` with vtable setup
- Hardcoded 24-byte default size (should use class metadata)
- Sets vtable pointer at offset 0 if `class_ptr` provided
- Called by code generator for `NEW` expressions

## C Wrapper Functions

**File**: `heap_c_wrappers.cpp`

Provides C linkage for use by assembly code and runtime:

```cpp
extern "C" void* Heap_allocVec(size_t numElements) {
    return HeapManager::getInstance().allocVec(numElements);
}

extern "C" void* Heap_allocObject(size_t size) {
    return HeapManager::getInstance().allocObject(size);
}

extern "C" void Heap_free(void* payload) {
    HeapManager::getInstance().free(payload);
}
```

## Data Structures

### HeapBlock (heap_manager_defs.h)

```cpp
typedef struct {
    AllocType type;            // ALLOC_VEC, ALLOC_STRING, ALLOC_OBJECT, ALLOC_LIST
    void* address;             // Base address of allocation
    size_t size;               // Total size in bytes
    const char* function_name; // Allocating function (for debugging)
    const char* variable_name; // Variable name (for debugging)
} HeapBlock;
```

### AllocType Enumeration

```cpp
typedef enum {
    ALLOC_UNKNOWN = 0,
    ALLOC_VEC,         // Vector allocation
    ALLOC_STRING,      // String allocation  
    ALLOC_OBJECT,      // Object allocation
    ALLOC_GENERIC,     // Generic allocation
    ALLOC_FREE,        // Freed block marker
    ALLOC_LIST         // List allocation
} AllocType;
```

## Thread Safety

- **Mutex Protection**: All tracking operations use `heap_mutex_`
- **Signal-Safe Tracking**: Optional shadow array for crash diagnostics
- **Lock Scope**: Minimal - only around map operations and metrics updates
- **Exception Safety**: RAII lock guards prevent deadlocks

## Memory Layout Summary

| Type | Prefix | Payload Return | Notes |
|------|--------|----------------|--------|
| Vector | 8-byte length | Offset +8 | Length = element count |
| String | 8-byte length | Offset +8 | Length = char count, null-terminated |
| Object | None | Offset +0 | Raw allocation, vtable set by caller |
| List | None | Offset +0 | Managed by freelist, just tracked |

## Integration Points

1. **Code Generator**: Calls `OBJECT_HEAP_ALLOC` for `NEW` expressions
2. **Runtime Functions**: Use `Heap_allocVec`, `Heap_allocObject` via C wrappers  
3. **Assembly Code**: Links to C wrapper functions
4. **Error Handling**: All allocations report failures via `BCPL_SET_ERROR`
5. **Debugging**: Trace logging and signal-safe tracking for crash analysis

## Current Limitations

1. **Fixed Object Size**: `OBJECT_HEAP_ALLOC` uses hardcoded 24 bytes
2. **No Size Recovery**: Cannot determine allocation size from pointer alone (vectors/strings store length, objects don't)
3. **No Automatic Cleanup**: Memory must be explicitly freed
4. **Limited Metadata**: Function/variable names not currently used
5. **No Fragmentation Handling**: Uses system allocator directly

This baseline provides the foundation for SAMM integration while maintaining compatibility with existing code generation patterns.