#include "RuntimeImporter.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <map>
#include <vector>
#include <cstring>

/**
 * @file RuntimeImporter.cpp
 * @brief Compiler-Side Runtime Importer Implementation
 * 
 * This file implements the "importer" side of the publish/import model.
 * It consumes the runtime manifest and registers all functions with
 * the compiler's internal systems, replacing all scattered registration.
 */

bool RuntimeImporter::import_all_runtime_functions(SymbolTable& symbol_table, bool enable_tracing) {
    if (enable_tracing) {
        std::cout << "=== Importing Runtime Functions from Manifest ===" << std::endl;
        std::cout << "API Version: " << get_runtime_api_version() << std::endl;
    }
    
    // Get the manifest from the runtime
    int function_count = 0;
    const RuntimeFunctionDescriptor* manifest = get_runtime_manifest(function_count);
    
    if (!manifest || function_count <= 0) {
        std::cerr << "FATAL: Failed to get runtime manifest!" << std::endl;
        return false;
    }
    
    if (enable_tracing) {
        std::cout << "Found " << function_count << " functions in manifest" << std::endl;
    }
    
    // Verify manifest integrity first
    if (!verify_runtime_manifest()) {
        std::cerr << "FATAL: Runtime manifest failed integrity check!" << std::endl;
        return false;
    }
    
    int successful_runtime_registrations = 0;
    int successful_symbol_registrations = 0;
    
    // Import each function from the manifest
    for (int i = 0; i < function_count; ++i) {
        const auto& desc = manifest[i];
        
        // Register with RuntimeManager for JIT execution
        if (register_with_runtime_manager(desc, enable_tracing)) {
            successful_runtime_registrations++;
        } else {
            std::cerr << "ERROR: Failed to register runtime function: " << desc.veneer_name << std::endl;
        }
        
        // Register with SymbolTable for compile-time resolution
        if (register_with_symbol_table(desc, symbol_table, enable_tracing)) {
            successful_symbol_registrations++;
        } else {
            std::cerr << "ERROR: Failed to register symbol: " << desc.veneer_name << std::endl;
        }
    }
    
    if (enable_tracing) {
        std::cout << "=== Import Results ===" << std::endl;
        std::cout << "Runtime registrations: " << successful_runtime_registrations << "/" << function_count << std::endl;
        std::cout << "Symbol registrations:  " << successful_symbol_registrations << "/" << function_count << std::endl;
    }
    
    // Success if all functions were registered
    bool success = (successful_runtime_registrations == function_count) && 
                   (successful_symbol_registrations == function_count);
    
    if (enable_tracing) {
        std::cout << "=== Runtime Function Import " << (success ? "COMPLETED" : "FAILED") << " ===" << std::endl;
    }
    
    return success;
}

void RuntimeImporter::verify_import_completeness(SymbolTable& symbol_table) {
    std::cout << "=== Verifying Runtime Function Import Completeness ===" << std::endl;
    
    int function_count = 0;
    const RuntimeFunctionDescriptor* manifest = get_runtime_manifest(function_count);
    
    std::vector<std::string> missing_functions;
    
    for (int i = 0; i < function_count; ++i) {
        const auto& desc = manifest[i];
        
        Symbol symbol;
        if (!symbol_table.lookup(desc.veneer_name, "Global", symbol)) {
            missing_functions.push_back(desc.veneer_name);
        }
    }
    
    if (!missing_functions.empty()) {
        std::cerr << "FATAL: Runtime function import verification failed!" << std::endl;
        std::cerr << "Missing functions in symbol table:" << std::endl;
        for (const auto& name : missing_functions) {
            std::cerr << "  - " << name << std::endl;
        }
        std::cerr << std::endl;
        std::cerr << "This indicates a serious problem with the runtime import system." << std::endl;
        std::cerr << "Check that import_all_runtime_functions() completed successfully." << std::endl;
        exit(1);
    }
    
    std::cout << "✅ Runtime function import verification passed (" << function_count << " functions)" << std::endl;
}

