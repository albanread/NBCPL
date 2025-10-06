# NEON Reducers - Modular Vector Instruction Encoders

This directory contains the new modular NEON reduction instruction encoders for NewBCPL. These encoders provide a clean separation between dispatch logic and encoding logic, making the codebase more maintainable and testable.

## Architecture Overview

### Core Components

- **NeonReducerRegistry**: Central dispatcher that maps reduction operations to specific encoders
- **Encoder Files**: Individual files containing highly specific, single-purpose encoders
- **Validation System**: Compliance tracking for each encoder function

### Design Principles

1. **Single Responsibility**: Each encoder handles exactly one instruction/layout combination
2. **Verifiable**: Every encoder tracks its validation status in comments
3. **Discoverable**: Central registry makes encoders easy to find and use
4. **Extensible**: Easy to add new instructions and vector layouts
5. **Safe Migration**: Fallback system ensures continuity during transition

## Directory Structure

```
neon_reducers/
├── NeonReducerRegistry.h          # Registry header with function declarations
├── NeonReducerRegistry.cpp        # Registry implementation and encoder registration
├── arm64_fminp_encoders.cpp       # Float minimum pairwise operations
├── arm64_sminp_encoders.cpp       # Signed integer minimum operations  
├── arm64_fmaxp_encoders.cpp       # Float maximum pairwise operations
├── arm64_faddp_encoders.cpp       # Float addition pairwise operations
├── arm64_addp_encoders.cpp        # Integer addition pairwise operations
└── README.md                      # This file
```

## Naming Convention

All encoder functions follow the pattern: `gen_neon_<instruction>_<vector_layout>`

### Examples:
- `gen_neon_fminp_4s()` - FMINP with 4x32-bit floats
- `gen_neon_fminp_2s()` - FMINP with 2x32-bit floats  
- `gen_neon_fminp_4h()` - FMINP with 4x16-bit floats (for FQUAD)
- `gen_neon_sminp_4s()` - SMINP with 4x32-bit integers

## Vector Type Mapping

| NewBCPL Type | NEON Arrangement | Description |
|-------------|------------------|-------------|
| FQUAD       | 4H              | 4x16-bit floats |
| FPAIR       | 2S              | 2x32-bit floats |
| PAIR        | 2S              | 2x32-bit integers |
| QUAD        | 4S              | 4x32-bit integers |
| FVEC8       | 4S              | 8-element float vector (processed in chunks) |
| VEC8        | 4S              | 8-element integer vector (processed in chunks) |

## Validation System

Each encoder function includes a standardized compliance block:

```cpp
/**
 * @brief  Description of the encoder function
 * @compliance
 * - clang_assembly_valid: false/true
 * - binary_encoding_valid: false/true  
 * - test_cases: list_of_test_files.bcl
 * @notes  Additional implementation notes
 */
```

### Validation Stages:
1. **Compilation**: Code compiles without errors
2. **Assembly**: Generated assembly matches expected output  
3. **Binary**: Compiled binary produces correct results
4. **Metadata Update**: Mark encoder as validated in comments

## Currently Registered Encoders

### Float Operations
- `PAIRWISE_MIN:FVEC8:4S` → `gen_neon_fminp_4s` ✗
- `PAIRWISE_MIN:FPAIR:2S` → `gen_neon_fminp_2s` ✗
- `PAIRWISE_MIN:FQUAD:4H` → `gen_neon_fminp_4h` ✗ (NEW - FQUAD support)
- `PAIRWISE_MAX:FVEC8:4S` → `gen_neon_fmaxp_4s` ✗
- `PAIRWISE_MAX:FPAIR:2S` → `gen_neon_fmaxp_2s` ✗
- `PAIRWISE_MAX:FQUAD:4H` → `gen_neon_fmaxp_4h` ✗ (NEW - FQUAD support)
- `PAIRWISE_ADD:FVEC8:4S` → `gen_neon_faddp_4s` ✗
- `PAIRWISE_ADD:FPAIR:2S` → `gen_neon_faddp_2s` ✗
- `PAIRWISE_ADD:FQUAD:4H` → `gen_neon_faddp_4h` ✗ (NEW - FQUAD support)

### Integer Operations
- `PAIRWISE_MIN:VEC8:4S` → `gen_neon_sminp_4s` ✗
- `PAIRWISE_MIN:PAIR:2S` → `gen_neon_sminp_2s` ✗
- `PAIRWISE_ADD:VEC8:4S` → `gen_neon_addp_4s` ✗
- `PAIRWISE_ADD:PAIR:2S` → `gen_neon_addp_2s` ✗

