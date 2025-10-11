#ifndef RUNTIME_IMPORTER_H
#define RUNTIME_IMPORTER_H

#include "runtime_api.h"
#include "SymbolTable.h"
#include "RuntimeManager.h"

/**
 * @file RuntimeImporter.h
 * @brief Compiler-Side Runtime Importer
 * 
 * This class implements the "importer" side of the publish/import model.
 * It consumes the runtime manifest published by the runtime library
 * and registers all functions with the compiler's internal systems.
 * 
 * This replaces all scattered, inconsistent registration calls with
 * a single, centralized import process.
 */

class RuntimeImporter {
public:
    /**
     * @brief Import and register all runtime functions
     * 
     * This is the main entry point that replaces all previous registration.
     * It imports the manifest from the runtime and registers functions with:
     * - RuntimeManager (for JIT execution)
     * - SymbolTable (for compile-time symbol resolution)
     * 
     * @param symbol_table Symbol table to register functions in
     * @param enable_tracing Whether to output detailed registration info
     * @return true if all functions imported successfully, false otherwise
     */
    static bool import_all_runtime_functions(SymbolTable& symbol_table, bool enable_tracing = false);
    
    /**
     * @brief Verify that all imported functions are accessible
     * 
     * Performs verification by attempting to look up each function
     * in the symbol table after import. Exits with fatal error if
     * any function is missing.
     * 
     * @param symbol_table Symbol table to verify against
     */
    static void verify_import_completeness(SymbolTable& symbol_table);
    
    /**
     * @brief List all available runtime functions
     * 
     * Implements the --list-runtime command by displaying the
     * complete manifest in a formatted table.
     * 
     * @param show_categories Whether to group by category
     * @param category_filter Only show functions in this category (nullptr for all)
     */
    static void list_runtime_functions(bool show_categories = true, const char* category_filter = nullptr);
    
    /**
     * @brief Get statistics about the runtime manifest
     * 
     * @param total_functions [out] Total number of functions
     * @param by_type [out] Array of counts by RuntimeFunctionType (size 4)
     * @param by_return_type [out] Array of counts by RuntimeReturnType (size 7)
     */
    static void get_runtime_statistics(int& total_functions, int by_type[4], int by_return_type[7]);
    
    /**
     * @brief Find linker symbol for a BCPL function name
     * 
     * Used by code generator when emitting calls for standalone executables.
     * 
     * @param veneer_name BCPL function name (e.g., "JOIN")
     * @return Linker symbol name (e.g., "_JOIN") or nullptr if not found
     */
    static const char* get_linker_symbol(const char* veneer_name);
    
    /**
     * @brief Check if a function exists in the manifest
     * 
     * @param veneer_name BCPL function name to check
     * @return true if function exists, false otherwise
     */
    static bool function_exists(const char* veneer_name);
    
    /**
     * @brief Get function descriptor for a BCPL function
     * 
     * @param veneer_name BCPL function name
     * @return Pointer to descriptor or nullptr if not found
     */
    static const RuntimeFunctionDescriptor* get_function_descriptor(const char* veneer_name);

private:
    /**
     * @brief Convert runtime types to compiler internal types
     */
    static VarType convert_return_type(RuntimeReturnType runtime_type);
    static FunctionType convert_function_type(RuntimeFunctionType runtime_type);
    static SymbolKind convert_to_symbol_kind(RuntimeFunctionType func_type, RuntimeReturnType return_type);
    
    /**
     * @brief Register single function with RuntimeManager
     */
    static bool register_with_runtime_manager(const RuntimeFunctionDescriptor& desc, bool enable_tracing);
    
    /**
     * @brief Register single function with SymbolTable
     */
    static bool register_with_symbol_table(const RuntimeFunctionDescriptor& desc, SymbolTable& symbol_table, bool enable_tracing);
    
    /**
     * @brief Format function information for display
     */
    static void print_function_info(const RuntimeFunctionDescriptor& desc, bool show_category = true);
    
    /**
     * @brief Print category header
     */
    static void print_category_header(const char* category);
};

#endif // RUNTIME_IMPORTER_H