# Investigation Report: "Register string cannot be empty" Bug

**Case ID**: BCPL-2024-001  
**Detective**: Chief Investigator AI  
**Date**: December 2024  
**Status**: ACTIVE - Architectural fix in progress  
**Severity**: Critical - Prevents compilation of recursive functions

## Executive Summary

Through systematic debugging and architectural analysis, we have identified the root cause of the persistent "Register string cannot be empty" error that occurs when compiling recursive functions in the NewBCPL compiler. The issue stems from a violation of phase separation where encoder functions are called during type analysis phase with empty register strings. A major architectural fix has been implemented but the issue persists.

## The Crime Scene

### Initial Symptoms
- **Error Message**: `NewBCPL Compiler Error: Register string cannot be empty.`
- **Trigger**: Compiling recursive functions (specifically `FACTORIAL_RECURSIVE`)
- **Timing**: Occurs after analysis phase completes, during code generation
- **Pattern**: Deterministic - always fails at the same point

### Test Case
```bcpl
LET FACTORIAL_RECURSIVE(N) = VALOF
$(
  IF N = 0 THEN RESULTIS 1
  RESULTIS N * FACTORIAL_RECURSIVE(N - 1)
$)

LET START() BE
$(
  LET N = 18
  LET R = FACTORIAL_RECURSIVE(N)
  WRITES("Result: ") 
  WRITEN(R)
  FINISH
$)
```

## Investigation Timeline

### Phase 4: Major Architectural Fix (🔧 Partial Success)
**Implementation**: Moved register allocation to occur upfront before code generation  
**Changes Made**:
- Modified `main.cpp` to perform all register allocation before code generation begins
- Created master `all_allocations` map passed to `NewCodeGenerator`
- Removed circular dependency between analysis and code generation phases
- Fixed `get_variable_register` to use strict error checking instead of fallback allocation

**Result**: `tests/test_functions.bcl` now compiles and runs successfully, but `tests/test_facts.bcl` still fails

**Current Status**: Error now occurs during type analysis phase, not code generation, indicating some encoder function is still being called during analysis

### Phase 5: Class System Regression Fix (🎯 Complete Success)
**Problem**: Architectural changes broke class member variable access  
**Root Cause**: `SymbolDiscoveryPass` wasn't processing class methods and their parameters  
**Fix Applied**:
- Enhanced `SymbolDiscoveryPass::visit(ClassDeclaration&)` to process method parameters
- Added class member variable support to `get_variable_register()` and `store_variable_register()`
- Restored proper separation between SymbolTable (parameters/locals) and ClassTable (member variables)

**Result**: `tests/class1.bcl` now compiles and runs successfully with full class functionality

## Investigation Timeline (Original)

### Phase 1: Initial Hypothesis (❌ Red Herring)
**Theory**: Symbol table scoping issue with parameter name collision  
**Evidence**: Variable `N` exists in both `START()` and `FACTORIAL_RECURSIVE()`  
**Investigation**: Deep dive into `ASTAnalyzer::visit(LetDeclaration&)`  
**Outcome**: Fixed symbol lookup issues but error persisted  

**Key Discovery**: The error was NOT caused by name collisions - even when parameter names were changed (`N` → `K`), the same error occurred.

### Phase 2: Architectural Analysis (✅ Breakthrough)
**Theory**: Circular dependency between analysis and code generation  
**Evidence**: Error caught in `infer_expression_type()` but should not involve encoders  
**Investigation**: Traced execution flow through debug statements  

**Key Finding**: Error occurs during **code generation phase**, not analysis phase, despite misleading error handling code.

### Phase 3: Pinpoint Investigation (🎯 Bullseye)
**Method**: Systematic debug output placement  
**Tools**: Strategic `std::cerr` statements at critical execution points  
**Breakthrough**: Identified exact failure location  

## The Smoking Gun

### Error Location (Updated)
**Original Location** (Fixed):
```
File: analysis/LinearScanAllocator.cpp
Function: LinearScanAllocator::allocate()
Context: Processing live interval for variable 'N' in FACTORIAL_RECURSIVE
Trigger: 4th recursive call to LinearScanAllocator during code generation
```

**Current Location**:
```
Phase: Type analysis (infer_expression_type)
Context: Processing function call to FACTORIAL_ITERATIVE in test_facts.bcl
Error: Encoder function called with empty register string during analysis phase
```

### Execution Flow Leading to Error
1. **Code Generation Starts**: `NewCodeGenerator::generate_code(Program&)`
2. **Function Processing**: `visit(FunctionDeclaration&)` for `FACTORIAL_RECURSIVE`
3. **Register Allocation**: `LinearScanAllocator::allocate()` called from within code generation
4. **Interval Processing**: Processing live interval for parameter `N`
5. **💥 BOOM**: Encoder function called with empty string somewhere in the allocation process

### Critical Evidence
- **Multiple Calls**: LinearScanAllocator called 4 times during single compilation
- **Successful Calls**: First 3 calls complete successfully
- **Fatal Call**: 4th call fails when processing parameter `N`
- **Timing**: Error occurs immediately after `does_interval_cross_call()` returns `true`

## Root Cause Analysis

