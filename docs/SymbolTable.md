# Symbol Table Design and Implementation Guide

## Overview

The Symbol Table is a centralized, authoritative source for all identifier information throughout the compiler. This document outlines the design, implementation, and integration of the symbol table system.

## Architecture

### Core Components

1. **Symbol**: A class representing a single identifier (variable, function, label, etc.)
   - Contains name, kind, type, scope level, and location information
   - Provides helper methods for common queries (is_local, is_global, etc.)

2. **SymbolTable**: Manages scopes and symbols
   - Implements a stack-based scope system
   - Provides lookup methods that respect shadowing rules
   - Maintains function parameter information

3. **SymbolTableBuilder**: An AST visitor that populates the symbol table
   - Traverses the entire AST to discover all declarations
   - Tracks scope boundaries
   - Resolves types where possible

## Symbol Types and Kinds

### Symbol Kinds

- `LOCAL_VAR`: Local variables declared with LET or FLET
- `STATIC_VAR`: Variables with static storage but local scope
- `GLOBAL_VAR`: Global variables
- `PARAMETER`: Function or routine parameters
- `FUNCTION`: User-defined integer-returning functions
- `FLOAT_FUNCTION`: User-defined float-returning functions
- `ROUTINE`: User-defined procedures (no return value)
- `RUNTIME_FUNCTION`: Runtime-provided integer-returning functions
- `RUNTIME_FLOAT_FUNCTION`: Runtime-provided float-returning functions
- `RUNTIME_ROUTINE`: Runtime-provided procedures
- `LABEL`: Code labels
- `MANIFEST`: Compile-time constants

### Variable Types

- `INTEGER`: 64-bit integers
- `FLOAT`: 64-bit floating point values
- `UNKNOWN`: Type not yet determined

### Function Parameter Information

Each function/routine symbol can store parameter information:
- Type (INTEGER/FLOAT)
- Whether the parameter is optional

## Symbol Location Information

The `SymbolLocation` struct tracks where a symbol is stored:

- `STACK`: Local variables on the stack (offset from FP)
- `DATA`: Static/global variables in the data segment (offset from X28)
- `ABSOLUTE`: Manifest constants with fixed values
- `LABEL`: Code labels (linked to their label name)
- `UNKNOWN`: Not yet allocated

## Integration with Compiler Pipeline

### Compilation Flow

1. **Parsing**: Lexer and Parser build the initial AST
2. **Manifest Resolution**: Replace manifest constants with literal values
3. **Symbol Table Building**: Build comprehensive symbol information (NEW STEP)
4. **Runtime Symbol Registration**: Add all runtime functions to symbol table (NEW STEP)
5. **Semantic Analysis**: Use symbol table for type checking and validations
6. **Optimizations**: Apply optimizations with complete type information
7. **Code Generation**: Use symbol table for variable lookup and allocation

### Benefits

- **Single Source of Truth**: No more inconsistent variable handling
- **Cleaner Code**: Simplified lookup logic in the code generator
- **Better Error Messages**: More precise diagnostics with scope information
- **Enhanced Type Safety**: Proper tracking of float vs. integer variables

## Code Generator Integration

The `NewCodeGenerator` now uses the symbol table:

1. Variable lookup has been refactored to check the symbol table first
2. Type information comes directly from symbols
3. Location information (stack/data) is also stored in the symbol table
4. Runtime function detection uses the symbol table instead of string checks
5. Fallback to previous methods when symbol information is incomplete

## ASTAnalyzer Integration

The `ASTAnalyzer` now:

1. Receives the populated symbol table as input
2. Uses symbol table for type information
3. Still maintains its own metrics for backward compatibility
4. Falls back to inference when symbol information is incomplete

## Usage Example

```cpp
// In main.cpp
// After parsing...
bcpl::SymbolTableBuilder symbol_table_builder(enable_tracing);
std::unique_ptr<bcpl::SymbolTable> symbol_table = symbol_table_builder.build(*ast);

// Register runtime functions and routines
bcpl::RuntimeSymbols::registerAll(*symbol_table);

// Pass to ASTAnalyzer
analyzer.analyze(*ast, symbol_table.get());

// Pass to CodeGenerator
NewCodeGenerator code_generator(
    // Other arguments...
    std::move(symbol_table)
);
```

## Future Enhancements

1. **Extended Type System**: Add support for arrays and custom types
2. **Interprocedural Analysis**: Track function calls and returns more precisely
3. **Link-time Resolution**: Integrate with external symbols
4. **Incremental Compilation**: Cache symbol tables between compilations

## Known Limitations

1. Type inference is still needed in some cases
2. The symbol table requires an initial AST pass
3. Some legacy code still uses the old variable lookup methods
4. Runtime function parameters could be checked more strictly

## Runtime Functions Integration

The symbol table now serves as the single source of truth for runtime functions:

1. **RuntimeSymbols** class populates the symbol table with all available runtime functions
2. Each runtime function is registered with:
   - Return type (INTEGER/FLOAT)
   - Parameter information (types, optionality)
   - Function kind (RUNTIME_FUNCTION/RUNTIME_FLOAT_FUNCTION/RUNTIME_ROUTINE)

3. **Benefits**:
   - Centralized definition of all runtime capabilities
   - Simplified code generator logic
   - No more string-based checks for special function names
   - Potential for stricter parameter validation