# Runtime Function Registration Process

## Overview

This document describes the standardized process for adding, modifying, or removing runtime functions in the NewBCPL compiler. All runtime functions must be registered through the centralized `RuntimeRegistry` system to ensure consistency and prevent registration issues.

## Key Files

- **`RuntimeRegistry.h`** - Header defining the registry structure
- **`RuntimeRegistry.cpp`** - Master registry containing all runtime functions
- **`runtime/`** - Directory containing runtime function implementations

## Process for Adding a New Runtime Function

### Step 1: Implement the Runtime Function

1. **Create or locate the implementation file** in the `runtime/` directory:

```cpp
// In runtime/my_new_functions.cpp
extern "C" {
    int MY_NEW_FUNCTION(int param1, int param2) {
        // Implementation here
        return result;
    }
}
```

2. **Add function declaration** to the appropriate header file or create a new one:

```cpp
// In runtime/my_new_functions.h
extern "C" {
    int MY_NEW_FUNCTION(int param1, int param2);
}
```

### Step 2: Add to Runtime Registry

1. **Add forward declaration** in `RuntimeRegistry.cpp`:

```cpp
// Add to the extern "C" block at the top of RuntimeRegistry.cpp
extern "C" {
    // ... existing declarations ...
    int MY_NEW_FUNCTION(int param1, int param2);
}
```

2. **Add registry entry** to `RUNTIME_FUNCTION_REGISTRY` array:

```cpp
// Add to the appropriate section in RUNTIME_FUNCTION_REGISTRY
{
    "MY_NEW_FUNCTION",                          // Veneer name (used in BCPL code)
    reinterpret_cast<void*>(MY_NEW_FUNCTION),   // Function pointer
    "_MY_NEW_FUNCTION",                         // Standalone label
    2,                                          // Parameter count
    FunctionType::STANDARD,                     // Function type
    VarType::INTEGER,                           // Return type
    SymbolKind::RUNTIME_FUNCTION,               // Symbol kind
    "Description of what this function does"    // Human-readable description
},
```

### Step 3: Choose Correct Types

#### Function Types:
- **`FunctionType::STANDARD`** - Regular integer/pointer functions
- **`FunctionType::FLOAT`** - Functions that work with float parameters

#### Return Types:
- **`VarType::INTEGER`** - Returns integer or pointer (most common)
- **`VarType::FLOAT`** - Returns floating-point value
- **`VarType::POINTER_TO_STRING_LIST`** - Returns pointer to string list
- **`VarType::POINTER_TO_INT_VEC`** - Returns pointer to integer vector
- **`VarType::POINTER_TO_FLOAT_VEC`** - Returns pointer to float vector

#### Symbol Kinds:
- **`SymbolKind::RUNTIME_FUNCTION`** - Functions that return a value
- **`SymbolKind::RUNTIME_FLOAT_FUNCTION`** - Functions that return float
- **`SymbolKind::RUNTIME_LIST_FUNCTION`** - Functions that return lists
- **`SymbolKind::RUNTIME_ROUTINE`** - Procedures that don't return values
- **`SymbolKind::RUNTIME_FLOAT_ROUTINE`** - Procedures handling float params

### Step 4: Test the Function

1. **Compile the compiler**:
```bash
./build.sh
```

2. **Verify registration**:
```bash
./NewBCPL --list-runtime | grep MY_NEW_FUNCTION
```

3. **Create a test BCPL program**:
```bcpl
LET START() BE
$(
    WRITES("Testing MY_NEW_FUNCTION...*N")
    LET result = MY_NEW_FUNCTION(10, 20)
    WRITES("Result: ")
    WRITEN(result)
    WRITES("*N")
$)
```

4. **Test compilation and execution**:
```bash
./NewBCPL test_my_function.bcl --run
```

## Process for Modifying Existing Functions

### Step 1: Locate the Function

1. **Find in registry**:
```bash
./NewBCPL --list-runtime | grep FUNCTION_NAME
```

2. **Find implementation**:
```bash
grep -r "FUNCTION_NAME" runtime/
```

### Step 2: Make Changes

1. **Update implementation** in the runtime files
2. **Update registry entry** if signature changed (parameter count, types)
3. **Update description** if behavior changed

### Step 3: Test Changes

Follow the testing process from Step 4 above.

## Process for Removing Functions

### Step 1: Remove from Registry

