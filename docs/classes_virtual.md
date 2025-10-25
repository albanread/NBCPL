# VIRTUAL Methods Implementation Plan for NewBCPL

## Overview

This document outlines the implementation plan for adding VIRTUAL method support to the NewBCPL compiler. The implementation will provide full object-oriented programming capabilities with runtime polymorphism, supporting both JIT and AOT compilation modes with different optimization strategies.

## Current State Analysis

### ✅ Already Implemented (70% complete)
- **AST Support**: `is_virtual` and `is_final` flags exist in `FunctionDeclaration` and `RoutineDeclaration`
- **TokenType**: `Virtual` and `Final` tokens are defined in `Lexer.h`
- **Parser**: Already recognizes and parses VIRTUAL/FINAL modifiers in `parse_function_or_routine_body()`
- **ClassTable**: Virtual method tracking, vtable slots, inheritance chain traversal
- **Partial Code Generation**: Vtable initialization, SUPER calls, virtual RELEASE calls

### ❌ Missing Critical Pieces (30% remaining)
- **Lexer Keywords**: VIRTUAL/FINAL not mapped in `keywords_` map
- **Virtual Method Calls**: `MemberAccessExpression` code generation missing
- **Dispatch Logic**: No differentiation between virtual vs. non-virtual calls
- **PIC Optimization**: Polymorphic Inline Cache for JIT performance

## Implementation Phases

### Phase 1: Core Virtual Method Support (1-2 days)

#### 1.1 Fix Lexer Keywords (5 minutes)
**File**: `Lexer.cpp`
**Change**: Add to `keywords_` map around line 40:
```cpp
{"VIRTUAL", TokenType::Virtual},
{"FINAL", TokenType::Final},
```

#### 1.2 Implement MemberAccessExpression Code Generation (4-6 hours)
**File**: `NewCodeGenerator.cpp`
**Add**: Complete `visit(MemberAccessExpression& node)` method:

```cpp
void NewCodeGenerator::visit(MemberAccessExpression& node) {
    debug_print("Generating member access for: " + node.member_name);
    
    // Generate code for object expression to get 'this' pointer
    generate_expression_code(*node.object_expr);
    std::string obj_reg = expression_result_reg_;
    
    // Determine class name and lookup method info
    std::string class_name = get_class_name_for_expression(node.object_expr.get());
    if (class_name.empty()) {
        throw std::runtime_error("Cannot determine class for member access: " + node.member_name);
    }
    
    ClassMethodInfo* method_info = class_table_->lookup_class_method(class_name, node.member_name);
    if (!method_info) {
        throw std::runtime_error("Method not found: " + class_name + "::" + node.member_name);
    }
    
    if (method_info->is_virtual) {
        generate_virtual_method_call(obj_reg, method_info);
    } else {
        generate_direct_method_call(method_info);
    }
}

void NewCodeGenerator::generate_virtual_method_call(const std::string& obj_reg, ClassMethodInfo* method_info) {
    if (is_jit_mode_) {
        generate_jit_virtual_call(obj_reg, method_info);
    } else {
        generate_aot_virtual_call(obj_reg, method_info);
    }
}

void NewCodeGenerator::generate_aot_virtual_call(const std::string& obj_reg, ClassMethodInfo* method_info) {
    // Standard vtable dispatch for AOT compilation
    std::string vtable_reg = register_manager_.acquire_scratch_reg(*this);
    std::string method_reg = register_manager_.acquire_scratch_reg(*this);
    
    // Load vtable pointer: LDR vtable_reg, [obj_reg, #0]
    emit(Encoder::create_ldr_imm(vtable_reg, obj_reg, 0, "Load vtable pointer"));
    
    // Load method address: LDR method_reg, [vtable_reg, #slot_offset]
    size_t vtable_offset = method_info->vtable_slot * 8;
    emit(Encoder::create_ldr_imm(method_reg, vtable_reg, vtable_offset, "Load virtual method address"));
    
    // Store method address for caller
    expression_result_reg_ = method_reg;
    
    register_manager_.release_register(vtable_reg);
    // Note: method_reg will be released by caller after BLR
}

void NewCodeGenerator::generate_direct_method_call(ClassMethodInfo* method_info) {
    // Generate direct address to non-virtual method
    std::string method_reg = register_manager_.acquire_scratch_reg(*this);
    emit(Encoder::create_adrp(method_reg, method_info->qualified_name));
    emit(Encoder::create_add_literal(method_reg, method_reg, method_info->qualified_name));
    expression_result_reg_ = method_reg;
}
```

