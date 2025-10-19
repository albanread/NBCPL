# Runtime Function Registration Process

## Overview

This document describes the streamlined process for adding, modifying, or removing runtime functions in the NewBCPL compiler. All runtime functions are managed through the centralized `runtime_api.cpp` manifest system, which automatically handles both JIT execution and standalone compilation.

## Key Files

- **`runtime_api.cpp`** - Master manifest containing all runtime function declarations and metadata
- **`RuntimeImporter.cpp`** - Automatically imports functions from the manifest (no manual editing required)
- **`runtime/`** - Directory containing runtime function implementations

## Process for Adding a New Runtime Function

### Step 1: Implement the Runtime Function

Create or locate the implementation file in the `runtime/` directory:

```cpp
// In runtime/runtime_core.inc or appropriate runtime file
extern "C" {
    int MY_NEW_FUNCTION(int param1, int param2) {
        // Implementation here
        return param1 + param2;
    }
}
```

**Important:** Use `extern "C"` linkage to prevent C++ name mangling.

### Step 2: Publish the Function in runtime_api.cpp

This is the **only** file you need to edit to make a runtime function available to BCPL programs.

#### 2a. Add Forward Declaration

Add your function to the `extern "C"` block at the top of `runtime_api.cpp`:

```cpp
extern "C" {
    // ... existing declarations ...
    
    // Add your new function here
    int MY_NEW_FUNCTION(int param1, int param2);
}
```

#### 2b. Add to Manifest Array

Add an entry to the `g_runtime_manifest` array in `runtime_api.cpp`:

```cpp
// Add to the appropriate section in g_runtime_manifest[]
{
    "MY_NEW_FUNCTION",                                    // Veneer name (used in BCPL)
    "_MY_NEW_FUNCTION",                                   // Linker symbol
    reinterpret_cast<RuntimeFunctionPtr>(MY_NEW_FUNCTION), // Function pointer
    2,                                                    // Parameter count
    RuntimeFunctionType::STANDARD,                        // Function type
    RuntimeReturnType::INTEGER,                           // Return type
    "Description of what this function does",             // Description
    "MATH"                                               // Category
},
```

### Step 3: Choose Correct Types

#### Function Types:
- **`RuntimeFunctionType::STANDARD`** - Regular integer/pointer functions
- **`RuntimeFunctionType::ROUTINE`** - Procedures that return void
- **`RuntimeFunctionType::FLOAT_ROUTINE`** - Void procedures with float parameters

#### Return Types:
- **`RuntimeReturnType::INTEGER`** - Returns integer or pointer (most common)
- **`RuntimeReturnType::VOID`** - No return value (routines)
- **`RuntimeReturnType::FLOAT`** - Returns floating-point value
- **`RuntimeReturnType::STRING`** - Returns string pointer
- **`RuntimeReturnType::STRING_LIST`** - Returns list of strings

#### Categories:
Use existing categories or create new ones:
- `"I/O"` - Input/output functions
- `"Math"` - Mathematical functions  
- `"Memory"` - Memory management
- `"String"` - String operations
- `"System"` - System functions
- `"TIMING"` - Performance measurement
- `"SDL"` - Graphics/multimedia

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
    WRITEF("Testing MY_NEW_FUNCTION...*N")
    LET result = MY_NEW_FUNCTION(10, 20)
    WRITEF("Result: %N*N", result)