### The Architectural Flaw
**Original Problem** (Fixed): Register allocation occurs **during** code generation rather than **before** it.

**Correct Flow** (Now Implemented):
```
Analysis → Register Allocation → Code Generation
```

**Original Broken Flow** (Fixed):
```
Analysis → Code Generation → Register Allocation → Code Generation → ...
```

**Current Remaining Issue**: Some encoder function is still being called during the type analysis phase with empty register strings.

### The Circular Dependency
1. Code generator needs register allocation to generate code
2. Register allocator somehow triggers encoder functions during allocation
3. Encoder functions expect valid register strings but receive empty strings
4. Exception thrown: "Register string cannot be empty"

### Why It's Intermittent
- Error occurs only on the **4th recursive call** to LinearScanAllocator
- First 3 calls succeed, suggesting state corruption or resource exhaustion
- Specific to functions with certain call patterns (recursive functions)

## Technical Details

### Call Stack Summary
```
main()
├── NewCodeGenerator::generate_code()
    ├── visit(Program&)
        ├── process_declaration(FunctionDeclaration&)
            ├── generate_function_like_code()
                ├── LinearScanAllocator::allocate()  // ← ERROR HERE
                    ├── does_interval_cross_call()   // ← Succeeds
                    ├── [Unknown encoder call]       // ← Fails with empty string
```

### Symbol Table Fixes Applied
During investigation, we discovered and fixed secondary issues:
- **Fixed**: Multiple `symbol_table_->lookup(var_name, "Global", symbol)` calls
- **Replaced with**: Proper `lookup_symbol(var_name, symbol)` calls
- **Impact**: Eliminated incorrect symbol lookup warnings
- **Result**: Cleaner error output, but main issue persisted

## Evidence Trail

### Debug Output Sequence (Last Successful Execution)
```
*** ENTERING LinearScanAllocator::allocate for function: FACTORIAL_RECURSIVE ***
*** GOT call sites successfully, size: 1 ***
*** FINISHED processing integer registers ***
*** FINISHED processing float registers ***
*** ABOUT TO PARTITION intervals, size: 1 ***
*** PROCESSING interval for variable: N ***
*** ENTERING does_interval_cross_call for variable: N ***
*** Interval range: 0 to 2 ***
*** Call sites count: 1 ***
*** Checking call site: 0 ***
*** Interval crosses call - returning true ***
[ERROR OCCURS HERE]
NewBCPL Compiler Error: Register string cannot be empty.
```

### Register State at Failure
- **Integer Registers**: All valid (`X20`, `X21`, `X22`, `X23`, `X24`, `X25`, `X26`, `X27`)
- **Float Registers**: All valid (`D8` through `D15`)
- **No Empty Strings**: In register lists themselves

## Recommended Solution

### Immediate Fix
**Prevent LinearScanAllocator from being called during code generation**

### Architectural Restructure
1. **Phase Separation**: Ensure clear boundaries between analysis, allocation, and generation
2. **Pre-allocation**: Complete all register allocation before any code generation begins
3. **State Management**: Prevent recursive calls to allocation during generation

### Implementation Strategy
1. Move all register allocation to a separate compilation phase
2. Store allocation results in a data structure accessible to code generator
3. Eliminate any code paths that trigger allocation during generation
4. Add validation to prevent circular dependencies

## Case Status: PARTIALLY SOLVED - INVESTIGATION PAUSED

✅ **Major Fix Implemented**: Circular dependency between code generation and register allocation eliminated  
✅ **Phase Separation**: Register allocation now occurs upfront before code generation  
✅ **Class System**: Fixed regression in class member variable access  
✅ **Partial Success**: `tests/test_functions.bcl` now works, `tests/class1.bcl` fully restored  
❌ **Remaining Issue**: `tests/test_facts.bcl` still fails - encoder called during type analysis with empty string  
⏸️ **Status**: Investigation paused - architectural foundation is solid, remaining issue is isolated

## Lessons Learned

1. **Follow the Data**: Debug output placement was crucial for pinpointing the exact failure location
2. **Question Assumptions**: Initial hypothesis about symbol scoping was completely wrong
3. **Architectural Discipline**: Circular dependencies create subtle, hard-to-debug issues
4. **Systematic Approach**: Methodical tracing beats clever theories every time

## Next Actions

1. ✅ **Major Fix Completed**: Restructured compilation phases to prevent circular dependency
2. ✅ **Phase Separation**: Implemented clean separation between analysis, allocation, and generation
3. ✅ **Regression Fix**: Restored class system functionality with cleaner architecture
4. 🔍 **Remaining Task**: Identify which encoder function is called during type analysis phase
5. 📋 **Future Work**: Add stack traces to encoder functions to pinpoint exact call site

---

**Case Updated by**: Chief Detective AI  
**File Location**: `NewBCPL/investigations/register-string-empty-bug-investigation.md`  
**Related Issues**: Circular dependency elimination (✅ FIXED), phase separation architecture (✅ IMPLEMENTED), class system regression (✅ FIXED)  

*"Major architectural victory achieved - the foundation is now solid. The remaining issue is isolated and the compiler is in much better shape overall. Sometimes fixing the architecture is more valuable than fixing the specific bug."*