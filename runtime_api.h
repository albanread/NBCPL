#ifndef RUNTIME_API_H
#define RUNTIME_API_H

#include <cstdint>

/**
 * @file runtime_api.h
 * @brief Central Runtime API - Publisher/Import Contract
 * 
 * This header defines the contract between the runtime library (publisher)
 * and the compiler (importer). The runtime publishes its API through a
 * manifest, and the compiler imports this manifest as its sole source of truth.
 * 
 * This design ensures:
 * - Single source of truth for all runtime functions
 * - Loose coupling between runtime and compiler
 * - Easy addition/modification of runtime functions
 * - Automatic verification and listing capabilities
 */

// Forward declaration for function pointer type
using RuntimeFunctionPtr = void*;

/**
 * @brief Runtime function types for different calling conventions
 */
enum class RuntimeFunctionType {
    STANDARD,           // Regular integer/pointer functions
    FLOAT,              // Functions that work with float parameters/returns
    ROUTINE,            // Procedures that don't return values
    FLOAT_ROUTINE       // Procedures that handle float parameters but don't return
};

/**
 * @brief Return type categories for runtime functions
 */
enum class RuntimeReturnType {
    INTEGER,            // Returns int/pointer (most common)
    FLOAT,              // Returns floating-point value  
    STRING_LIST,        // Returns pointer to string list
    INT_VECTOR,         // Returns pointer to integer vector
    FLOAT_VECTOR,       // Returns pointer to float vector
    STRING,             // Returns pointer to string
    VOID                // No return value (routines)
};

/**
 * @brief Complete descriptor for a single runtime function
 * 
 * This structure contains all metadata needed by the compiler to:
 * - Register the function in symbol tables
 * - Generate correct calling code
 * - Emit proper linker symbols for standalone mode
 * - Provide user documentation
 */
struct RuntimeFunctionDescriptor {
    const char* veneer_name;            // Name used in BCPL code (e.g., "JOIN")
    const char* linker_symbol;          // Label for standalone executables (e.g., "_JOIN")
    RuntimeFunctionPtr function_pointer; // Actual pointer to the C++ implementation
    int arg_count;                      // Number of arguments expected
    RuntimeFunctionType function_type;   // Calling convention type
    RuntimeReturnType return_type;       // What the function returns
    const char* description;            // Human-readable description
    const char* category;               // Functional category (e.g., "I/O", "Math", "Memory")
};

/**
 * @brief Get the complete runtime function manifest
 * 
 * This is the primary interface between runtime (publisher) and compiler (importer).
 * The runtime publishes its complete API through this function, and the compiler
 * imports it as the authoritative source of all available runtime functions.
 * 
 * @param count [out] Number of functions in the manifest
 * @return Pointer to array of RuntimeFunctionDescriptor structs
 */
const RuntimeFunctionDescriptor* get_runtime_manifest(int& count);

/**
 * @brief Get manifest version for compatibility checking
 * 
 * @return Version string in format "major.minor.patch"
 */
const char* get_runtime_api_version();

/**
 * @brief Initialize the runtime system
 * 
 * Must be called before any runtime functions are used.
 * This replaces scattered initialization calls.
 */
void initialize_runtime_system();

/**
 * @brief Shutdown the runtime system
 * 
 * Clean shutdown of runtime resources.
 */
void shutdown_runtime_system();

/**
 * @brief Verify runtime function integrity
 * 
 * Performs internal consistency checks on the manifest.
 * Should be called during compiler startup for debugging.
 * 
 * @return true if all functions are valid, false otherwise
 */
bool verify_runtime_manifest();

/**
 * @brief Look up a function descriptor by veneer name
 * 
 * Convenience function for compiler to find specific function metadata.
 * 
 * @param veneer_name The BCPL function name to look up
 * @return Pointer to descriptor if found, nullptr otherwise
 */
const RuntimeFunctionDescriptor* lookup_runtime_function(const char* veneer_name);

/**
 * @brief Get functions by category
 * 
 * Helper for organizing function listings and documentation.
 * 
 * @param category Category name (e.g., "I/O", "Math")
 * @param count [out] Number of functions in this category
 * @return Array of pointers to descriptors in the category
 */
const RuntimeFunctionDescriptor** get_functions_by_category(const char* category, int& count);

#endif // RUNTIME_API_H