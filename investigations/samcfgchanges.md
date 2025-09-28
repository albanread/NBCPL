# SAMM CFG Builder Changes Documentation

This document captures the changes made to `CFGBuilderPass.cpp` to support SAMM (Scope Aware Memory Management) by injecting `HeapManager_enter_scope()` and `HeapManager_exit_scope()` calls at appropriate control flow points.

## Overview

The CFG Builder was modified to automatically inject SAMM scope management calls into the control flow graph. This ensures that every scope (block) properly enters and exits SAMM tracking, allowing automatic cleanup of heap allocations when scopes end.

## Key Principle

- **Scope Entry**: Inject `HeapManager_enter_scope()` at the beginning of `BlockStatement`
- **Scope Exit**: Inject `HeapManager_exit_scope()` before any control flow statement that exits a scope

## Detailed Changes

### 1. BlockStatement Entry Point (Lines 941-997)

```cpp
void CFGBuilderPass::visit(BlockStatement& node) {
    if (trace_enabled_) {
        std::cout << "[CFGBuilderPass] Visiting BlockStatement with SAMM scope management." << std::endl;
    }

    // --- SAMM: Inject scope entry call ---
    if (!current_basic_block) end_current_block_and_start_new();
    
    debug_print("SAMM: Injecting HeapManager_enter_scope() call at block start");
    auto enter_scope_expr = std::make_unique<VariableAccess>("HeapManager_enter_scope");
    auto enter_scope_call = std::make_unique<RoutineCallStatement>(std::move(enter_scope_expr), std::vector<ExprPtr>{});
    current_basic_block->add_statement(std::move(enter_scope_call));

    // --- Synchronize SymbolTable scope ---
    if (symbol_table_) {
        symbol_table_->enterScope();
    }

    // ... process statements in block ...

    // --- SAMM: Inject scope exit call for normal block exit ---
    if (current_basic_block && !current_basic_block->ends_with_control_flow()) {
        debug_print("SAMM: Injecting HeapManager_exit_scope() call for normal block exit");
        auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
        auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
        current_basic_block->add_statement(std::move(exit_scope_call));
    }

    // --- Synchronize SymbolTable scope ---
    if (symbol_table_) {
        symbol_table_->exitScope();
    }
}
```

**Purpose**: Every `$( ... $)` block gets automatic scope tracking.

### 2. Return Statement (Lines 887-897)

```cpp
void CFGBuilderPass::visit(ReturnStatement& node) {
    if (!current_basic_block) end_current_block_and_start_new();
    
    debug_print("SAMM: Visiting ReturnStatement - SAMM disabled, no scope exit call injected.");
    
    // SAMM: DISABLED - Inject scope exit call before return
    // auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
    // auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
    // current_basic_block->add_statement(std::move(exit_scope_call));
    
    // Add the original RETURN statement
    current_basic_block->add_statement(std::make_unique<ReturnStatement>(node));
    end_current_block_and_start_new();
}
```

**Note**: Currently disabled, but shows where scope exit would be injected before function returns.

### 3. Resultis Statement (Lines 909-919)

```cpp
void CFGBuilderPass::visit(ResultisStatement& node) {
    if (!current_basic_block) end_current_block_and_start_new();
    
    debug_print("SAMM: Visiting ResultisStatement - SAMM disabled, no scope exit call injected.");
    
    // SAMM: DISABLED - Inject scope exit call before resultis
    // auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
    // auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
    // current_basic_block->add_statement(std::move(exit_scope_call));
    
    // Add the original RESULTIS statement
    current_basic_block->add_statement(std::make_unique<ResultisStatement>(node));
    end_current_block_and_start_new();
}
```

**Note**: Currently disabled for RESULTIS (function value returns).

### 4. Control Flow Exit Statements

The following statements all inject `HeapManager_exit_scope()` calls before executing:

#### FinishStatement (Lines 1215-1225)
```cpp
void CFGBuilderPass::visit(FinishStatement& node) { 
    if (!current_basic_block) end_current_block_and_start_new(); 
    
    debug_print("SAMM: Visiting FinishStatement - injecting scope exit call.");
    
    // SAMM: Inject scope exit call before finish
    auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
    auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
    current_basic_block->add_statement(std::move(exit_scope_call));
    
    // Add the original FINISH statement
    current_basic_block->add_statement(std::make_unique<FinishStatement>(node));
    end_current_block_and_start_new();
}
```

#### BreakStatement (Lines 1231-1241)
```cpp
void CFGBuilderPass::visit(BreakStatement& node) { 
    if (!current_basic_block) end_current_block_and_start_new(); 
    
    debug_print("SAMM: Visiting BreakStatement - injecting scope exit call.");
    
    // SAMM: Inject scope exit call before break
    auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
    auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
    current_basic_block->add_statement(std::move(exit_scope_call));
    
    // Add the original BREAK statement
    current_basic_block->add_statement(std::make_unique<BreakStatement>(node));
    end_current_block_and_start_new();
}
```

#### LoopStatement (Lines 1251-1261)
```cpp
void CFGBuilderPass::visit(LoopStatement& node) { 
    if (!current_basic_block) end_current_block_and_start_new(); 
    
    debug_print("SAMM: Visiting LoopStatement - injecting scope exit call.");
    
    // SAMM: Inject scope exit call before loop
    auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
    auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
    current_basic_block->add_statement(std::move(exit_scope_call));
    
    // Add the original LOOP statement
    current_basic_block->add_statement(std::make_unique<LoopStatement>(node));
    end_current_block_and_start_new();
}
```

