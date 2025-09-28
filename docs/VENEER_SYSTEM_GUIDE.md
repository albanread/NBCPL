# BCPL JIT Compiler Veneer System - Technical Implementation Guide

## Table of Contents
1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Implementation Details](#implementation-details)
4. [Adding New External Functions](#adding-new-external-functions)
5. [Debugging Guide](#debugging-guide)
6. [Performance Considerations](#performance-considerations)
7. [Troubleshooting](#troubleshooting)

## Overview

The veneer system in the BCPL JIT compiler provides a standardized mechanism for calling external runtime functions. Instead of generating custom trampolines, the system uses the existing code generation pipeline to create ARM64-compliant veneers that follow industry best practices.

### Key Principles
- **Standardization**: Uses existing Encoder, InstructionStream, and LabelManager
- **ARM64 ABI Compliance**: Follows official ARM64 calling conventions
- **Maintainability**: Clean, debuggable code with proper abstractions
- **Performance**: Efficient integration with the JIT pipeline

## Architecture

### Component Diagram
```
┌─────────────────────┐    ┌─────────────────────┐
│ ExternalFunction    │    │ VeneerManager       │
│ Scanner             │───▶│                     │
└─────────────────────┘    └─────────────────────┘
           │                         │
           │                         ▼
           │                ┌─────────────────────┐
           │                │ Standard Encoder    │
           │                │ - MOVZ/MOVK/BR     │
           ▼                └─────────────────────┘
┌─────────────────────┐              │
│ Code Generator      │              ▼
│ - FunctionCall      │    ┌─────────────────────┐
│ - RoutineCall       │───▶│ InstructionStream   │
└─────────────────────┘    │ - Label Management  │
                           └─────────────────────┘
```

### Core Classes

#### VeneerManager
- **Purpose**: Orchestrates veneer generation
- **Key Methods**:
  - `generate_veneers()`: Creates all needed veneers
  - `has_veneer()`: Checks if veneer exists for function
  - `get_veneer_label()`: Returns label name for function
  - `create_veneer()`: Generates individual veneer instructions

#### ExternalFunctionScanner
- **Purpose**: Identifies external function calls in AST
- **Key Methods**:
  - `scan()`: Traverses AST and collects external functions
  - `visit(FunctionCall&)`: Handles function expressions
  - `visit(RoutineCallStatement&)`: Handles routine statements

## Implementation Details

### Veneer Generation Process

1. **Discovery Phase**
   ```cpp
   // Scan AST for external function calls
   std::set<std::string> external_functions = external_scanner_.scan(program);
   ```

2. **Generation Phase**
   ```cpp
   // Generate veneers using standard pipeline
   veneer_manager_.generate_veneers(external_functions, instruction_stream_, label_manager_);
   ```

3. **Integration Phase**
   ```cpp
   // Function calls use standard BL to veneer labels
   if (veneer_manager_.has_veneer(function_name)) {
       std::string veneer_label = function_name + "_veneer";
       emit(Encoder::create_branch_with_link(veneer_label));
   }
   ```

### Veneer Structure

Each veneer follows this pattern:
```assembly
FUNCTION_NAME_veneer:
    MOVZ X16, #<bits_0_15>           ; Load immediate into X16[15:0]
    MOVK X16, #<bits_16_31>, LSL #16 ; Insert into X16[31:16]
    MOVK X16, #<bits_32_47>, LSL #32 ; Insert into X16[47:32]
    BR X16                           ; Branch to address in X16
```

**Why X16?**
- X16 (IP0) is designated by ARM64 ABI for intra-procedure calls
- Safe to use as temporary during function call setup
- Not allocated by register manager for variables
- Standard practice for veneers and trampolines

### Runtime Function Resolution

```cpp
void* VeneerManager::get_runtime_function_pointer(const std::string& function_name) const {
    try {
        const RuntimeFunction& func = RuntimeManager::instance().get_function(function_name);
        return func.address;
    } catch (const std::exception&) {
        return nullptr;
    }
}
```

## Adding New External Functions

### Step 1: Register Runtime Function
```cpp
// In RuntimeSymbols.cpp
void RuntimeSymbols::registerAll(SymbolTable& symbol_table) {
    // Add your function
    registerRuntimeFunction(symbol_table, "MY_FUNCTION", {
        {VarType::INTEGER, false}, // param1: integer
        {VarType::FLOAT, false}    // param2: float
    });
}
```

### Step 2: Implement Runtime Function
```cpp
// In runtime implementation
extern "C" int my_function(int param1, double param2) {
    // Your implementation
    return result;
}
```

### Step 3: Register with RuntimeManager
```cpp
// In runtime registration
RuntimeManager::instance().register_function("MY_FUNCTION", 
    reinterpret_cast<void*>(my_function), 2);
```

### Step 4: Use in BCPL Code
```bcpl
LET START() BE $(
    LET result = MY_FUNCTION(42, 3.14)
    WRITEF("Result: %d*N", result)
$)
```

The veneer system will automatically:
1. Detect the external function call
2. Generate a veneer with proper label
3. Link BL instructions to the veneer
4. Execute the runtime function

## Debugging Guide

### Enable Trace Output
```bash
./NewBCPL --run --trace-codegen your_program.bcl
```

### Key Debug Information

1. **External Function Detection**
   ```
   [DEBUG] Found 4 unique external functions:
   [DEBUG]   - NEWLINE
   [DEBUG]   - RAND
   [DEBUG]   - WRITEF
   [DEBUG]   - WRITES
   ```

2. **Veneer Generation**
   ```
   [VeneerManager] Generated 4 veneers using standard pipeline...
   [VeneerManager] Created veneer: WRITES_veneer for function: WRITES
   ```

3. **Assembly Output**
   ```
   WRITES_veneer:
   103484034 0xd28f2110  MOVZ X16, #30984
   103484038 0xf2a05030  MOVK X16, #641, LSL #16
   10348403c 0xf2c00030  MOVK X16, #1, LSL #32
   103484040 0xd61f0200  BR X16
   ```

### Common Debug Scenarios

#### Veneer Not Generated
**Symptom**: "Undefined label 'FUNCTION_veneer'"
**Causes**:
- Function not registered in RuntimeManager
- ExternalFunctionScanner not detecting call
- Function name mismatch

**Solution**:
```cpp
// Check if function is registered
if (RuntimeManager::instance().is_function_registered("FUNCTION_NAME")) {
    std::cout << "Function is registered" << std::endl;
}
```

#### Wrong Function Pointer
**Symptom**: Crash or unexpected behavior when calling function
**Causes**:
- Incorrect function registration
- ABI mismatch (calling convention)
- Type conversion issues

**Solution**:
```cpp
// Verify function pointer
void* ptr = RuntimeManager::instance().get_function("FUNCTION_NAME").address;
std::cout << "Function pointer: " << ptr << std::endl;
```

#### Label Resolution Issues
**Symptom**: Linker errors about undefined labels
**Causes**:
- InstructionStream not defining labels properly
- Timing issues in generation order

**Solution**: Check label definition order in debug output

## Performance Considerations

### Veneer Overhead
- **Per-call cost**: 4 instructions (MOVZ/MOVK/MOVK/BR)
- **Memory overhead**: ~16 bytes per unique external function
- **Cache impact**: Minimal - veneers placed with main code

### Optimization Opportunities

1. **Veneer Sharing**: Multiple calls to same function use one veneer
2. **Direct Calls**: Inline frequently used functions
3. **Conditional Generation**: Only create veneers when needed

### Performance Monitoring
```cpp
// Add timing to veneer generation
auto start = std::chrono::high_resolution_clock::now();
veneer_manager_.generate_veneers(external_functions, instruction_stream_, label_manager_);
auto end = std::chrono::high_resolution_clock::now();
std::cout << "Veneer generation took: " 
          << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() 
          << " microseconds" << std::endl;
```

## Troubleshooting

### Build Issues

#### Missing Includes
```cpp
#include "VeneerManager.h"
#include "ExternalFunctionScanner.h"
#include "InstructionStream.h"
#include "LabelManager.h"
```

#### Linking Errors
- Ensure RuntimeManager is properly initialized
- Check that external functions are compiled and linked
- Verify symbol visibility (extern "C" for C functions)

### Runtime Issues

#### Segmentation Faults
1. **Check function signatures** - Mismatched parameters cause stack corruption
2. **Verify ABI compliance** - ARM64 calling convention must be followed
3. **Debug register usage** - Ensure X16 isn't being used elsewhere

#### Incorrect Results
1. **Parameter ordering** - BCPL vs C parameter order
2. **Type conversions** - Integer/float/pointer mismatches
3. **Return value handling** - Ensure proper register usage

### Testing Strategy

#### Unit Tests
```cpp
// Test external function detection
std::set<std::string> functions = scanner.scan(test_program);
ASSERT_TRUE(functions.count("WRITES") > 0);

// Test veneer generation
veneer_manager.generate_veneers(functions, stream, label_mgr);
ASSERT_TRUE(veneer_manager.has_veneer("WRITES"));
```

#### Integration Tests
```bcpl
// Test actual function calls
LET START() BE $(
    WRITES("Testing veneer system")
    NEWLINE()
    LET x = RAND(100)
    WRITEF("Random: %d*N", x)
$)
```

## Best Practices

### Code Organization
1. **Keep veneers simple** - Only load address and branch
2. **Use standard registers** - Follow ARM64 ABI (X16 for veneers)
3. **Maintain consistency** - Use established patterns

### Error Handling
1. **Validate function registration** - Check before veneer creation
2. **Graceful degradation** - Fallback to direct calls when possible
3. **Clear error messages** - Help developers debug issues

### Documentation
1. **Document new functions** - Include in RuntimeSymbols.cpp
2. **Update test cases** - Verify veneer generation works
3. **Version compatibility** - Note any ABI changes

## Future Enhancements

### Planned Improvements
1. **Lazy veneer generation** - Create only when needed
2. **Hot path optimization** - Inline critical functions
3. **Cross-module support** - Inter-library calls
4. **Debug instrumentation** - Call tracing and profiling

### Extension Points
- **Custom veneer types** - Support different calling conventions
- **Dynamic resolution** - Runtime function loading
- **Security features** - Call validation and sandboxing

This guide provides the technical foundation for understanding and extending the veneer system. For additional support, refer to the source code comments and the STANDARDIZED_VENEER_SYSTEM.md feature documentation.