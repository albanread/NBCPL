# NEON Encoder Refactoring Plan

This document outlines a comprehensive plan to refactor the NEON reducer encoders in NewBCPL, creating a clean, verifiable, and scalable architecture by separating dispatcher logic from encoding logic.

## Executive Summary

The current system mixes dispatch logic with encoding logic, making it difficult to maintain and validate. This plan introduces a registry-based architecture with highly specific, single-purpose encoders that are individually testable and verifiable.

## Current Issues

Based on the vector encoder inventory (`vector_tidy.md`), we identified:

1. **Duplicate Functionality**: Multiple encoder systems (main encoders, VectorCodeGen, specialized generators)
2. **Inconsistent Naming**: Three different naming conventions across the codebase
3. **Scattered Logic**: Vector operations spread across multiple files and classes
4. **Missing FQUAD Support**: New 4x16-bit float type needs dedicated encoders
5. **No Verification System**: No systematic way to track encoder correctness

## Proposed Solution: Registry-Based Architecture

### Core Principles

1. **Single Responsibility**: Each encoder handles exactly one instruction/layout combination
2. **Verifiable**: Every encoder tracks its validation status
3. **Discoverable**: Central registry makes encoders easy to find and use
4. **Extensible**: Easy to add new instructions and vector layouts
5. **Safe Migration**: Fallback system ensures continuity during transition

## Implementation Plan

### 1. New Directory Structure

Create a dedicated home for all NEON reducer logic:

```
codegen/neon_reducers/
├── NeonReducerRegistry.h
├── NeonReducerRegistry.cpp
├── arm64_fminp_encoders.cpp
├── arm64_sminp_encoders.cpp
├── arm64_fmaxp_encoders.cpp
├── arm64_faddp_encoders.cpp
└── README.md
```

### 2. Naming Convention and Standards

**Convention**: `gen_neon_<instruction>_<vector_layout>`

- `<instruction>`: Lowercase ARM64 NEON instruction name (e.g., fminp, sminp)
- `<vector_layout>`: Lane format (e.g., 4s, 2s, 2d, 4h)

**Examples**:
- `gen_neon_fminp_4s(...)` - FMINP with 4x32-bit floats
- `gen_neon_fminp_2s(...)` - FMINP with 2x32-bit floats
- `gen_neon_fminp_4h(...)` - FMINP with 4x16-bit floats (for FQUAD)
- `gen_neon_sminp_4s(...)` - SMINP with 4x32-bit integers

### 3. Standardized Metadata Block

Every encoder function must include compliance tracking:

```cpp
/**
 * @brief  Generates ARM64 NEON FMINP instruction for a 4S vector layout.
 * @compliance
 * - clang_assembly_valid: false
 * - binary_encoding_valid: false
 * - test_cases: test_fminp_4s_basic.bcl, test_fminp_4s_edge.bcl
 * @notes  Performs a pairwise minimum on four 32-bit floats.
 */
void gen_neon_fminp_4s(CodeGenerator& cg, const ReductionStatement& node);
```

## Step-by-Step Implementation

### Step 1: Create the Registry Infrastructure

#### NeonReducerRegistry.h
```cpp
#pragma once
#include "../CodeGenerator.h"
#include "../../AST.h"
#include <functional>
#include <string>
#include <map>

// Define a function pointer type for our encoders
using NeonReducerEncoder = std::function<void(CodeGenerator&, const ReductionStatement&)>;

class NeonReducerRegistry {
public:
    NeonReducerRegistry();
    
    // Find an encoder for the given reducer name and vector type
    NeonReducerEncoder findEncoder(const std::string& reducer_name, const Symbol& input_symbol);
    
    // Get list of all registered encoders (for debugging/testing)
    std::vector<std::string> getRegisteredEncoders() const;
    
    // Check if a specific encoder exists
    bool hasEncoder(const std::string& reducer_name, const std::string& vector_type) const;

private:
    // The key format: "REDUCER_NAME:VECTOR_TYPE:LAYOUT"
    std::map<std::string, NeonReducerEncoder> encoder_map_;
    
    // Helper to create registry keys
    std::string makeKey(const std::string& reducer_name, const std::string& vector_type) const;
};
```

