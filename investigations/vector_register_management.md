# Vector Register Management Strategy

**Date**: December 2024  
**Feature**: Scalable Q-register management for vector operations  
**Target**: ARM64 NEON optimization with RegisterManager authority  
**Status**: Design specification for implementation  

## Overview

This document outlines a comprehensive strategy for managing ARM64 Q-registers in vector reduction operations, with the **RegisterManager as the single source of truth** for all register allocation decisions. The design addresses scalability from small vectors (optimal unrolling) to arbitrarily large vectors (chunked processing) while maintaining constant register pressure.

## Core Design Principles

### 1. **RegisterManager Authority** 🏛️
- **Single Source of Truth**: All register allocation/deallocation goes through RegisterManager
- **Consistent Interface**: Use RegisterManager APIs exclusively, no direct register manipulation
- **Pressure Monitoring**: RegisterManager tracks availability and handles spilling decisions
- **Type Safety**: Proper Q-register vs D-register vs X-register management

### 2. **Adaptive Strategy Based on Vector Size** 📏
- **Small Vectors (1-4 PAIRS)**: Loop unrolling with dedicated registers
- **Medium Vectors (5-32 PAIRS)**: Register reuse with single allocation
- **Large Vectors (33+ PAIRS)**: Chunked processing with memory optimization

### 3. **Constant Register Pressure** ⚖️
- **Maximum 3-4 Q-registers** allocated regardless of vector size
- **Predictable Resource Usage**: No surprise register exhaustion
- **Graceful Degradation**: Fallback strategies when registers unavailable

### 4. **Scalar Fallback Support** 🔄
- **--no-neon Flag**: Complete scalar implementations for all operations
- **Testing & Validation**: Scalar fallback enables correctness verification
- **Portability**: Ensures code works on non-NEON ARM64 or debugging environments
- **Performance Baseline**: Provides measurement baseline for SIMD optimizations

## Vector Size Thresholds

```cpp
class VectorRegisterStrategy {
private:
    static constexpr size_t UNROLL_THRESHOLD = 4;     // <= 4 pairs: full unroll
    static constexpr size_t REUSE_THRESHOLD = 32;     // <= 32 pairs: register reuse  
    static constexpr size_t CHUNK_SIZE = 16;          // 16 pairs per chunk for large vectors
    
public:
    enum class Strategy {
        UNROLL,    // Maximum performance, no loops
        REUSE,     // Constant registers, single loop
        CHUNKED,   // Streaming processing, memory-optimized
        SCALAR     // No NEON - pure scalar operations
    };
    
    Strategy selectStrategy(size_t vector_size, bool neon_enabled) const {
        if (!neon_enabled) return Strategy::SCALAR;  // Force scalar mode
        if (vector_size <= UNROLL_THRESHOLD) return Strategy::UNROLL;
        if (vector_size <= REUSE_THRESHOLD) return Strategy::REUSE;
        return Strategy::CHUNKED;
    }
};
```

## Strategy 1: Loop Unrolling (Small Vectors)

### **Target**: 1-4 PAIRS (2-8 elements)
### **Benefit**: Zero loop overhead, maximum ILP (Instruction Level Parallelism)

```cpp
void generateUnrolledReduction(const std::vector<ExprPtr>& args, ReductionOp op, size_t size) {
    // RegisterManager authority: Request all needed registers upfront
    std::vector<std::string> q_regs;
    size_t num_chunks = (size + 1) / 2;  // Round up for pairs
    
    // Request registers from RegisterManager
    for (size_t i = 0; i < num_chunks * 3; ++i) {  // 3 regs per chunk (left, right, result)
        std::string q_reg = register_manager_.acquire_q_scratch_reg(code_generator_);
        if (q_reg.empty()) {
            // RegisterManager says no more registers - fallback to reuse strategy
            release_acquired_registers(q_regs);
            generateRegisterReuseReduction(args, op, size);
            return;
        }
        q_regs.push_back(q_reg);
    }
    
    // Fully unrolled processing - each pair gets dedicated registers
    for (size_t i = 0; i < size; i += 2) {
        size_t chunk_idx = i / 2;
        std::string q_left = q_regs[chunk_idx * 3];
        std::string q_right = q_regs[chunk_idx * 3 + 1];  
        std::string q_result = q_regs[chunk_idx * 3 + 2];
        
        // No loops - straight-line code for maximum performance
        emit_load_pair(q_left, left_addr, i * 8);
        emit_load_pair(q_right, right_addr, i * 8);
        emit_neon_operation(q_result, q_left, q_right, op);
        emit_store_pair(q_result, result_addr, i * 8);
    }
    
    // RegisterManager authority: Return all registers
    release_acquired_registers(q_regs);
}
```

