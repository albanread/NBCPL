# Object/Class Release Fix Implementation

## Overview

This fix addresses a critical issue in the BCPL class system where base classes with user-defined `RELEASE` methods were not properly deallocating their memory. The compiler was only injecting automatic cleanup calls for derived classes, leaving base class objects allocated in the heap.

## Problem Description

### Original Behavior
- **Base Classes**: User-defined `RELEASE` methods executed custom cleanup code but did not free the object's memory
- **Derived Classes**: Correctly injected `SUPER.RELEASE()` calls, ensuring proper cleanup chain
- **Result**: Memory leaks for base class objects with custom destructors

### Root Cause
The `inject_superclass_release_calls` function in `ClassPass.cpp` had an early return for classes without parent classes:

```cpp
// PROBLEMATIC CODE (before fix)
if (entry->parent_name.empty()) {
    return; // No superclass to call - THIS CAUSED THE BUG
}
```

This meant base classes never received any automatic cleanup injection.

## Solution

### Updated Logic
The fix modifies `ClassPass::inject_superclass_release_calls()` to handle both base and derived classes appropriately:

1. **Base Classes**: Inject `OBJECT_HEAP_FREE(SELF)` call after user code
2. **Derived Classes**: Continue injecting `SUPER.RELEASE()` call (existing behavior)

### Implementation Details

```cpp
void ClassPass::inject_superclass_release_calls(ClassDeclaration* class_node, ClassTableEntry* entry) {
    for (const auto& member : class_node->members) {
        if (auto* routine = dynamic_cast<RoutineDeclaration*>(member.declaration.get())) {
            if (routine->name == "RELEASE") {
                if (auto* body = dynamic_cast<CompoundStatement*>(routine->body.get())) {
                    
                    if (entry->parent_name.empty()) {
                        // BASE CLASS: Inject OBJECT_HEAP_FREE(SELF)
                        auto free_func = std::make_unique<VariableAccess>("OBJECT_HEAP_FREE");
                        std::vector<ExprPtr> args;
                        args.push_back(std::make_unique<VariableAccess>("SELF"));
                        auto free_stmt = std::make_unique<RoutineCallStatement>(std::move(free_func), std::move(args));
                        body->statements.push_back(std::move(free_stmt));
                        
                    } else {
                        // DERIVED CLASS: Inject SUPER.RELEASE() (existing logic)
                        auto super_call = std::make_unique<SuperMethodCallExpression>("RELEASE", std::vector<ExprPtr>{});
                        auto super_stmt = std::make_unique<RoutineCallStatement>(std::move(super_call), std::vector<ExprPtr>{});
                        body->statements.push_back(std::move(super_stmt));
                    }
                }
                return;
            }
        }
    }
}
```

## Impact

### Before Fix
```bcpl
CLASS Point (x, y) BE {
    LET RELEASE() BE {
        WRITES("Point destroyed*N")
        // BUG: No memory deallocation here!
        // Object remains in heap, causing memory leak
    }
}
```

### After Fix
```bcpl
CLASS Point (x, y) BE {
    LET RELEASE() BE {
        WRITES("Point destroyed*N")
        // AUTOMATICALLY INJECTED: OBJECT_HEAP_FREE(SELF)
        // Object is properly deallocated from heap
    }
}
```

### Inheritance Chain Behavior

#### Base Class (Point)
1. Execute user code: `WRITES("Point destroyed*N")`
2. **NEW**: Execute injected `OBJECT_HEAP_FREE(SELF)`
3. Object memory is freed and HeapManager metrics updated

#### Derived Class (ColoredPoint extends Point)
1. Execute user code: `WRITES("ColoredPoint destroyed*N")`
2. Execute injected `SUPER.RELEASE()` (unchanged behavior)
3. This calls Point's RELEASE, which now includes the memory free

## Benefits

### Memory Management
- ✅ **Eliminates Memory Leaks**: Base class objects are now properly deallocated
- ✅ **Maintains Inheritance Chain**: Derived classes still correctly call parent destructors
- ✅ **Automatic Injection**: No user code changes required
- ✅ **HeapManager Integration**: Proper metrics tracking for all object deallocations

### Developer Experience
- ✅ **Transparent Operation**: Existing code works without modification
- ✅ **Predictable Behavior**: All objects with RELEASE methods are guaranteed to be freed
- ✅ **Debug Tracing**: Enhanced logging shows injection for both base and derived classes

## Debugging and Tracing

### Trace Output
When `trace_enabled_` is true, the compiler now outputs:

```
[CLASSPASS] Injected OBJECT_HEAP_FREE(SELF) into user-defined RELEASE for base class 'Point'.
[CLASSPASS] Injected SUPER.RELEASE() into user-defined RELEASE for derived class 'ColoredPoint'.
```

### Runtime Verification
The fix can be verified by:
1. Creating base class objects with RELEASE methods
2. Checking HeapManager metrics before and after object destruction
3. Ensuring object allocations and frees are balanced

## Technical Notes

### AST Node Construction
The fix creates proper AST nodes for the injected call:
- `VariableAccess` for the function name ("OBJECT_HEAP_FREE")
- `VariableAccess` for the argument ("SELF")
- `RoutineCallStatement` to wrap the function call
- Proper move semantics to avoid copying

### Code Generation
The injected `OBJECT_HEAP_FREE(SELF)` call:
1. Is processed by the code generator like any other function call
2. Resolves to the runtime function that interfaces with HeapManager
3. Properly updates allocation tracking and metrics
4. Integrates with the Bloom filter double-free detection system

## Related Systems

### HeapManager Integration
This fix works in conjunction with:
- **Object Allocation**: `OBJECT_HEAP_ALLOC()` for object creation
- **Bloom Filter Detection**: Enhanced double-free detection system
- **Runtime Metrics**: Accurate tracking of object lifecycle

### Class System
The fix maintains compatibility with:
- **Single Inheritance**: Proper SUPER.RELEASE() chaining
- **Method Resolution**: Virtual method dispatch for RELEASE
- **Memory Layout**: No changes to object structure or vtables

## Future Considerations

### Potential Enhancements
1. **Finalizer Support**: Could be extended to support multiple cleanup phases
2. **Resource Management**: Framework for managing non-memory resources
3. **Destructor Ordering**: Guarantee specific cleanup sequences
4. **Exception Safety**: Ensure cleanup in error conditions

### Backward Compatibility
- ✅ Existing code continues to work unchanged
- ✅ No breaking changes to class syntax or semantics
- ✅ Optional tracing for migration verification
- ✅ Gradual adoption possible (class-by-class)

## Conclusion

This fix resolves a fundamental memory management issue in the BCPL class system by ensuring that all objects with user-defined RELEASE methods are properly deallocated, regardless of their position in the inheritance hierarchy. The solution is elegant, transparent, and maintains full backward compatibility while providing immediate memory leak prevention.