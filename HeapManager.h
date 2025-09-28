#ifndef HEAPMANAGER_H
#define HEAPMANAGER_H

#include <cstddef>
#include <cstdint>
#include "runtime.h"

class HeapManager {
private:

    HeapBlock heapBlocks[MAX_HEAP_BLOCKS]; // Fixed-size array for signal safety
    size_t heapIndex;

    // Metrics
    size_t totalBytesAllocated;
    size_t totalBytesFreed;
    size_t totalVectorsAllocated;
    size_t totalStringsAllocated;
    size_t totalVectorsFreed;
    size_t totalStringsFreed;

    // Trace flag
    bool traceEnabled = false;

    HeapManager();

public:
    static HeapManager& getInstance();

    // Enable or disable tracing
    void setTrace(bool enable);

    // Trace flag accessor
    bool isTracingEnabled() const { return traceEnabled; }

    // Trace log function
    // void traceLog(const char* format, ...) const; // REMOVED

    // Allocation methods
    void* allocVec(size_t numElements);
    void* allocString(size_t numChars);

    // Resizing methods
    void* resizeVec(void* payload, size_t newNumElements);
    void* resizeString(void* payload, size_t newNumChars);

    // Free memory
    void free(void* payload);

    // Metrics and debugging
    void printMetrics() const;
    void dumpHeap() const;
    void dumpHeapSignalSafe();
};

#endif // HEAPMANAGER_H