#### NeonReducerRegistry.cpp
```cpp
#include "NeonReducerRegistry.h"
#include <iostream>

// Forward declare encoder functions
void gen_neon_fminp_4s(CodeGenerator&, const ReductionStatement&);
void gen_neon_fminp_2s(CodeGenerator&, const ReductionStatement&);
void gen_neon_fminp_4h(CodeGenerator&, const ReductionStatement&); // For FQUAD
void gen_neon_sminp_4s(CodeGenerator&, const ReductionStatement&);
void gen_neon_sminp_2s(CodeGenerator&, const ReductionStatement&);

NeonReducerRegistry::NeonReducerRegistry() {
    // Register float pairwise minimum encoders
    encoder_map_["PAIRWISE_MIN:FVEC8:4S"] = gen_neon_fminp_4s;
    encoder_map_["PAIRWISE_MIN:FPAIR:2S"] = gen_neon_fminp_2s;
    encoder_map_["PAIRWISE_MIN:FQUAD:4H"] = gen_neon_fminp_4h;  // New FQUAD support
    
    // Register integer pairwise minimum encoders
    encoder_map_["PAIRWISE_MIN:VEC8:4S"] = gen_neon_sminp_4s;
    encoder_map_["PAIRWISE_MIN:PAIR:2S"] = gen_neon_sminp_2s;
    
    // Future encoders will be added here...
}

NeonReducerEncoder NeonReducerRegistry::findEncoder(const std::string& reducer_name, const Symbol& symbol) {
    std::string key = makeKey(reducer_name, symbol.getTypeAsString());
    
    auto it = encoder_map_.find(key);
    if (it != encoder_map_.end()) {
        return it->second;
    }
    
    // Log missing encoder for debugging
    std::cerr << "DEBUG: No NEON encoder found for: " << key << std::endl;
    return nullptr;
}

std::string NeonReducerRegistry::makeKey(const std::string& reducer_name, const std::string& vector_type) const {
    // Map vector types to their NEON arrangements
    std::string layout = "UNKNOWN";
    if (vector_type == "FVEC8" || vector_type == "VEC8" || vector_type == "QUAD" || vector_type == "FQUAD") {
        layout = vector_type == "FQUAD" ? "4H" : "4S";
    } else if (vector_type == "FPAIR" || vector_type == "PAIR") {
        layout = "2S";
    }
    
    return reducer_name + ":" + vector_type + ":" + layout;
}

std::vector<std::string> NeonReducerRegistry::getRegisteredEncoders() const {
    std::vector<std::string> encoders;
    for (const auto& pair : encoder_map_) {
        encoders.push_back(pair.first);
    }
    return encoders;
}

bool NeonReducerRegistry::hasEncoder(const std::string& reducer_name, const std::string& vector_type) const {
    std::string key = makeKey(reducer_name, vector_type);
    return encoder_map_.find(key) != encoder_map_.end();
}
```

### Step 2: Create First Specific Encoder