#### 1.3 Update Method Headers (30 minutes)
**File**: `NewCodeGenerator.h`
**Add**: Method declarations:
```cpp
private:
    void generate_virtual_method_call(const std::string& obj_reg, ClassMethodInfo* method_info);
    void generate_aot_virtual_call(const std::string& obj_reg, ClassMethodInfo* method_info);
    void generate_jit_virtual_call(const std::string& obj_reg, ClassMethodInfo* method_info);
    void generate_direct_method_call(ClassMethodInfo* method_info);
```

#### 1.4 Testing (2-3 hours)
**Create**: Test file `tests/test_virtual_basic.b`:
```bcpl
CLASS Animal
    VIRTUAL ROUTINE Speak() = WRITEF("Generic animal sound*N")
    ROUTINE Move() = WRITEF("Animal moves*N")

CLASS Dog : Animal  
    ROUTINE Speak() = WRITEF("Woof!*N")

CLASS Cat : Animal
    ROUTINE Speak() = WRITEF("Meow!*N")

LET Start() = VALOF {
    LET dog = NEW Dog()
    LET cat = NEW Cat()
    
    // Test virtual dispatch
    dog.Speak()    // Should print "Woof!"
    cat.Speak()    // Should print "Meow!"
    
    // Test non-virtual dispatch  
    dog.Move()     // Should print "Animal moves"
    cat.Move()     // Should print "Animal moves"
    
    RESULTIS 0
}
```

### Phase 2: Advanced Virtual Method Features (3-5 days)

#### 2.1 Abstract Methods Support
**Enhancement**: Support pure virtual methods:
```bcpl
CLASS Shape
    VIRTUAL ROUTINE Area() = 0      // Pure virtual - must be overridden
    VIRTUAL ROUTINE Perimeter() = 0 // Pure virtual - must be overridden
```

**Implementation**:
- Parser: Detect `= 0` syntax for pure virtual methods
- ClassTable: Track abstract classes (classes with pure virtual methods)  
- Semantic Analysis: Prevent instantiation of abstract classes
- Code Generation: Generate runtime error for pure virtual calls

#### 2.2 Virtual Inheritance Chain Resolution
**Enhancement**: Proper method resolution order and override validation:
- Validate virtual method signatures match parent signatures
- Detect and report virtual method conflicts in multiple inheritance
- Implement proper method resolution order (MRO)

#### 2.3 FINAL Method Support
**Enhancement**: Complete FINAL method implementation:
- Prevent overriding of FINAL methods in derived classes
- Allow devirtualization optimization for FINAL methods
- Generate compile-time errors for FINAL override attempts

### Phase 3: JIT PIC Optimization (2-3 weeks)

#### 3.1 Basic Inline Cache Infrastructure
**Files**: `NewCodeGenerator.cpp`, `JITExecutor.cpp`
**Implementation**: Monomorphic inline cache:

```cpp
struct PICEntry {
    uint64_t cached_type_id;
    uint64_t cached_method_addr;
    uint64_t call_count;
    bool is_active;
};

void NewCodeGenerator::generate_jit_virtual_call(const std::string& obj_reg, ClassMethodInfo* method_info) {
    if (!enable_pic_optimization_) {
        return generate_aot_virtual_call(obj_reg, method_info); // Fallback
    }
    
    // Allocate PIC cache entry
    std::string cache_label = "pic_cache_" + std::to_string(next_pic_id_++);
    pic_entries_[cache_label] = std::make_unique<PICEntry>();
    
    // Generate PIC stub
    generate_pic_monomorphic_stub(obj_reg, method_info, cache_label);
}

void NewCodeGenerator::generate_pic_monomorphic_stub(const std::string& obj_reg, 
                                                    ClassMethodInfo* method_info, 
                                                    const std::string& cache_label) {
    // Load cached type ID
    std::string cached_type_reg = register_manager_.acquire_scratch_reg(*this);
    emit(Encoder::create_adrp(cached_type_reg, cache_label + "_type"));
    emit(Encoder::create_ldr_imm(cached_type_reg, cached_type_reg, 0));
    
    // Load actual type from object (assume type ID at offset 8)
    std::string actual_type_reg = register_manager_.acquire_scratch_reg(*this);  
    emit(Encoder::create_ldr_imm(actual_type_reg, obj_reg, 8, "Load actual type ID"));
    
    // Compare types
    emit(Encoder::create_cmp(cached_type_reg, actual_type_reg));
    
    std::string cache_hit_label = cache_label + "_hit";
    std::string cache_miss_label = cache_label + "_miss";
    
    // Branch on comparison
    emit(Encoder::create_branch_conditional("BEQ", cache_hit_label));
    emit(Encoder::create_branch(cache_miss_label));
    
    // Cache hit path - direct call
    emit_label(cache_hit_label);
    std::string cached_method_reg = register_manager_.acquire_scratch_reg(*this);
    emit(Encoder::create_adrp(cached_method_reg, cache_label + "_method"));
    emit(Encoder::create_ldr_imm(cached_method_reg, cached_method_reg, 0));
    expression_result_reg_ = cached_method_reg;
    
    // Skip miss handler
    std::string exit_label = cache_label + "_exit";
    emit(Encoder::create_branch(exit_label));
    
    // Cache miss path - vtable lookup + cache update
    emit_label(cache_miss_label);
    generate_pic_cache_miss_handler(obj_reg, method_info, cache_label);
    
    emit_label(exit_label);
    
    register_manager_.release_register(cached_type_reg);
    register_manager_.release_register(actual_type_reg);
}
```

#### 3.2 Polymorphic Inline Cache
**Enhancement**: Support multiple cached types per call site:
- Track top N most frequent types at each call site
- Generate chained type checks for polymorphic sites
- Fall back to vtable lookup for mega-morphic sites

#### 3.3 Adaptive Recompilation
**Enhancement**: Runtime profiling and recompilation:
- Collect call frequency statistics
- Identify hot virtual call sites
- Trigger recompilation with better type information
- Support method specialization for monomorphic sites

#### 3.4 PIC Performance Monitoring
**Files**: `RuntimeManager.cpp`, `JITExecutor.cpp`
**Implementation**: 
- Collect PIC hit/miss statistics
- Report optimization effectiveness
- Provide runtime debugging for PIC behavior

## Architecture Considerations

### JIT vs AOT Compilation Modes

#### JIT Mode Advantages for PIC:
- **Runtime Profiling**: Observe actual call patterns
- **Code Patching**: Modify generated machine code in real-time
- **Adaptive Optimization**: Recompile with better type information
- **Immediate Feedback**: Call site behavior visible immediately

#### AOT Mode Limitations:
- **No Runtime Profile**: Cannot predict call patterns at compile-time
- **Immutable Code**: Cannot patch call sites after compilation
- **Static Linking**: Final addresses unknown until link time
- **No Feedback Loop**: Cannot adapt to runtime behavior

### Recommended Strategy:
1. **Both Modes**: Implement basic virtual dispatch with vtable lookups
2. **JIT Only**: Add PIC optimization for hot virtual call sites
3. **AOT Future**: Consider Profile-Guided Optimization if needed

## Performance Expectations