### **Performance Characteristics**:
- **Instruction Count**: Minimal (no loop setup/teardown)
- **Register Pressure**: High but bounded by threshold
- **Cache Behavior**: Excellent (all data likely in L1)
- **Pipeline Utilization**: Optimal (no branches)

## Strategy 2: Register Reuse (Medium Vectors)

### **Target**: 5-32 PAIRS (10-64 elements)  
### **Benefit**: Constant register usage, predictable performance

```cpp
void generateRegisterReuseReduction(const std::vector<ExprPtr>& args, ReductionOp op, size_t size) {
    // RegisterManager authority: Request minimal register set
    RegisterSet regs;
    if (!register_manager_.try_acquire_reduction_set(regs)) {
        // RegisterManager can't provide 3 Q-registers - use fallback
        generateSpilledReduction(args, op, size);
        return;
    }
    
    // Single loop with register reuse - constant register pressure
    for (size_t i = 0; i < size; i += 2) {
        size_t offset = i * 8;
        
        // Reuse same 3 registers for every iteration
        emit_load_pair(regs.q_left, left_addr, offset);
        emit_load_pair(regs.q_right, right_addr, offset);
        emit_neon_operation(regs.q_result, regs.q_left, regs.q_right, op);
        emit_store_pair(regs.q_result, result_addr, offset);
        
        // Optional: Prefetch next iteration
        if (i + 4 < size) {
            emit_prefetch(left_addr, offset + 32);
            emit_prefetch(right_addr, offset + 32);
        }
    }
    
    // RegisterManager authority: Single release call
    register_manager_.release_reduction_set(regs);
}
```

### **Performance Characteristics**:
- **Register Pressure**: Constant (3 Q-registers)
- **Memory Bandwidth**: Good (streaming access pattern)
- **Cache Behavior**: Excellent for vectors up to 32 PAIRS (~256 bytes)
- **Predictability**: Linear scaling with vector size

## Strategy 3: Chunked Processing (Large Vectors)

### **Target**: 33+ PAIRS (66+ elements)
### **Benefit**: Handles arbitrary size, memory bandwidth optimization

```cpp
void generateChunkedReduction(const std::vector<ExprPtr>& args, ReductionOp op, size_t size) {
    // RegisterManager authority: Same 3 registers for entire operation
    RegisterSet regs;
    if (!register_manager_.try_acquire_reduction_set(regs)) {
        generateFallbackReduction(args, op, size);
        return;
    }
    
    // Process in cache-friendly chunks
    for (size_t chunk_start = 0; chunk_start < size; chunk_start += CHUNK_SIZE) {
        size_t chunk_end = std::min(chunk_start + CHUNK_SIZE, size);
        
        // Prefetch entire chunk into cache
        emit_chunk_prefetch(left_addr, right_addr, chunk_start * 8, (chunk_end - chunk_start) * 8);
        
        // Process chunk with hot cache
        for (size_t i = chunk_start; i < chunk_end; i += 2) {
            size_t offset = i * 8;
            
            emit_load_pair(regs.q_left, left_addr, offset);
            emit_load_pair(regs.q_right, right_addr, offset);
            emit_neon_operation(regs.q_result, regs.q_left, regs.q_right, op);
            emit_store_pair(regs.q_result, result_addr, offset);
        }
    }
    
    register_manager_.release_reduction_set(regs);
}
```

### **Performance Characteristics**:
- **Register Pressure**: Constant (3 Q-registers)  
- **Memory Pattern**: Optimized for L1/L2 cache hierarchy
- **Scalability**: Linear scaling to GB-sized vectors
- **Power Efficiency**: Minimizes cache misses and memory stalls

## RegisterManager Interface Extensions

### **New Methods for Vector Operations**