$)
```

4. **Test both execution modes**:
```bash
./NewBCPL --run test_my_function.bcl    # JIT mode
./NewBCPL --exec test_my_function.bcl   # Standalone mode
```

## Process for Modifying Existing Functions

### Step 1: Locate the Function

1. **Find in manifest**:
```bash
./NewBCPL --list-runtime | grep FUNCTION_NAME
```

2. **Find implementation**:
```bash
grep -r "FUNCTION_NAME" runtime/
```

### Step 2: Make Changes

1. **Update implementation** in the runtime files
2. **Update manifest entry** in `runtime_api.cpp` if signature changed (parameter count, types)
3. **Update description** if behavior changed

### Step 3: Test Changes

Follow the testing process from Step 4 above.

## Process for Removing Functions

### Step 1: Remove from runtime_api.cpp

1. **Delete the entry** from `g_runtime_manifest` array
2. **Remove forward declaration** from `extern "C"` block

### Step 2: Handle Implementation

1. **Keep implementation** if used by other systems
2. **Remove implementation** if completely unused
3. **Update build files** if removing entire source files

### Step 3: Verify Removal

1. **Check for usage**:
```bash
grep -r "FUNCTION_NAME" . --include="*.bcl"
```

2. **Test compilation** to ensure no breakage

## Common Patterns and Examples

### Integer Function Returning Value
```cpp
{
    "STRLEN", "_STRLEN", reinterpret_cast<RuntimeFunctionPtr>(STRLEN), 1,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
    "Get length of string", "String"
},
```

### Void Routine (No Return Value)
```cpp
{
    "WRITES", "_WRITES", reinterpret_cast<RuntimeFunctionPtr>(WRITES), 1,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Write string to output", "I/O"
},
```

### Float Function
```cpp
{
    "FSIN", "_FSIN", reinterpret_cast<RuntimeFunctionPtr>(FSIN), 1,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::FLOAT,
    "Sine function (radians)", "Math"
},
```

### List-Returning Function
```cpp
{
    "SPLIT", "_SPLIT", reinterpret_cast<RuntimeFunctionPtr>(SPLIT), 2,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING_LIST,
    "Split string into list using delimiter", "String"
},
```

## Automatic Integration

The `RuntimeImporter` automatically handles:

- **Symbol Table Registration** - Functions appear in compiler symbol table
- **JIT Function Resolution** - Functions callable in `--run` mode  
- **Linker Integration** - Functions available in `--exec` mode
- **Documentation Generation** - Functions appear in `--list-runtime`

**You don't need to manually register functions anywhere else.**

## Troubleshooting

### Function Not Found During Compilation
**Error:** `Symbol 'FUNCTION_NAME' not found`

**Solutions:**
1. Check that function is in `g_runtime_manifest` array in `runtime_api.cpp`
2. Verify spelling matches exactly
3. Ensure the manifest entry is properly formatted
4. Run `./NewBCPL --list-runtime` to verify registration

### Function Missing at Runtime
**Error:** Program crashes when calling function

**Solutions:**
1. Check function pointer is correct in manifest
2. Verify parameter count matches implementation
3. Ensure runtime library contains the function implementation
4. Check that `extern "C"` linkage is used

### Build Errors After Adding Function
**Error:** Compilation fails

**Solutions:**
1. Check syntax of manifest entry (missing commas, brackets)
2. Verify forward declaration matches implementation
3. Ensure function is implemented with correct signature
4. Check for typos in function names

## Best Practices

1. **Use descriptive names** that clearly indicate function purpose
2. **Group related functions** by category in the manifest
3. **Provide clear descriptions** for `--list-runtime` output
4. **Test thoroughly** with both `--run` and `--exec` modes
5. **Follow naming conventions** (uppercase for BCPL functions)
6. **Document parameter expectations** in descriptions
7. **Handle error cases** gracefully in implementations
8. **Use appropriate return types** for the data being returned

## Adding New Function Categories

When adding a new category of runtime functions:

1. **Choose a descriptive category name** (e.g., "GRAPHICS", "NETWORK")
2. **Group related functions** together in the manifest
3. **Add category documentation** to this file
4. **Update examples** section with category-specific patterns

## System Architecture

The simplified architecture works as follows:

1. **Developer** adds function to `runtime_api.cpp` manifest
2. **RuntimeImporter** reads manifest during compiler startup
3. **Compiler** automatically registers functions in symbol table
4. **RuntimeManager** uses manifest for JIT function resolution
5. **Linker** uses manifest symbols for standalone executables

This ensures consistency across all compilation modes with minimal developer overhead.

## Summary: Your New Workflow

Adding a runtime function is now a simple 3-step process:

1. **Implement** the function in a `runtime/` file with `extern "C"` linkage
2. **Publish** the function by adding its declaration and manifest entry to `runtime_api.cpp`  
3. **Test** with `./build.sh` and verify it works in both `--run` and `--exec` modes

That's it! The `RuntimeImporter` handles everything else automatically.