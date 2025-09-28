# Fix: AST Corruption and Loop Context Management

**Date**: September 23, 2024  
**Status**: ✅ Fixed  
**Priority**: Critical  
**Components**: ASTAnalyzer, LICM Pass, ForEachStatement, LetDeclaration  

## Problem Summary

The compiler suffered from critical AST corruption issues that manifested as:

1. **Variable names being erased** during optimization passes
2. **ForEachStatement failing to manage loop context** causing state leakage from FOR loops
3. **Fragile state management** in LetDeclaration causing potential corruption

## Root Cause Analysis

### Primary Issue: LICM Pass AST Corruption

**Location**: `LoopInvariantCodeMotionPass.cpp` + `main.cpp`

The Loop Invariant Code Motion (LICM) pass was running **before** function metrics were established by signature analysis. This caused:

```cpp
// TemporaryVariableFactory failing to create variables
std::string temp_var = temp_var_factory_.create(...);
// temp_var returns empty string due to missing function metrics
// LICM creates VariableAccess with empty name -> AST corruption
rhs = std::make_unique<VariableAccess>(temp_var); // temp_var is ""
```

**Evidence**:
```
TemporaryVariableFactory Error: Function metrics not found for: START
[ERROR] VariableAccess node has EMPTY NAME! This indicates corruption.
```

### Secondary Issue: ForEachStatement Loop Context

**Location**: `analysis/az_impl/ASTAnalyzer.cpp`

ForEachStatement visitor was missing loop context stack management that all other loop visitors had:

```cpp
// Missing in ForEachStatement:
loop_context_stack_.push(LoopContext::FOREACH_LOOP);
// ... visitor logic ...
loop_context_stack_.pop();
```

This caused FOR loop variable renaming logic to incorrectly apply to FOREACH loops.

### Tertiary Issue: LetDeclaration State Management

**Location**: `analysis/az_impl/az_visit_LetDeclaration.cpp`

Manual scope changes without RAII could lead to state corruption if exceptions occurred:

```cpp
// Fragile pattern:
std::string previous_scope = current_function_scope_;
current_function_scope_ = name;
initializer->accept(*this); // Could throw exception
current_function_scope_ = previous_scope; // Might not execute
```

## Solution Implementation

### 1. Fixed LICM Pass Ordering and Error Handling

**File**: `main.cpp`
```cpp
// OLD: LICM ran before signature analysis
if (enable_opt) {
    LoopInvariantCodeMotionPass licm_pass(...);
    ast = licm_pass.apply(std::move(ast)); // Too early!
}
// Signature analysis happened after

// NEW: LICM runs after signature analysis
SignatureAnalysisVisitor signature_visitor(...);
signature_visitor.analyze_signatures(*ast);

if (enable_opt) {
    LoopInvariantCodeMotionPass licm_pass(...);
    ast = licm_pass.apply(std::move(ast)); // Now has function metrics
}
```

**File**: `LoopInvariantCodeMotionPass.cpp`
```cpp
std::string temp_var = temp_var_factory_.create(...);

// NEW: Error handling to prevent corruption
if (temp_var.empty()) {
    std::cerr << "[LICM] Error: Failed to create temporary variable for function " 
              << current_function_name_ << ". Skipping hoisting." << std::endl;
    continue; // Skip optimization to avoid AST corruption
}
```

### 2. Fixed ForEachStatement Loop Context Management

**File**: `analysis/ASTAnalyzer.h`
```cpp
// Added FOREACH_LOOP to enum
enum class LoopContext { NONE, FOR_LOOP, WHILE_LOOP, UNTIL_LOOP, REPEAT_LOOP, FOREACH_LOOP };
```

**File**: `analysis/az_impl/ASTAnalyzer.cpp`
```cpp
void ASTAnalyzer::visit(ForEachStatement& node) {
    // NEW: Push FOREACH loop context to prevent FOR loop state interference
    loop_context_stack_.push(LoopContext::FOREACH_LOOP);
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Pushed FOREACH loop context. Context stack size: " << loop_context_stack_.size() << std::endl;

    // ... existing visitor logic ...

    // NEW: Pop FOREACH loop context
    loop_context_stack_.pop();
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Popped FOREACH loop context. Context stack size: " << loop_context_stack_.size() << std::endl;
}
```

Also fixed duplicate `body->accept(*this)` call that was present.

### 3. Improved LetDeclaration State Management with RAII

**File**: `analysis/az_impl/az_visit_LetDeclaration.cpp`
```cpp
// NEW: RAII helper class for managing function scope changes
class ScopeGuard {
private:
    std::string& scope_ref_;
    std::string previous_scope_;
    
public:
    ScopeGuard(std::string& scope_ref, const std::string& new_scope) 
        : scope_ref_(scope_ref), previous_scope_(scope_ref) {
        scope_ref_ = new_scope;
    }
    
    ~ScopeGuard() {
        scope_ref_ = previous_scope_;
    }
    
    // Delete copy constructor and assignment operator to prevent misuse
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
};

// Usage in function-like LET declarations:
if (is_function_like) {
    // Use RAII to ensure scope is always restored, even if an exception occurs
    ScopeGuard scope_guard(current_function_scope_, name);
    if (initializer) {
        initializer->accept(*this);
    }
    continue;
}
```

## Testing and Verification

### Before Fix
```
--- AST After Short-Circuiting Pass ---
AssignmentStatement:
  LHS:
    VariableAccess:           // <-- CORRUPTED EMPTY NAME
  RHS:
    VariableAccess:           // <-- CORRUPTED EMPTY NAME

[ERROR] VariableAccess node has EMPTY NAME! This indicates corruption.
TemporaryVariableFactory Error: Function metrics not found for: START
```

### After Fix
```
Pass 1: Analyzing function signatures...
[LICM] Hoisting invariant expression in function START
[DEBUG] Processing assignment to variable: _opt_temp_0
[DEBUG] Symbol lookup for '_opt_temp_0': FOUND
[ANALYZER TRACE] Visiting VariableAccess: _opt_temp_0
[ANALYZER TRACE] Pushed WHILE loop context. Context stack size: 1
```

## Impact

- **✅ Eliminated AST corruption** - No more empty variable names
- **✅ Fixed optimization pass ordering** - LICM now runs with proper function context
- **✅ Improved loop context management** - ForEachStatement properly isolated from FOR loop logic
- **✅ Enhanced exception safety** - RAII pattern prevents state corruption
- **✅ Maintained backward compatibility** - All existing functionality preserved

## Files Modified

1. `main.cpp` - Fixed LICM pass ordering and analyzer declaration scope
2. `analysis/ASTAnalyzer.h` - Added FOREACH_LOOP to LoopContext enum
3. `analysis/az_impl/ASTAnalyzer.cpp` - Fixed ForEachStatement context management
4. `analysis/az_impl/az_visit_LetDeclaration.cpp` - Added RAII ScopeGuard
5. `LoopInvariantCodeMotionPass.cpp` - Added error handling for temp variable creation

## Future Considerations

- Monitor LICM pass performance with new ordering
- Consider extending RAII pattern to other state management areas
- Add automated tests for nested loop scenarios
- Document loop context stack usage for future developers

## Related Issues

This fix resolves the core AST corruption that was preventing:
- Proper variable name resolution
- Correct loop variable scoping
- Reliable optimization passes
- Stable compiler execution