1. **Delete the entry** from `RUNTIME_FUNCTION_REGISTRY` in `RuntimeRegistry.cpp`
2. **Remove forward declaration** if no longer needed

### Step 2: Update Implementation

1. **Keep implementation** if used by other systems
2. **Remove implementation** if completely unused
3. **Update build files** if removing entire source files

### Step 3: Verify Removal

1. **Check for usage**:
```bash
grep -r "FUNCTION_NAME" . --include="*.bcl"
```

2. **Test compilation** of existing programs to ensure no breakage

## Common Patterns and Examples

### Integer Function Returning Value
```cpp
{
    "LENGTH", reinterpret_cast<void*>(LENGTH), "_LENGTH", 1,
    FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_FUNCTION,
    "Get length of string or array"
},
```

### Void Routine (No Return Value)
```cpp
{
    "WRITES", reinterpret_cast<void*>(WRITES), "_WRITES", 1,
    FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_ROUTINE,
    "Write string to output"
},
```

### Float Function
```cpp
{
    "FSIN", reinterpret_cast<void*>(FSIN), "_FSIN", 1,
    FunctionType::FLOAT, VarType::FLOAT, SymbolKind::RUNTIME_FLOAT_FUNCTION,
    "Sine function (radians)"
},
```

### List-Returning Function
```cpp
{
    "SPLIT", reinterpret_cast<void*>(SPLIT), "_SPLIT", 2,
    FunctionType::STANDARD, VarType::POINTER_TO_STRING_LIST, SymbolKind::RUNTIME_LIST_FUNCTION,
    "Split string into list using delimiter"
},
```

## Troubleshooting

### Function Not Found During Compilation
**Error:** `Symbol 'FUNCTION_NAME' not found in ANY context`

**Solutions:**
1. Check that function is in `RUNTIME_FUNCTION_REGISTRY`
2. Verify spelling matches exactly
3. Ensure `RegisterAllRuntimeSymbols()` is called
4. Run `./NewBCPL --list-runtime` to verify registration

### Infinite Loop in Generated Code
**Error:** Program hangs when calling function

**Solutions:**
1. Check function pointer is correct in registry
2. Verify parameter count matches implementation
3. Ensure symbol lookup finds the function in "Global" context

### Registration Verification Fails
**Error:** `FATAL: Runtime function registration verification failed!`

**Solutions:**
1. Ensure `RegisterAllRuntimeFunctions()` is called before `RegisterAllRuntimeSymbols()`
2. Check for typos in function names
3. Verify all registry entries have valid function pointers

### Duplicate Registration Error
**Error:** `Runtime function 'NAME' already registered`

**Solutions:**
1. Check for duplicate entries in `RUNTIME_FUNCTION_REGISTRY`
2. Ensure old registration calls have been removed
3. Verify function name is unique

## Maintenance Tasks

### Quarterly Review
1. **Audit registry completeness**:
```bash
# Find runtime functions not in registry
grep -r "extern.*C" runtime/ | grep -v RuntimeRegistry
```

2. **Check for unused functions**:
```bash
# Find functions in registry not used in codebase
./NewBCPL --list-runtime | while read line; do
    func=$(echo "$line" | awk '{print $1}')
    if ! grep -r "$func" . --include="*.bcl" >/dev/null; then
        echo "Potentially unused: $func"
    fi
done
```

### Adding New Runtime Categories
When adding a new category of runtime functions:

1. **Add section comment** in `RUNTIME_FUNCTION_REGISTRY`:
```cpp
// =============================================================================
// NEW CATEGORY NAME
// =============================================================================
```

2. **Update documentation** sections in this file
3. **Add examples** to this document

## Best Practices

1. **Use descriptive names** that clearly indicate function purpose
2. **Group related functions** together in the registry
3. **Provide clear descriptions** for documentation
4. **Test thoroughly** with both simple and complex cases
5. **Follow naming conventions** (uppercase for BCPL functions)
6. **Document parameter types** and expected values
7. **Handle error cases** gracefully in implementations
8. **Use appropriate return types** for the data being returned

## Integration with Build System

The centralized registry system integrates with:

- **Runtime Manager** - For JIT function resolution
- **Symbol Table** - For compile-time symbol resolution  
- **Linker** - For standalone executable generation
- **Documentation** - Via `--list-runtime` command

This ensures that adding a function to the registry automatically makes it available across all compilation modes and tooling.