```cpp
class RegisterManager {
public:
    // Vector-specific register allocation
    struct ReductionRegisterSet {
        std::string q_left;
        std::string q_right; 
        std::string q_result;
        bool valid;
    };
    
    // Atomic allocation of reduction register set
    bool try_acquire_reduction_set(ReductionRegisterSet& regs);
    void release_reduction_set(const ReductionRegisterSet& regs);
    
    // Query register availability before committing to strategy
    size_t get_available_q_register_count() const;
    bool can_support_unroll_strategy(size_t vector_size) const;
    
    // Register pressure management
    void request_register_spill_if_needed();
    void restore_spilled_registers();
    
    // Performance monitoring
    struct RegisterMetrics {
        size_t q_registers_allocated;
        size_t spill_events;
        size_t allocation_failures;
    };
    RegisterMetrics get_vector_register_metrics() const;
};
```

### **Integration with Existing RegisterManager**

```cpp
bool RegisterManager::try_acquire_reduction_set(ReductionRegisterSet& regs) {
    // Check NEON availability first
    if (!neon_enabled_) {
        return false;  // Force scalar fallback when NEON disabled
    }
    
    // Check availability before committing
    if (get_available_q_register_count() < 3) {
        return false;  // Let caller handle fallback
    }
    
    // Atomic allocation - either get all 3 or fail
    regs.q_left = acquire_q_scratch_reg(code_generator);
    if (regs.q_left.empty()) {
        return false;
    }
    
    regs.q_right = acquire_q_scratch_reg(code_generator);
    if (regs.q_right.empty()) {
        release_fp_register(regs.q_left);
        return false;
    }
    
    regs.q_result = acquire_q_scratch_reg(code_generator);
    if (regs.q_result.empty()) {
        release_fp_register(regs.q_left);
        release_fp_register(regs.q_right);
        return false;
    }
    
    regs.valid = true;
    return true;
}

// NEON enable/disable control
void RegisterManager::set_neon_enabled(bool enabled) {
    neon_enabled_ = enabled;
}

bool RegisterManager::is_neon_enabled() const {
    return neon_enabled_;
}
```

## Strategy 4: Scalar Fallback (--no-neon)

### **Target**: Any vector size when NEON disabled
### **Benefit**: Guaranteed compatibility, testing validation, debugging support

```cpp
void generateScalarReduction(const std::vector<ExprPtr>& args, ReductionOp op, size_t size) {
    // Pure scalar implementation - no Q-registers needed
    // Uses only X-registers managed by RegisterManager
    
    std::string x_left_base = code_generator_.get_variable_register(left_var->name);
    std::string x_right_base = code_generator_.get_variable_register(right_var->name);  
    std::string x_result_base = allocateResultVector(VarType::PAIRS, size);
    
    // Process each PAIR element individually with scalar operations
    for (size_t pair_idx = 0; pair_idx < size; ++pair_idx) {
        size_t pair_offset = pair_idx * 8;  // 2 words per PAIR
        
        // Process first element of PAIR
        std::string x_temp1 = register_manager_.acquire_scratch_reg(code_generator_);
        std::string x_temp2 = register_manager_.acquire_scratch_reg(code_generator_);
        std::string x_result = register_manager_.acquire_scratch_reg(code_generator_);
        
        // Load first elements: left[pair].first, right[pair].first  
        emit(Encoder::create_ldr_imm(x_temp1, x_left_base, pair_offset));
        emit(Encoder::create_ldr_imm(x_temp2, x_right_base, pair_offset));
        
        // Scalar operation on first elements
        emit_scalar_operation(x_result, x_temp1, x_temp2, op);
        emit(Encoder::create_str_imm(x_result, x_result_base, pair_offset));
        
        // Load second elements: left[pair].second, right[pair].second
        emit(Encoder::create_ldr_imm(x_temp1, x_left_base, pair_offset + 4));
        emit(Encoder::create_ldr_imm(x_temp2, x_right_base, pair_offset + 4));
        
        // Scalar operation on second elements  
        emit_scalar_operation(x_result, x_temp1, x_temp2, op);
        emit(Encoder::create_str_imm(x_result, x_result_base, pair_offset + 4));
        
        register_manager_.release_register(x_temp1);
        register_manager_.release_register(x_temp2);
        register_manager_.release_register(x_result);
    }
    
    code_generator_.expression_result_reg_ = x_result_base;
}

void emit_scalar_operation(const std::string& result, const std::string& left, 
                          const std::string& right, ReductionOp op) {
    switch (op) {
        case ReductionOp::MIN:
            emit(Encoder::create_cmp_reg(left, right));
            emit(Encoder::create_csel(result, left, right, "LT"));  // result = (left < right) ? left : right
            break;
        case ReductionOp::MAX:
            emit(Encoder::create_cmp_reg(left, right));
            emit(Encoder::create_csel(result, left, right, "GT"));  // result = (left > right) ? left : right
            break;
        case ReductionOp::SUM:
            emit(Encoder::create_add_reg(result, left, right));
            break;
    }
}
```