#### arm64_fminp_encoders.cpp
```cpp
// codegen/neon_reducers/arm64_fminp_encoders.cpp
#include "../CodeGenerator.h"
#include "../../AST.h"

/**
 * @brief  Generates ARM64 NEON FMINP instruction for a 4S vector layout.
 * @compliance
 * - clang_assembly_valid: false
 * - binary_encoding_valid: false
 * - test_cases: test_fminp_4s_basic.bcl
 * @notes  Processes two 8-element 32-bit float vectors (FVEC 8).
 */
void gen_neon_fminp_4s(CodeGenerator& cg, const ReductionStatement& node) {
    // 1. Get symbols and stack offsets for inputs and output
    Symbol in_sym1 = cg.getSymbol(node.input_vector1_name);
    Symbol in_sym2 = cg.getSymbol(node.input_vector2_name);
    Symbol out_sym = cg.getSymbol(node.output_variable_name);

    // 2. Load the two source vectors into q0 and q1
    cg.emit("LDR q0, [sp, #" + std::to_string(in_sym1.stack_offset) + "]");
    cg.emit("LDR q1, [sp, #" + std::to_string(in_sym2.stack_offset) + "]");

    // 3. Emit the specific NEON instruction
    cg.emit("FMINP.4S v0, v0, v1"); // The core of the encoder

    // 4. Store the result from the destination register
    cg.emit("STR q0, [sp, #" + std::to_string(out_sym.stack_offset) + "]");
}

/**
 * @brief  Generates ARM64 NEON FMINP instruction for a 2S vector layout.
 * @compliance
 * - clang_assembly_valid: false
 * - binary_encoding_valid: false
 * - test_cases: test_fminp_2s_basic.bcl
 * @notes  Processes two FPAIR (2x32-bit float) values.
 */
void gen_neon_fminp_2s(CodeGenerator& cg, const ReductionStatement& node) {
    Symbol in_sym1 = cg.getSymbol(node.input_vector1_name);
    Symbol in_sym2 = cg.getSymbol(node.input_vector2_name);
    Symbol out_sym = cg.getSymbol(node.output_variable_name);

    // Load 64-bit FPAIR values into NEON registers
    cg.emit("LDR d0, [sp, #" + std::to_string(in_sym1.stack_offset) + "]");
    cg.emit("LDR d1, [sp, #" + std::to_string(in_sym2.stack_offset) + "]");

    // FMINP.2S: Pairwise minimum of two 2-element float vectors
    cg.emit("FMINP.2S v0, v0, v1");

    // Store 64-bit result
    cg.emit("STR d0, [sp, #" + std::to_string(out_sym.stack_offset) + "]");
}

/**
 * @brief  Generates ARM64 NEON FMINP instruction for a 4H vector layout.
 * @compliance
 * - clang_assembly_valid: false
 * - binary_encoding_valid: false
 * - test_cases: test_fminp_4h_fquad.bcl
 * @notes  Processes FQUAD (4x16-bit float) values - NEW for FQUAD support.
 */
void gen_neon_fminp_4h(CodeGenerator& cg, const ReductionStatement& node) {
    Symbol in_sym1 = cg.getSymbol(node.input_vector1_name);
    Symbol in_sym2 = cg.getSymbol(node.input_vector2_name);
    Symbol out_sym = cg.getSymbol(node.output_variable_name);

    // Load 64-bit FQUAD values (4x16-bit floats) into NEON registers
    cg.emit("LDR d0, [sp, #" + std::to_string(in_sym1.stack_offset) + "]");
    cg.emit("LDR d1, [sp, #" + std::to_string(in_sym2.stack_offset) + "]");

    // FMINP.4H: Pairwise minimum of 4x16-bit float vectors
    cg.emit("FMINP.4H v0, v0, v1");

    // Store 64-bit result  
    cg.emit("STR d0, [sp, #" + std::to_string(out_sym.stack_offset) + "]");
}
```

### Step 3: Update Main Dispatcher

#### Modified cg_visit_ReductionStatement.cpp
```cpp
// In codegen/cg_impl/cg_visit_ReductionStatement.cpp

#include "../neon_reducers/NeonReducerRegistry.h"

// Static instance of the registry
static NeonReducerRegistry neon_reducer_registry;

void CodeGenerator::visit(ReductionStatement& node) {
    Symbol input_sym = getSymbol(node.input_vector1_name);

    // 1. Try to find a new, specific encoder first
    NeonReducerEncoder encoder = neon_reducer_registry.findEncoder(
        node.reducer->name, input_sym
    );

    if (encoder) {
        // --- NEW PATH ---
        // A specific, validated encoder was found. Use it.
        debug_print("Using validated NEON encoder for " + node.reducer->name);
        encoder(*this, node);
        
    } else {
        // --- OLD PATH (Fallback) ---
        // No specific encoder yet. Use the old monolithic function.
        // This ensures the compiler keeps working during the transition.
        std::cerr << "WARN: Using fallback encoder for " << node.reducer->name 
                  << " on " << input_sym.getTypeAsString() << std::endl;
        
        // Call existing implementation
        visit_reduction_statement_legacy(node);
    }
}

// Keep old implementation as fallback during transition
void CodeGenerator::visit_reduction_statement_legacy(ReductionStatement& node) {
    // Your existing switch statement on vector type goes here
    // This will be removed once all encoders are migrated
}
```

### Step 4: Validation and Testing Process

#### Validation Workflow
1. **Compile Test**: Ensure the code compiles without errors
2. **Assembly Check**: Verify generated assembly matches expected output
3. **Binary Test**: Run compiled binary and check results
4. **Update Metadata**: Mark encoder as validated in comments