void RuntimeImporter::list_runtime_functions(bool show_categories, const char* category_filter) {
    int function_count = 0;
    const RuntimeFunctionDescriptor* manifest = get_runtime_manifest(function_count);
    
    std::cout << "=============================================================================" << std::endl;
    std::cout << "                         RUNTIME FUNCTION MANIFEST" << std::endl;
    std::cout << "                            (Publisher/Import Model)" << std::endl;
    std::cout << "=============================================================================" << std::endl;
    std::cout << "API Version: " << get_runtime_api_version() << std::endl;
    std::cout << "Total Functions: " << function_count << std::endl;
    
    if (category_filter) {
        std::cout << "Filter: " << category_filter << " functions only" << std::endl;
    }
    
    std::cout << "=============================================================================" << std::endl;
    
    if (show_categories) {
        // Group by category
        std::map<std::string, std::vector<const RuntimeFunctionDescriptor*>> by_category;
        
        for (int i = 0; i < function_count; ++i) {
            const auto& desc = manifest[i];
            if (!category_filter || strcmp(desc.category, category_filter) == 0) {
                by_category[desc.category].push_back(&desc);
            }
        }
        
        for (const auto& category_pair : by_category) {
            print_category_header(category_pair.first.c_str());
            
            std::cout << std::left << std::setw(20) << "NAME" 
                      << std::setw(8) << "PARAMS" 
                      << std::setw(15) << "TYPE"
                      << std::setw(15) << "RETURNS"
                      << "DESCRIPTION" << std::endl;
            std::cout << std::string(79, '-') << std::endl;
            
            for (const auto* desc : category_pair.second) {
                print_function_info(*desc, false);
            }
            std::cout << std::endl;
        }
    } else {
        // Simple list
        std::cout << std::left << std::setw(20) << "NAME" 
                  << std::setw(8) << "PARAMS" 
                  << std::setw(15) << "TYPE"
                  << std::setw(15) << "RETURNS"
                  << std::setw(12) << "CATEGORY"
                  << "DESCRIPTION" << std::endl;
        std::cout << std::string(90, '-') << std::endl;
        
        for (int i = 0; i < function_count; ++i) {
            const auto& desc = manifest[i];
            if (!category_filter || strcmp(desc.category, category_filter) == 0) {
                print_function_info(desc, true);
            }
        }
    }
    
    std::cout << "=============================================================================" << std::endl;
    
    // Print statistics
    int total, by_type[4] = {0}, by_return[7] = {0};
    get_runtime_statistics(total, by_type, by_return);
    
    std::cout << "Function Type Distribution:" << std::endl;
    std::cout << "  Standard: " << by_type[0] << ", Float: " << by_type[1] 
              << ", Routine: " << by_type[2] << ", Float Routine: " << by_type[3] << std::endl;
    std::cout << "=============================================================================" << std::endl;
}

void RuntimeImporter::get_runtime_statistics(int& total_functions, int by_type[4], int by_return_type[7]) {
    int function_count = 0;
    const RuntimeFunctionDescriptor* manifest = get_runtime_manifest(function_count);
    
    total_functions = function_count;
    
    // Initialize arrays
    for (int i = 0; i < 4; i++) by_type[i] = 0;
    for (int i = 0; i < 7; i++) by_return_type[i] = 0;
    
    for (int i = 0; i < function_count; ++i) {
        const auto& desc = manifest[i];
        
        // Count by function type
        by_type[static_cast<int>(desc.function_type)]++;
        
        // Count by return type
        by_return_type[static_cast<int>(desc.return_type)]++;
    }
}

const char* RuntimeImporter::get_linker_symbol(const char* veneer_name) {
    const RuntimeFunctionDescriptor* desc = lookup_runtime_function(veneer_name);
    return desc ? desc->linker_symbol : nullptr;
}

bool RuntimeImporter::function_exists(const char* veneer_name) {
    return lookup_runtime_function(veneer_name) != nullptr;
}

const RuntimeFunctionDescriptor* RuntimeImporter::get_function_descriptor(const char* veneer_name) {
    return lookup_runtime_function(veneer_name);
}

// Private implementation methods

VarType RuntimeImporter::convert_return_type(RuntimeReturnType runtime_type) {
    switch (runtime_type) {
        case RuntimeReturnType::INTEGER:      return VarType::INTEGER;
        case RuntimeReturnType::FLOAT:        return VarType::FLOAT;
        case RuntimeReturnType::STRING_LIST:  return VarType::POINTER_TO_STRING_LIST;
        case RuntimeReturnType::INT_VECTOR:   return VarType::POINTER_TO_INT_VEC;
        case RuntimeReturnType::FLOAT_VECTOR: return VarType::POINTER_TO_FLOAT_VEC;
        case RuntimeReturnType::STRING:       return VarType::INTEGER; // String pointers are integers
        case RuntimeReturnType::VOID:         return VarType::INTEGER; // Routines still return integer for ABI
        default:                              return VarType::UNKNOWN;
    }
}

FunctionType RuntimeImporter::convert_function_type(RuntimeFunctionType runtime_type) {
    switch (runtime_type) {
        case RuntimeFunctionType::STANDARD:      return FunctionType::STANDARD;
        case RuntimeFunctionType::FLOAT:         return FunctionType::FLOAT;
        case RuntimeFunctionType::ROUTINE:       return FunctionType::STANDARD;
        case RuntimeFunctionType::FLOAT_ROUTINE: return FunctionType::FLOAT;
        default:                                 return FunctionType::STANDARD;
    }
}