#### EndcaseStatement (Lines 1271-1281)
```cpp
void CFGBuilderPass::visit(EndcaseStatement& node) { 
    if (!current_basic_block) end_current_block_and_start_new(); 
    
    debug_print("SAMM: Visiting EndcaseStatement - injecting scope exit call.");
    
    // SAMM: Inject scope exit call before endcase
    auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
    auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
    current_basic_block->add_statement(std::move(exit_scope_call));
    
    // Add the original ENDCASE statement
    current_basic_block->add_statement(std::make_unique<EndcaseStatement>(node));
    end_current_block_and_start_new();
}
```

#### GotoStatement (Lines 1291-1301)
```cpp
void CFGBuilderPass::visit(GotoStatement& node) {
    if (!current_basic_block) end_current_block_and_start_new();
    
    debug_print("SAMM: Visiting GotoStatement - injecting scope exit call.");
    
    // SAMM: Inject scope exit call before goto
    auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
    auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
    current_basic_block->add_statement(std::move(exit_scope_call));
    
    // Add the original GOTO statement
    current_basic_block->add_statement(std::make_unique<GotoStatement>(node));
    end_current_block_and_start_new();
}
```

## Implementation Pattern

The consistent pattern used for all scope exit injections:

1. **Ensure Basic Block**: `if (!current_basic_block) end_current_block_and_start_new();`
2. **Debug Logging**: `debug_print("SAMM: Visiting XxxStatement - injecting scope exit call.");`
3. **Create Exit Call**: 
   ```cpp
   auto exit_scope_expr = std::make_unique<VariableAccess>("HeapManager_exit_scope");
   auto exit_scope_call = std::make_unique<RoutineCallStatement>(std::move(exit_scope_expr), std::vector<ExprPtr>{});
   current_basic_block->add_statement(std::move(exit_scope_call));
   ```
4. **Add Original Statement**: `current_basic_block->add_statement(std::make_unique<XxxStatement>(node));`
5. **End Block**: `end_current_block_and_start_new();`

## Status Notes

- **Active**: `BlockStatement`, `FinishStatement`, `BreakStatement`, `LoopStatement`, `EndcaseStatement`, `GotoStatement`
- **Disabled**: `ReturnStatement`, `ResultisStatement` (commented out with "SAMM disabled" notes)

## Runtime Integration

These injected calls work with the HeapManager's C interface:
- `HeapManager_enter_scope()` - Pushes a new scope onto the SAMM stack
- `HeapManager_exit_scope()` - Pops the current scope and cleans up all allocations made within it

## Future Considerations

1. **Return/Resultis Handling**: Currently disabled - may need different approach for function exits
2. **Exception Safety**: Consider how scope exit calls interact with error handling
3. **Nested Scopes**: Ensure proper nesting behavior for complex control flow
4. **Performance**: Monitor overhead of frequent scope enter/exit calls

## SAMM HeapManager Implementation Notes

### Retained Allocation Methods

The HeapManager implemented "retained" allocation variants that allocate to the parent scope instead of current scope:

```cpp
void* HeapManager::allocObjectRetained(size_t size) {
    void* ptr = internalAlloc(size, ALLOC_OBJECT, nullptr, nullptr);
    if (!ptr) return nullptr;
    
    // SAMM: Add to parent scope if enabled
    if (samm_enabled_.load()) {
        std::lock_guard<std::mutex> lock(scope_mutex_);
        if (scope_allocations_.size() > 1) {
            auto& parent_scope = scope_allocations_[scope_allocations_.size() - 2];
            parent_scope.emplace_back(ptr, ALLOC_OBJECT);
            traceLog("SAMM: Allocated retained object %p (added to parent scope)\n", ptr);
        }
    }
    return ptr;
}
```

**Pattern**: All retained allocations (`allocObjectRetained`, `allocVecRetained`, `allocStringRetained`, `allocListRetained`) follow this pattern:
1. Allocate using `internalAlloc`
2. If SAMM enabled, add to **parent scope** (size() - 2) instead of current scope
3. This prevents cleanup when current scope exits, but allows cleanup when parent scope exits

### Key SAMM Infrastructure

- **Scope Stack**: `scope_allocations_` - vector of allocation vectors, one per scope level
- **Thread Safety**: `scope_mutex_` protects scope operations
- **Allocation Tracking**: Each allocation stored as `{ptr, AllocType}` pair
- **Background Cleanup**: Separate thread processes cleanup queue asynchronously

### SAMM Control Methods

- `setSAMMEnabled(bool)` - Enable/disable SAMM and start/stop background worker
- `startBackgroundWorker()` - Launches cleanup thread
- `stopBackgroundWorker()` - Shuts down cleanup thread
- `enterScope()` - Push new scope onto stack
- `exitScope()` - Pop scope and queue allocations for cleanup

## Current State Before Reset

### What's Working
- CFG Builder scope injection is fully implemented and functional
- SAMM HeapManager methods exist but have compilation issues due to version mismatches
- Basic allocation (`allocObject`, `allocString`, etc.) simplified but needs proper integration

### What's Broken
- HeapManager compilation errors due to git revert inconsistencies
- Member variable type mismatches (pointer vs object for bloom filter)
- Missing method declarations (`internalAlloc`, `setSAMMEnabled`, etc.)
- Test files using old function signatures

### Recommended Reset Point
- Hard reset to commit `cdd5993` (origin/objectcpl) - "pre main changes"
- This should provide clean heap manager baseline
- Re-apply CFG changes from this documentation
- Focus on just disabling SAMM background thread for step 1

### Critical Insight
The problem is NOT in the code generator - it's working correctly. The issue is:
1. SAMM background thread needs to be disabled (step 1: leak memory, don't crash)
2. Only CFG builder and heap manager should change
3. Don't modify function signatures or the core allocation API

---
*Documented on: September 2024  
Status: Ready for clean reset and proper SAMM thread disable  
Next Steps: Hard reset, disable SAMM thread only, test object allocation*