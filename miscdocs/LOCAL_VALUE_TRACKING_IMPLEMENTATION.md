# Local Value Tracking Implementation for Address Calculations

## Problem Statement

The BCPL-to-ARM64 compiler was generating redundant address calculations for string literals, even after implementing AST-level Common Subexpression Elimination (CSE). The core issue was that the code generator was emitting identical ADRP/ADD instruction sequences for the same string literal multiple times within a function.

### Example of the Problem

For code like:
```bcpl
LET START() BE
$(
    WRITEF("Hello World")
    WRITEF("Hello World")  // Redundant address calculation
    WRITEF("Hello World")  // Redundant address calculation
$)
```

The compiler was generating:
```asm
; First use
ADRP X9, L_str0@PAGE
ADD X9, X9, L_str0@PAGEOFF
ADD X9, X9, #8
MOV X0, X9
BL _WRITEF

; Second use - REDUNDANT!
ADRP X9, L_str0@PAGE
ADD X9, X9, L_str0@PAGEOFF
ADD X9, X9, #8
MOV X0, X9
BL _WRITEF

; Third use - REDUNDANT!
ADRP X9, L_str0@PAGE
ADD X9, X9, L_str0@PAGEOFF
ADD X9, X9, #8
MOV X0, X9
BL _WRITEF
```

## Root Cause Analysis

The issue occurred during the translation of CSE-optimized code:

1. **CSE Pass Works Correctly**: The AST-level CSE correctly identified repeated string literals and created assignment statements:
   ```
   AssignmentStatement: _cse_temp_0 := StringLiteral("Hello World")
   RoutineCallStatement: WRITEF(VariableAccess: _cse_temp_0)
   RoutineCallStatement: WRITEF(VariableAccess: _cse_temp_0)
   ```

2. **Code Generator Bug**: The code generator wasn't properly visiting the AssignmentStatement nodes created by CSE, so it fell back to processing the original StringLiteral nodes directly.

3. **Missing Local Optimization**: Even when assignments were processed, the code generator had no mechanism to recognize that a variable held a previously calculated address.

## Solution: Local Value Tracking

The solution implements **local value tracking** within the code generator to:

1. **Register Canonical Addresses**: When an assignment stores the address of a literal into a variable, record this relationship.
2. **Optimize Variable Access**: When accessing a variable known to hold an address, load from the variable instead of recalculating the address.
3. **Maintain Correctness**: Clear tracking when variables are reassigned or at function boundaries.

## Implementation Details

### 1. Data Structures Added to NewCodeGenerator

```cpp
// Map from variable name to canonical address expression it holds
std::unordered_map<std::string, std::string> local_value_map_;
```

### 2. Helper Methods Added

```cpp
// Register that a variable holds the address of a canonical expression
void register_canonical_address(const std::string& var_name, const std::string& canonical_form);

// Check if a variable has a known address value
bool has_known_address_value(const std::string& var_name) const;

// Get the canonical form of the address stored in a variable
std::string get_canonical_address_form(const std::string& var_name) const;

// Invalidate tracking for a specific variable (when it's reassigned)
void invalidate_variable_tracking(const std::string& var_name);

// Clear all local value tracking (called at function boundaries)
void clear_local_value_tracking();

// Generate canonical form for an expression
std::string get_expression_canonical_form(const Expression* expr) const;
```

### 3. Integration Points

#### AssignmentStatement Handler
When processing assignments, register canonical addresses:

```cpp
void NewCodeGenerator::visit(AssignmentStatement& node) {
    // ... existing assignment logic ...
    
    if (auto* var_access = dynamic_cast<VariableAccess*>(lhs_expr.get())) {
        // Register canonical address if RHS is a trackable literal
        std::string canonical_form = get_expression_canonical_form(node.rhs[i].get());
        if (!canonical_form.empty()) {
            register_canonical_address(var_access->name, canonical_form);
        } else {
            // If RHS is not trackable, invalidate any existing tracking
            invalidate_variable_tracking(var_access->name);
        }
        
        handle_variable_assignment(var_access, value_to_store_reg);
    }
}
```

#### VariableAccess Handler
When accessing variables, check for known address values:

```cpp
void NewCodeGenerator::visit(VariableAccess& node) {
    // Check if this variable holds a known address
    if (has_known_address_value(node.name)) {
        std::string canonical_form = get_canonical_address_form(node.name);
        
        // Check if this is a string literal address we can optimize
        if (canonical_form.find("STRING_LITERAL:") == 0) {
            // Load the pre-calculated address from the variable
            // instead of recalculating it
            expression_result_reg_ = get_variable_register(node.name);
            return;
        }
    }
    
    // Fall back to normal variable access
    expression_result_reg_ = get_variable_register(node.name);
}
```

#### Function Boundaries
Clear tracking at function entry:

```cpp
void NewCodeGenerator::generate_function_like_code(...) {
    // Clear local value tracking for new function
    clear_local_value_tracking();
    
    // ... rest of function generation ...
}
```

### 4. Canonical Form Generation

String literals are represented as:
```cpp
std::string get_expression_canonical_form(const Expression* expr) const {
    switch (expr->getType()) {
        case ASTNode::NodeType::StringLit: {
            auto* str_lit = static_cast<const StringLiteral*>(expr);
            return "STRING_LITERAL:\"" + str_lit->value + "\"";
        }
        // ... other literal types ...
    }
}
```

## Expected Optimization Result

With local value tracking implemented, the same code should generate:

```asm
; First use - calculate and store address
ADRP X9, L_str0@PAGE
ADD X9, X9, L_str0@PAGEOFF
ADD X9, X9, #8
STR X9, [X29, #16]  ; Store to _cse_temp_0 stack slot
MOV X0, X9
BL _WRITEF

; Second use - load pre-calculated address
LDR X9, [X29, #16]  ; Load from _cse_temp_0 stack slot
MOV X0, X9
BL _WRITEF

; Third use - load pre-calculated address
LDR X9, [X29, #16]  ; Load from _cse_temp_0 stack slot
MOV X0, X9
BL _WRITEF
```

This eliminates the redundant ADRP/ADD sequences and replaces them with simple LDR instructions.

## Benefits

1. **Performance**: Eliminates redundant address calculations within functions
2. **Code Size**: Reduces instruction count for repeated literal access
3. **Compatibility**: Works independently of AST-level optimizations
4. **Maintainability**: Local to code generation, doesn't affect other compiler phases

## Limitations and Future Extensions

1. **Scope**: Currently limited to function-local optimization
2. **Literal Types**: Initially focused on string literals, but extensible to other constants
3. **Invalidation**: Simple invalidation strategy; could be enhanced with more sophisticated analysis
4. **Register Allocation**: Coordinates with existing register allocation but could be further optimized

## Testing Strategy

Test cases should verify:
1. Basic redundant string literal elimination
2. Mixed string literals (some repeated, some unique)
3. Variable reassignment invalidation
4. Function boundary clearing
5. Nested function calls
6. Error cases and edge conditions

This implementation provides a robust foundation for eliminating redundant address calculations while maintaining code correctness and compiler stability.