Legend: ✗ = Not yet validated, ✓ = Validated

## Usage

### From Code Generator
```cpp
#include "codegen/neon_reducers/NeonReducerRegistry.h"

static NeonReducerRegistry neon_reducer_registry;

void NewCodeGenerator::visit(ReductionStatement& node) {
    Symbol input_sym = getSymbol(node.input_vector1_name);
    
    // Try to find a validated encoder
    NeonReducerEncoder encoder = neon_reducer_registry.findEncoder(
        node.reducer->name, input_sym
    );
    
    if (encoder) {
        // Use the new validated encoder
        encoder(*this, node);
    } else {
        // Fall back to legacy implementation
        visit_reduction_statement_legacy(node);
    }
}
```

### Registry Information
```cpp
// Get all registered encoders
auto encoders = registry.getRegisteredEncoders();

// Check if specific encoder exists
bool exists = registry.hasEncoder("PAIRWISE_MIN", "FQUAD");

// Get encoder description
std::string info = registry.getEncoderInfo("PAIRWISE_MIN", "FQUAD");
```

## FQUAD Integration

This architecture provides native support for the new FQUAD (4x16-bit float) vector type:

### New 4H Encoders:
- `gen_neon_fminp_4h()` - FQUAD pairwise minimum
- `gen_neon_fmaxp_4h()` - FQUAD pairwise maximum  
- `gen_neon_faddp_4h()` - FQUAD pairwise addition

These encoders use the "4H" NEON arrangement for 4x16-bit float operations.

## Implementation Status

### Phase 1: Foundation ✓
- [x] Directory structure created
- [x] Registry infrastructure implemented
- [x] Core encoder files created
- [x] Integration points defined

### Phase 2: Core Encoders ⚠️
- [ ] Validate FMINP encoders (4S, 2S, 4H)
- [ ] Validate FMAXP encoders (4S, 2S, 4H)  
- [ ] Validate FADDP encoders (4S, 2S, 4H)
- [ ] Validate SMINP encoders (4S, 2S)
- [ ] Validate ADDP encoders (4S, 2S)

### Phase 3: Integration ⚠️
- [ ] Update main dispatcher to use registry
- [ ] Add fallback mechanism
- [ ] Test that existing functionality still works
- [ ] Add comprehensive test cases

### Phase 4: Validation ⏳
- [ ] Compile and test each encoder individually
- [ ] Validate assembly output
- [ ] Validate binary execution
- [ ] Update compliance metadata

### Phase 5: Cleanup ⏳
- [ ] Remove old monolithic functions
- [ ] Remove fallback mechanisms  
- [ ] Add performance benchmarks
- [ ] Complete documentation

## Adding New Encoders

### 1. Create Encoder Function
```cpp
void gen_neon_<instruction>_<layout>(NewCodeGenerator& cg, const ReductionStatement& node) {
    // Implementation with standardized metadata block
}
```

### 2. Register in NeonReducerRegistry.cpp
```cpp
registerEncoder("OPERATION_NAME", "VECTOR_TYPE", gen_neon_<instruction>_<layout>,
                "Human-readable description");
```

### 3. Add Forward Declaration in NeonReducerRegistry.h
```cpp
void gen_neon_<instruction>_<layout>(NewCodeGenerator&, const ReductionStatement&);
```

### 4. Validate and Update Metadata
Run tests and update the compliance block once validated.

## Error Handling

The registry provides robust error handling:
- **Missing Encoders**: Registry returns nullptr for unknown encoders
- **Fallback Support**: Main dispatcher can fall back to legacy implementation
- **Debug Logging**: Registry logs missing encoders for debugging

## Performance Considerations

- Registry lookup is O(1) using std::map
- No overhead from complex dispatch logic
- Direct function calls to specific encoders
- Efficient register allocation within each encoder

## Future Enhancements

### Automatic Code Generation
Consider generating encoders from templates for common patterns.

### Runtime Optimization
Add runtime encoder selection based on CPU features or vector sizes.

### Cross-Platform Support
Extend architecture to support other SIMD instruction sets (AVX, etc.).

## Testing

Each encoder should have dedicated test cases:
- Basic functionality tests
- Edge case tests  
- Performance benchmarks
- Assembly output validation

Test files follow the pattern: `test_<instruction>_<layout>_<scenario>.bcl`

---

*This architecture provides a solid foundation for maintainable, testable, and extensible NEON encoder management while naturally supporting the new FQUAD vector type.*