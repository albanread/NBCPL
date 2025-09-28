# Memory Cleanup Design Documentation

## Overview

This document clarifies the design decisions regarding automatic memory cleanup responsibility in the NewBCPL compiler.

## Design Decision: CFGBuilderPass is Sole Authority

**RESOLVED**: CFGBuilderPass is the single, authoritative source for all automatic memory cleanup of local variables.

## Rationale

### Why CFGBuilderPass?

CFGBuilderPass is the optimal location for cleanup logic because:

1. **Complete Control Flow Visibility**: CFGBuilderPass has comprehensive knowledge of ALL possible exit paths from a scope, including:
   - Normal block exit
   - Early returns (`RETURN`, `RESULTIS`)
   - Loop breaks (`BREAK`, `ENDCASE`)
   - Exception paths
   - Jump statements (`GOTO`)

2. **Proper CFG Integration**: Cleanup code is inserted as proper BasicBlocks in the control flow graph, ensuring:
   - Cleanup occurs on every possible execution path
   - Integration with optimization passes
   - Correct liveness analysis

3. **LIFO Ordering**: Variables are cleaned up in reverse order of declaration (Last In, First Out), preventing use-after-free errors.

## Implementation Details

### Location
- **File**: `CFGBuilderPass.cpp`
- **Method**: `visit(BlockStatement& node)`
- **Cleanup Types**:
  - **Block Cleanup**: `BlockCleanup_` blocks for normal scope exit
  - **Defer Cleanup**: `DeferCleanup_` blocks for DEFER statements

### Memory Type Handling
- **Lists**: `BCPL_FREE_LIST(variable)` for list types
- **Vectors/Strings**: `FREEVEC(variable)` for VEC and STRING types  
- **Objects**: `variable.RELEASE()` for object types with destructors

### Variable Tracking
- Uses `block_variable_stack` to track which variables were declared in each block
- Only cleans up variables that were actually declared in the current scope
- Respects `owns_heap_memory` flag to avoid cleaning stack-allocated variables

## Disabled Code

### ASTAnalyzer Cleanup (PERMANENTLY DISABLED)

**File**: `analysis/az_impl/az_visit_BlockStatement.cpp`

The ASTAnalyzer previously contained cleanup injection logic that has been **permanently disabled** to prevent double-free crashes. This code is commented out and should never be re-enabled.

**Reason for Disabling**: 
- ASTAnalyzer operates on the raw AST without control flow knowledge
- Cannot guarantee cleanup on all exit paths
- Would create double-free scenarios when combined with CFGBuilderPass cleanup

## Architecture Benefits

1. **Memory Safety**: No memory leaks from automatic variables
2. **No Double-Free**: Single cleanup authority prevents double-free crashes
3. **Exception Safety**: Cleanup occurs even on exceptional control flow
4. **Optimization Friendly**: CFG-integrated cleanup works with all optimization passes
5. **Maintainable**: Single source of truth for cleanup logic

## Future Considerations

- This design is **final and should not be changed**
- Any new cleanup requirements should be added to CFGBuilderPass
- ASTAnalyzer cleanup code should remain permanently disabled
- New memory types should be added to the CFGBuilderPass cleanup logic

## Related Files

- `CFGBuilderPass.h` - Header documentation
- `CFGBuilderPass.cpp` - Implementation (visit(BlockStatement))
- `analysis/az_impl/az_visit_BlockStatement.cpp` - Disabled cleanup code
- `NewCodeGenerator.cpp` - Final code generation for cleanup calls

## Verification

To verify this design is working:
1. Check that programs with heap-allocated variables don't leak memory
2. Ensure no double-free crashes occur
3. Verify cleanup occurs on all control flow paths (normal exit, early returns, breaks)
4. Confirm LIFO cleanup ordering

This design resolves the previous ambiguity and provides a solid foundation for memory-safe BCPL programming.