# NoPrep Attribute Implementation

## Overview

This document describes the implementation of the `nopeep` attribute for instructions in the NewBCPL compiler's peephole optimizer. The `nopeep` attribute allows developers to mark specific instructions that should not be optimized by any peephole optimization patterns.

## Background

The `nopeep` attribute was added to prevent critical instructions from being modified by the peephole optimizer. This is particularly important for:

- Bounds checking instructions that must remain intact for safety
- Debug-specific instructions that need to be preserved
- Performance-critical code sections where specific instruction sequences are required
- Instructions that have side effects that must not be eliminated

## Implementation Details

### Instruction Structure

The `nopeep` attribute is implemented as a boolean field in the `Instruction` struct located in `Encoder.h`:

```cpp
struct Instruction {
    // ... other fields ...
    
    // Peephole optimizer control
    bool nopeep = false; // If true, this instruction should not be optimized by peephole optimizer
    
    // ... other fields ...
};
```

### Pattern Modifications

All peephole optimization patterns have been updated to check for the `nopeep` attribute before attempting to match or transform instructions. The pattern is consistent across all patterns:

1. **Single instruction patterns**: Check if the instruction has `nopeep = true`
2. **Multi-instruction patterns**: Check if any instruction in the pattern has `nopeep = true`

If any instruction in a potential match has the `nopeep` attribute set to `true`, the pattern returns `{false, 0}` to indicate no match.

### Files Modified

The following files were updated to implement `nopeep` support:

#### Core Optimizer Files
- `PeepholeOptimizer.cpp` - Updated all pattern creation functions
- `PeepholePatterns.cpp` - Updated all patterns in this file

#### Pattern-Specific Files
- `optimizer/patterns/AddressFusion.cpp` - Address fusion patterns
- `optimizer/patterns/ConstantFolding.cpp` - Constant folding patterns
- `optimizer/patterns/ControlFlow.cpp` - Control flow optimization patterns
- `optimizer/patterns/MemoryOptimization.cpp` - Memory optimization patterns
- `optimizer/patterns/StrengthReduction.cpp` - Strength reduction patterns

### Patterns Updated

The following peephole optimization patterns now respect the `nopeep` attribute:

1. **Branch Chaining Pattern** - `createBranchChainingPattern()`
2. **Redundant Load Elimination** - `createRedundantLoadEliminationPattern()`
3. **Redundant Move Pattern** - `createRedundantMovePattern()`
4. **Copy Propagation Pattern** - `createCopyPropagationPattern()`
5. **Load After Store Pattern** - `createLoadAfterStorePattern()`
6. **Dead Store Pattern** - `createDeadStorePattern()`
7. **Redundant Compare Pattern** - `createRedundantComparePattern()`
8. **Constant Folding Pattern** - `createConstantFoldingPattern()`
9. **Strength Reduction Pattern** - `createStrengthReductionPattern()`
10. **Identity Operation Elimination** - `createIdentityOperationEliminationPattern()`
11. **Redundant Store Pattern** - `createRedundantStorePattern()`
12. **ADR Fusion Pattern** - `createAdrFusionPattern()`
13. **Self Move Elimination** - `createSelfMoveEliminationPattern()`
14. **Multiply by Power of Two** - `createMultiplyByPowerOfTwoPattern()`
15. **Divide by Power of Two** - `createDivideByPowerOfTwoPattern()`
16. **Compare Zero Branch** - `createCompareZeroBranchPattern()`
17. **Identical Move Pattern** - `createIdenticalMovePattern()`
18. **Redundant Move Pattern 2** - `createRedundantMovePattern2()`
19. **In-Place Comparison Pattern** - `createInPlaceComparisonPattern()`
20. **In-Place Arithmetic Pattern** - `createInPlaceArithmeticPattern()`
21. **LDR to LDP Pattern** - `createLdrToLdpXPattern()`
22. **MOV-SUB-MOV Scratch Pattern** - `createMovSubMovScratchPattern()`
23. **Conservative MOVZ Scratch Pattern** - `createConservativeMovzScratchPattern()`
24. **Load Through Scratch Register** - `createLoadThroughScratchRegisterPattern()` (already had nopeep check)
25. **STR to STP Pattern** - `createStrToStpXPattern()`
26. **ADR+ADD Fusion Pattern** - `createAdrAddAddFusionPattern()`

## Usage Examples

### Setting the nopeep Attribute

```cpp
// Create an instruction that should not be optimized
Instruction ldr_instr = Encoder::create_ldr_imm(dest_reg, base_reg, offset, "Load vector length");
ldr_instr.nopeep = true;  // Mark as protected from optimization
emit(ldr_instr);
```

### Common Use Cases

1. **Bounds Checking Instructions**:
```cpp
Instruction bounds_ldr_instr = Encoder::create_ldr_imm(length_reg, length_addr_reg, 0, "Load vector length for bounds check");
bounds_ldr_instr.nopeep = true; // Protect from peephole optimization
emit(bounds_ldr_instr);
```

2. **Critical Floating-Point Operations**:
```cpp
Instruction ldr_instr = Encoder::create_ldr_fp_imm(dest_reg, effective_addr_reg, 0);
ldr_instr.nopeep = true; // Protect from peephole optimization
emit(ldr_instr);
```

## Implementation Pattern

Each pattern follows this consistent structure:

```cpp
std::unique_ptr<InstructionPattern> createExamplePattern() {
    return std::make_unique<InstructionPattern>(
        pattern_size,
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            // For single instruction patterns:
            if (instrs[pos].nopeep) {
                return {false, 0};
            }
            
            // For multi-instruction patterns:
            if (instrs[pos].nopeep || instrs[pos + 1].nopeep /* ... */) {
                return {false, 0};
            }
            
            // ... rest of matching logic ...
        },
        // ... transformer function ...
        "Pattern description"
    );
}
```

## Testing

A test file `test_nopeep_attribute.cpp` was created to verify the basic functionality of the `nopeep` attribute. The test verifies that:

1. Instructions can be created with `nopeep = true`
2. Instructions default to `nopeep = false`
3. The peephole optimizer can process instruction streams containing instructions with the `nopeep` attribute
4. The attribute is properly preserved in instruction streams

## Compilation Verification

All modified files have been verified to compile successfully with the existing build system. The implementation introduces no new dependencies and maintains backward compatibility.

## Future Considerations

1. **Command-line Control**: Consider adding command-line flags to globally enable/disable respect for the `nopeep` attribute
2. **Debugging Support**: Add tracing to show when optimizations are skipped due to the `nopeep` attribute
3. **Statistics**: Track how often optimizations are prevented by the `nopeep` attribute
4. **Documentation**: Add the `nopeep` attribute to the compiler's user documentation

## Conclusion

The `nopeep` attribute implementation provides fine-grained control over peephole optimization while maintaining the performance benefits of optimization for instructions that don't require protection. All existing peephole patterns now respect this attribute, ensuring that critical instructions marked with `nopeep = true` will not be modified by the optimizer.