### **Performance Characteristics**:
- **No Q-registers**: Uses only X-registers through RegisterManager
- **Element-wise processing**: 2x slower than SIMD (processes elements individually)
- **Predictable**: Linear scaling, no cache complexity
- **Compatible**: Works on any ARM64 system regardless of NEON support

### **Use Cases**:
- **--no-neon command line flag**: Force scalar mode for testing
- **Debugging**: Isolate SIMD vs algorithmic issues
- **Validation**: Verify NEON results against scalar reference
- **Compatibility**: Ensure code works on minimal ARM64 systems

## Fallback Strategy: Register Pressure Handling

### **When RegisterManager Cannot Provide Q-Registers**

```cpp
void generateFallbackReduction(const std::vector<ExprPtr>& args, ReductionOp op, size_t size) {
    // Strategy 1: Use X-registers with scalar operations (when Q-registers unavailable)
    std::string x_temp1 = register_manager_.acquire_scratch_reg(code_generator_);
    std::string x_temp2 = register_manager_.acquire_scratch_reg(code_generator_);
    std::string x_result = register_manager_.acquire_scratch_reg(code_generator_);
    
    for (size_t i = 0; i < size; i += 2) {
        // Load PAIR elements individually
        emit(Encoder::create_ldr_imm(x_temp1, left_addr, i * 8));
        emit(Encoder::create_ldr_imm(x_temp2, right_addr, i * 8));
        
        // Scalar comparison/arithmetic operations
        emit_scalar_reduction_operation(x_result, x_temp1, x_temp2, op);
        
        // Store result
        emit(Encoder::create_str_imm(x_result, result_addr, i * 8));
    }
    
    register_manager_.release_register(x_temp1);
    register_manager_.release_register(x_temp2);
    register_manager_.release_register(x_result);
}
```

## Memory Optimization Techniques

### **Cache-Aware Prefetching**

```cpp
void emit_smart_prefetch(const std::string& base_addr, size_t current_offset, size_t vector_size) {
    static constexpr size_t L1_CACHE_LINE = 64;    // ARM64 typical
    static constexpr size_t PREFETCH_DISTANCE = L1_CACHE_LINE * 2;
    
    if (current_offset + PREFETCH_DISTANCE < vector_size * 8) {
        // Prefetch to L1 cache
        emit(Encoder::create_prfm("PLDL1KEEP", base_addr, current_offset + PREFETCH_DISTANCE));
    }
}

void emit_chunk_prefetch(const std::string& left_addr, const std::string& right_addr, 
                        size_t offset, size_t chunk_size) {
    // Prefetch entire chunk for both source vectors
    for (size_t pf_offset = 0; pf_offset < chunk_size; pf_offset += 64) {
        emit(Encoder::create_prfm("PLDL1KEEP", left_addr, offset + pf_offset));
        emit(Encoder::create_prfm("PLDL1KEEP", right_addr, offset + pf_offset));
    }
}
```

## Performance Monitoring and Tuning

### **Runtime Metrics Collection**

```cpp
struct VectorOperationMetrics {
    size_t vectors_processed;
    size_t total_elements;
    size_t unroll_operations;
    size_t reuse_operations; 
    size_t chunked_operations;
    size_t fallback_operations;
    std::chrono::nanoseconds total_time;
    
    double get_elements_per_second() const {
        return total_elements / (total_time.count() / 1e9);
    }
};
```

### **Adaptive Threshold Tuning**

