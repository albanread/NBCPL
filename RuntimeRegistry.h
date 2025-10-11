#ifndef RUNTIME_REGISTRY_H
#define RUNTIME_REGISTRY_H

#include "DataTypes.h"
#include "Symbol.h"
#include <cstddef>

/**
 * @brief Information about a single runtime function
 * 
 * This structure contains all the metadata needed to register
 * a runtime function in both the RuntimeManager and SymbolTable.
 */
struct RuntimeFunctionInfo {
    const char* veneer_name;      // Internal name used in BCPL code (e.g., "JOIN")
    void* function_pointer;       // Pointer to the C++ implementation
    const char* standalone_label; // External label for standalone mode (e.g., "_JOIN")
    int parameter_count;          // Number of parameters the function expects
    FunctionType function_type;   // STANDARD, FLOAT, etc.
    VarType return_type;          // Return type (INTEGER, FLOAT, POINTER_TO_STRING_LIST, etc.)
    SymbolKind symbol_kind;       // RUNTIME_FUNCTION, RUNTIME_ROUTINE, etc.
    const char* description;      // Human-readable description for documentation
};

/**
 * @brief Master registry of all runtime functions
 * 
 * This is the single source of truth for all runtime function registrations.
 * Every runtime function callable from BCPL code must have an entry here.
 */
extern const RuntimeFunctionInfo RUNTIME_FUNCTION_REGISTRY[];

/**
 * @brief Number of functions in the registry
 */
extern const size_t RUNTIME_FUNCTION_COUNT;

/**
 * @brief Register all runtime functions from the master registry
 * 
 * This function iterates through RUNTIME_FUNCTION_REGISTRY and registers
 * each function with the RuntimeManager. This should be called during
 * compiler initialization.
 */
void RegisterAllRuntimeFunctions();

/**
 * @brief Register all runtime function symbols from the master registry
 * 
 * This function iterates through RUNTIME_FUNCTION_REGISTRY and registers
 * each function as a symbol in the provided SymbolTable. This should be
 * called after RegisterAllRuntimeFunctions().
 * 
 * @param symbol_table The symbol table to register functions in
 */
void RegisterAllRuntimeSymbols(class SymbolTable& symbol_table);

/**
 * @brief Verify that all runtime functions were successfully registered
 * 
 * This function performs a startup verification check by attempting to
 * look up each function in the registry within the symbol table. If any
 * function is missing, the compiler will halt with a fatal error.
 * 
 * @param symbol_table The symbol table to verify against
 */
void VerifyRuntimeFunctionRegistration(class SymbolTable& symbol_table);

/**
 * @brief List all registered runtime functions to stdout
 * 
 * This function prints a formatted list of all runtime functions in the
 * registry, including their names, parameter counts, types, and descriptions.
 * Used for the --list-runtime command line option.
 */
void ListRuntimeFunctions();

#endif // RUNTIME_REGISTRY_H