SymbolKind RuntimeImporter::convert_to_symbol_kind(RuntimeFunctionType func_type, RuntimeReturnType return_type) {
    switch (func_type) {
        case RuntimeFunctionType::STANDARD:
            if (return_type == RuntimeReturnType::STRING_LIST) {
                return SymbolKind::RUNTIME_LIST_FUNCTION;
            } else if (return_type == RuntimeReturnType::VOID) {
                return SymbolKind::RUNTIME_ROUTINE;
            } else {
                return SymbolKind::RUNTIME_FUNCTION;
            }
            
        case RuntimeFunctionType::FLOAT:
            if (return_type == RuntimeReturnType::VOID) {
                return SymbolKind::RUNTIME_FLOAT_ROUTINE;
            } else {
                return SymbolKind::RUNTIME_FLOAT_FUNCTION;
            }
            
        case RuntimeFunctionType::ROUTINE:
            return SymbolKind::RUNTIME_ROUTINE;
            
        case RuntimeFunctionType::FLOAT_ROUTINE:
            return SymbolKind::RUNTIME_FLOAT_ROUTINE;
            
        default:
            return SymbolKind::RUNTIME_FUNCTION;
    }
}

bool RuntimeImporter::register_with_runtime_manager(const RuntimeFunctionDescriptor& desc, bool enable_tracing) {
    try {
        auto& manager = RuntimeManager::instance();
        
        manager.register_function(
            desc.veneer_name,
            desc.arg_count,
            desc.function_pointer,
            convert_function_type(desc.function_type),
            convert_return_type(desc.return_type)
        );
        
        if (enable_tracing) {
            std::cout << "Runtime: " << std::left << std::setw(20) << desc.veneer_name 
                      << " -> " << desc.function_pointer << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Runtime registration failed for " << desc.veneer_name << ": " << e.what() << std::endl;
        return false;
    }
}

bool RuntimeImporter::register_with_symbol_table(const RuntimeFunctionDescriptor& desc, SymbolTable& symbol_table, bool enable_tracing) {
    try {
        // Create symbol with proper "Global" context
        Symbol symbol(
            desc.veneer_name,
            convert_to_symbol_kind(desc.function_type, desc.return_type),
            convert_return_type(desc.return_type),
            0,  // scope_level
            "Global"  // function_name - this is key for proper lookup!
        );
        
        // TODO: Add parameter information if needed in the future
        // symbol.parameters = ...;
        
        if (!symbol_table.addSymbol(symbol)) {
            std::cerr << "Symbol table rejected: " << desc.veneer_name << " (duplicate?)" << std::endl;
            return false;
        }
        
        if (enable_tracing) {
            std::cout << "Symbol:  " << std::left << std::setw(20) << desc.veneer_name 
                      << " (" << desc.description << ")" << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Symbol registration failed for " << desc.veneer_name << ": " << e.what() << std::endl;
        return false;
    }
}

void RuntimeImporter::print_function_info(const RuntimeFunctionDescriptor& desc, bool show_category) {
    // Convert types to readable strings
    std::string type_str;
    switch (desc.function_type) {
        case RuntimeFunctionType::STANDARD:      
            type_str = (desc.return_type == RuntimeReturnType::VOID) ? "ROUTINE" : "FUNCTION"; 
            break;
        case RuntimeFunctionType::FLOAT:         
            type_str = (desc.return_type == RuntimeReturnType::VOID) ? "FLOAT_ROUTINE" : "FLOAT_FUNC"; 
            break;
        case RuntimeFunctionType::ROUTINE:       type_str = "ROUTINE"; break;
        case RuntimeFunctionType::FLOAT_ROUTINE: type_str = "FLOAT_ROUTINE"; break;
    }
    
    std::string return_str;
    switch (desc.return_type) {
        case RuntimeReturnType::INTEGER:      return_str = "INTEGER"; break;
        case RuntimeReturnType::FLOAT:        return_str = "FLOAT"; break;
        case RuntimeReturnType::STRING_LIST:  return_str = "STRING_LIST"; break;
        case RuntimeReturnType::INT_VECTOR:   return_str = "INT_VECTOR"; break;
        case RuntimeReturnType::FLOAT_VECTOR: return_str = "FLOAT_VECTOR"; break;
        case RuntimeReturnType::STRING:       return_str = "STRING"; break;
        case RuntimeReturnType::VOID:         return_str = "VOID"; break;
    }
    
    std::cout << std::left << std::setw(20) << desc.veneer_name
              << std::setw(8) << desc.arg_count
              << std::setw(15) << type_str
              << std::setw(15) << return_str;
    
    if (show_category) {
        std::cout << std::setw(12) << desc.category;
    }
    
    std::cout << desc.description << std::endl;
}

void RuntimeImporter::print_category_header(const char* category) {
    std::cout << std::endl;
    std::cout << ">>> " << category << " Functions <<<" << std::endl;
}