### Virtual Call Performance (relative to direct calls):
| Method | JIT Performance | AOT Performance |
|--------|-----------------|-----------------|
| Direct Call | 1.0x (baseline) | 1.0x (baseline) |
| Virtual Dispatch | 2.5-3.0x | 2.5-3.0x |
| **JIT PIC (monomorphic)** | **1.1-1.2x** | N/A |
| **JIT PIC (polymorphic)** | **1.5-2.0x** | N/A |
| AOT with guards | N/A | 2.0-2.5x |

### Expected PIC Hit Rates:
- **Monomorphic sites**: 95-99% hit rate
- **Polymorphic sites**: 85-95% hit rate  
- **Mega-morphic sites**: 60-80% hit rate

## Testing Strategy

### Test Categories:
1. **Basic Virtual Dispatch**: Simple inheritance with virtual methods
2. **Multiple Inheritance**: Diamond inheritance patterns
3. **Abstract Classes**: Pure virtual methods and instantiation prevention
4. **Performance Tests**: Micro-benchmarks for virtual call overhead
5. **PIC Tests**: Call site behavior under different usage patterns

### Benchmark Suite:
```bcpl
// Performance test for virtual calls
CLASS TestBase
    VIRTUAL ROUTINE HotMethod() = counter := counter + 1

CLASS TestDerived : TestBase
    ROUTINE HotMethod() = counter := counter + 2

LET BenchmarkVirtualCalls() = VALOF {
    LET objects = VEC 1000
    // Fill with mixed TestBase/TestDerived instances
    
    LET start_time = GET_TIME()
    FOR i = 0 TO 999999 DO {
        LET obj = objects!(i REM 1000)
        obj.HotMethod()  // Should be optimized by PIC
    }
    LET end_time = GET_TIME()
    
    WRITEF("Virtual calls took %d ms*N", end_time - start_time)
    RESULTIS 0
}
```

## Implementation Timeline

### Week 1: Core Implementation
- [ ] Fix lexer keywords (Day 1)
- [ ] Implement basic virtual dispatch (Days 1-3)
- [ ] Create test suite (Days 4-5)

### Week 2: Advanced Features  
- [ ] Abstract method support (Days 1-2)
- [ ] FINAL method validation (Days 3-4)
- [ ] Inheritance validation (Day 5)

### Weeks 3-4: JIT PIC (Phase 1)
- [ ] Monomorphic inline cache (Week 3)
- [ ] Cache miss handling (Week 4)
- [ ] Performance validation (Week 4)

### Weeks 5-6: JIT PIC (Phase 2)
- [ ] Polymorphic inline cache (Week 5)
- [ ] Adaptive recompilation (Week 6)
- [ ] Performance monitoring (Week 6)

## Risk Mitigation

### Potential Issues:
1. **Register Pressure**: PIC generates additional register usage
   - **Mitigation**: Careful register management, spill rare cases
2. **Code Size**: PIC stubs increase code size per virtual call site
   - **Mitigation**: Selective PIC application based on profiling
3. **Complexity**: PIC adds significant implementation complexity
   - **Mitigation**: Implement incrementally, thorough testing

### Fallback Strategy:
- All virtual call optimizations can fall back to standard vtable dispatch
- AOT mode provides baseline functionality without PIC complexity
- JIT mode can disable PIC optimization via runtime flag

## Success Metrics

### Functionality:
- [ ] All virtual method features work in both JIT and AOT modes
- [ ] Performance regression < 5% for non-virtual code
- [ ] Virtual call overhead < 3x direct calls (without PIC)

### Performance (JIT with PIC):
- [ ] Monomorphic virtual calls < 1.2x direct call overhead
- [ ] PIC hit rate > 90% for typical object-oriented code
- [ ] Hot virtual call sites approach direct call performance

### Compatibility:
- [ ] Existing non-OOP code unaffected
- [ ] Assembly output remains clean for AOT mode
- [ ] No breaking changes to current class system

This implementation plan provides a clear roadmap for adding comprehensive VIRTUAL method support to NewBCPL, with particular attention to the performance advantages possible in JIT mode while maintaining compatibility with AOT compilation.