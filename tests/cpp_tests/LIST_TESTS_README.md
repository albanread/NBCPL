# List Tests Tool Documentation

## 📁 **Location & Build**
```bash
cd NewBCPL/tests/cpp_tests/
make clean && make list_tests
./list_tests
```

## 🔧 **Build Requirements**
- **Runtime Library**: Must run `./buildruntime` from project root first
- **Links Against**: `libbcpl_runtime_sdl2_static.a` (unified runtime with SDL2)
- **Dependencies**: HeapManager, freelist system, SAMM scope management

## 🧪 **Test Suite Overview**
The tool runs **8 comprehensive tests** covering BCPL list operations, memory management, and performance:

### **1. List Creation/Destruction**
- Creates 1000-element integer list
- Validates data integrity and proper cleanup
- Tests basic `create_int_list()` and scope exit

### **2. List Append**  
- Creates 10-element list, appends 10 more
- Verifies append operation and data consistency
- Tests direct pointer manipulation for appends

### **3. Scope-based Freeing (SAMM)**
- Tests SAMM scope entry/exit with list creation
- Validates automatic cleanup on scope boundaries
- Demonstrates scope-based memory management

### **4. List Performance (Creation/Destruction)**
- Performance test: 100K element list creation/cleanup
- Reports creation and SAMM cleanup timing
- Measures large-scale allocation performance

### **5. Small Lists Creation (5000 × 6 items)**
- Stress test: 5000 small lists with 6 items each  
- Tests freelist scaling under rapid allocation pressure
- Validates list header and atom counting

### **6. Small List Append Speed (5000 × 6 appends)**
- Performance test: Create small lists + append to each
- Reports append rate (typically >1M appends/second)
- Tests allocation/append performance at scale

### **7. Large List Append Speed (5000 × 6 appends to 1K lists)**
- Performance test: Create 1K-element lists + append to each
- Compares large vs small list append performance  
- Identifies potential O(n) vs O(1) performance issues

### **8. Memory Leak Check**
- Stress test: 1000 iterations of 1K-element list creation
- Each iteration uses separate SAMM scope
- Designed for valgrind/leak detection tools

## 📊 **Metrics & Reporting**

### **Performance Metrics:**
- List creation/destruction timing (ms)
- Append rates (appends/second)  
- SAMM cleanup timing (ms)

### **Memory Management:**
- **List Element Tracking**: Headers/atoms created vs destroyed
- **HeapManager Stats**: Bytes allocated/freed, cleanup operations
- **Bloom Filter Metrics**: Fixed 12MB filter capacity and usage
- **Freelist Scaling**: Adaptive chunk size scaling and efficiency

### **Key Performance Indicators:**
- **Freelist reuse rate**: Should be >99.99%
- **Append performance**: >1M appends/second for small lists
- **Memory efficiency**: Minimal heap allocations vs total requests
- **Cleanup time**: SAMM scope cleanup timing

## 🚀 **Expected Results**

### **Typical Performance:**
```
Small List Append Speed: ~1.04M appends/second
Large List Append Speed: ~889K appends/second  
Freelist reuse rate: 99.9999%
Total cleanup time: ~63ms
```

### **Memory Efficiency:**
```
Total node requests: ~6.2M
Nodes reused from freelist: ~6.195M  
Heap allocations: <50 replenishments
Current freelist node count: ~5.1M (available for reuse)
```

## 🔧 **Architecture & Design**

### **SAMM Integration:**
- Each test uses `HeapManager_enterScope()` / `HeapManager_exitScope()`
- **ListHeaders** are tracked in SAMM scopes
- On scope exit: ListAtoms → freelist, ListHeaders → freelist
- **Automatic cleanup** prevents memory leaks

### **Freelist System:**
- **Direct allocation**: `getNodeFromFreelist()`, `getHeaderFromFreelist()`
- **Adaptive scaling**: Chunk sizes scale 4x under pressure (1K→4K→16K→64K→128K)
- **99.99% reuse**: Nearly all requests satisfied from freelist pool
- **Thread-safe**: Mutex-protected freelist operations

### **Memory Management Layers:**
1. **SAMM Scope**: Automatic scope-based cleanup
2. **Freelist**: High-performance node pooling and reuse
3. **HeapManager**: Traditional malloc/free with tracking
4. **Bloom Filter**: 12MB fixed-size double-free detection

## 🐛 **Debugging & Validation**

### **Running Under Valgrind:**
```bash
valgrind --leak-check=full ./list_tests
```

### **Key Validation Points:**
- No memory leaks reported by valgrind
- List headers created ≈ List headers destroyed (accounting for freelist pooling)
- SAMM cleanup time > 0ms (proves scope cleanup working)
- Freelist reuse rate > 99.9%

### **Common Issues:**
- **Low reuse rate**: Indicates scope management not working
- **High cleanup time**: May indicate cleanup inefficiency  
- **Memory leaks**: Usually scope entry/exit imbalance

## 📈 **Performance Benchmarking**

This tool serves as a comprehensive benchmark for:
- **List operation performance** at scale
- **Memory management efficiency** across different allocation patterns
- **SAMM scope-based cleanup** validation
- **Freelist adaptive scaling** behavior under pressure

The results provide baseline performance metrics for BCPL list operations and validate the correctness of the integrated memory management system.