```cpp
class AdaptiveThresholds {
private:
    size_t unroll_threshold_ = 4;
    size_t reuse_threshold_ = 32;
    VectorOperationMetrics metrics_;
    
public:
    // Adjust thresholds based on observed performance
    void update_thresholds_from_metrics() {
        if (metrics_.fallback_operations > metrics_.total_operations * 0.1) {
            // Too many fallbacks - reduce unroll threshold
            unroll_threshold_ = std::max(1UL, unroll_threshold_ - 1);
        }
        
        if (metrics_.get_elements_per_second() < expected_throughput) {
            // Performance below target - adjust strategy boundaries
            reuse_threshold_ = std::min(64UL, reuse_threshold_ + 4);
        }
    }
};
```

## Implementation Priority

### **Phase 1: Scalar Fallback Implementation** (High Priority)
- [ ] Add `--no-neon` command line flag parsing
- [ ] Implement complete scalar reduction operations  
- [ ] Add NEON enable/disable to RegisterManager
- [ ] Ensure scalar fallback works for all reduction types

### **Phase 2: RegisterManager Extensions** (High Priority)
- [ ] Implement `try_acquire_reduction_set()` and `release_reduction_set()`
- [ ] Add Q-register availability queries  
- [ ] Integrate NEON enable/disable with register allocation
- [ ] Add scalar-only register acquisition methods

### **Phase 3: Strategy Implementation** (High Priority)  
- [ ] Modify current register reuse approach in `reductions.cpp`
- [ ] Add vector size detection and strategy dispatch
- [ ] Implement unroll strategy for small vectors
- [ ] Integrate scalar strategy selection

### **Phase 4: Advanced Optimizations** (Medium Priority)
- [ ] Add chunked processing for large vectors
- [ ] Implement memory prefetching strategies
- [ ] Add performance metrics collection
- [ ] Cross-validate NEON vs scalar results

### **Phase 5: Tuning and Validation** (Medium Priority)
- [ ] Benchmark NEON vs scalar performance ratios
- [ ] Validate correctness using scalar reference implementations
- [ ] Implement adaptive threshold tuning
- [ ] Add automated testing with --no-neon flag

## Risk Mitigation

### **RegisterManager Integration Risks**
- **Risk**: Changes to RegisterManager break existing code
- **Mitigation**: Add new methods without modifying existing API
- **Fallback**: Keep current approach working during transition

### **Scalar Fallback Risks**
- **Risk**: Scalar implementation has different behavior than NEON
- **Mitigation**: Extensive cross-validation testing
- **Fallback**: Use scalar as reference implementation for correctness

### **Performance Regression Risks**  
- **Risk**: Strategy overhead outweighs SIMD benefits
- **Mitigation**: Comprehensive benchmarking at each phase
- **Fallback**: Always maintain scalar fallback path

### **Memory Access Pattern Risks**
- **Risk**: Poor cache behavior with chunked processing
- **Mitigation**: Extensive testing on real ARM64 hardware
- **Fallback**: Tunable chunk sizes based on target CPU

## Expected Performance Gains

### **NEON vs Scalar Performance Comparison**

#### **Small Vectors (1-4 PAIRS)**
- **NEON**: 4x SIMD + 20-30% unrolling = ~5-6x speedup
- **Scalar**: Baseline performance (1x)
- **Use Cases**: NEON for performance, scalar for validation

#### **Medium Vectors (5-32 PAIRS)**
- **NEON**: 4x SIMD + optimal bandwidth = ~4x speedup
- **Scalar**: Baseline performance (1x)  
- **Use Cases**: NEON for production, scalar for debugging

#### **Large Vectors (33+ PAIRS)**
- **NEON**: 4x SIMD + cache optimization = ~3-4x speedup
- **Scalar**: Baseline performance (1x)
- **Use Cases**: NEON for throughput, scalar for correctness verification

#### **Command Line Control**
```bash
# Force NEON optimizations (default)
./NewBCPL --run vector_test.bcl

# Force scalar fallback for testing
./NewBCPL --no-neon --run vector_test.bcl

# Benchmark comparison
./NewBCPL --benchmark --run vector_test.bcl  # Tests both modes
```

---

**Conclusion**: This design provides **scalable, predictable performance** across all vector sizes while maintaining **RegisterManager authority** and **constant resource usage**. The adaptive strategy ensures optimal performance for each use case while gracefully handling resource constraints.