#### Example Test Case (test_fminp_4h_fquad.bcl)
```bcpl
// Test case for FQUAD pairwise minimum
LET main() BE {
    LET a = FQUAD(1.0, 2.0, 3.0, 4.0)
    LET b = FQUAD(4.0, 1.0, 2.0, 5.0)
    LET result = PAIRWISE_MIN(a, b)
    
    // Expected: FQUAD(1.0, 1.0, 2.0, 4.0)
    WRITEF("Result: %f %f %f %f\n", 
           result.first, result.second, result.third, result.fourth)
}
```

## Migration Strategy

### Phase 1: Foundation (Week 1)
- [ ] Create directory structure
- [ ] Implement NeonReducerRegistry infrastructure
- [ ] Create first encoder (gen_neon_fminp_4s)
- [ ] Update dispatcher with fallback system
- [ ] Test that existing functionality still works

### Phase 2: Core Float Operations (Week 2)
- [ ] Add remaining FMINP encoders (2S, 4H for FQUAD)
- [ ] Add FMAXP encoders (4S, 2S, 4H)
- [ ] Add FADDP encoders
- [ ] Validate each encoder as implemented

### Phase 3: Integer Operations (Week 3)
- [ ] Add SMINP encoders (4S, 2S)
- [ ] Add SMAXP encoders  
- [ ] Add ADDP encoders
- [ ] Test integer vector operations

### Phase 4: FQUAD Integration (Week 4)
- [ ] Complete all 4H (16-bit) encoders
- [ ] Test FQUAD operations thoroughly
- [ ] Update type system integration
- [ ] Add FQUAD-specific test cases

### Phase 5: Cleanup (Week 5)
- [ ] Remove old monolithic functions
- [ ] Remove fallback path
- [ ] Add comprehensive test suite
- [ ] Update documentation

## Error Handling Strategy

### Registry Errors
```cpp
// Add to NeonReducerRegistry
NeonReducerEncoder findEncoderWithFallback(
    const std::string& reducer_name, 
    const Symbol& symbol,
    bool& used_fallback
) {
    auto encoder = findEncoder(reducer_name, symbol);
    if (encoder) {
        used_fallback = false;
        return encoder;
    }
    
    // Try to find a compatible fallback
    used_fallback = true;
    return findCompatibleFallback(reducer_name, symbol);
}
```

### Encoder Validation
```cpp
// Add validation helpers
class EncoderValidator {
public:
    static bool validateAssembly(const std::string& encoder_name, 
                               const std::string& generated_asm);
    static bool validateBinary(const std::string& test_file);
};
```

## Benefits of This Approach

### 1. **Maintainability**
- Each encoder is self-contained and focused
- Easy to locate and modify specific functionality
- Clear separation between dispatch and encoding logic

### 2. **Testability** 
- Individual encoders can be tested in isolation
- Validation status is tracked and visible
- Systematic approach to correctness verification

### 3. **Extensibility**
- Adding new instructions is straightforward
- FQUAD support integrates naturally
- Future vector types can be added easily

### 4. **Debuggability**
- Clear naming makes debugging easier
- Registry provides visibility into available encoders
- Fallback system aids in troubleshooting

### 5. **Performance**
- Registry lookup is fast (hash map)
- No overhead from complex dispatch logic
- Direct function calls to specific encoders

## Future Enhancements

### 1. **Automatic Code Generation**
Consider generating encoders from templates:
```cpp
template<typename VectorType, int Lanes>
void gen_neon_fminp(CodeGenerator& cg, const ReductionStatement& node);
```

### 2. **Runtime Encoder Selection**
For advanced optimizations:
```cpp
class RuntimeEncoderSelector {
public:
    NeonReducerEncoder selectOptimalEncoder(
        const std::string& reducer_name,
        const RuntimeVectorInfo& info
    );
};
```

### 3. **Cross-Platform Support**
Extend to other architectures:
```cpp
class ArchitectureAwareRegistry {
    std::map<Architecture, std::unique_ptr<EncoderRegistry>> registries_;
};
```

## Conclusion

This refactoring plan provides a solid foundation for maintainable, testable, and extensible NEON encoder management. The registry-based architecture cleanly separates concerns while providing a safe migration path. The systematic validation approach ensures correctness, and the design naturally accommodates the new FQUAD vector type.

The phased implementation approach allows for gradual migration while maintaining compiler functionality throughout the process.

---

*Document Version: 1.0*  
*Created: December 2024*  
*Purpose: NEON Encoder Refactoring Implementation Guide*