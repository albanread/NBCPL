#include "RuntimeRegistry.h"
#include "RuntimeManager.h"
#include "SymbolTable.h"
#include "Symbol.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>

// Forward declarations for runtime functions
extern "C" {
    // Core I/O functions
    void WRITES(int string_ptr);
    void WRITEN(int value);
    void FWRITE(float value);
    void WRITEF(int format_ptr);
    void WRITEC(int char_value);
    int RDCH();
    void NEWLINE();
    
    // String/List operations
    int SPLIT(int string_ptr, int delimiter_ptr);
    int JOIN(int list_ptr, int delimiter_ptr);
    
    // Memory management
    int bcpl_alloc_words(int count, int size, int type);
    int bcpl_alloc_chars(int count);
    int bcpl_getvec(int size);
    int bcpl_fgetvec(int size);
    void bcpl_free(int ptr);
    
    // Math functions
    int RAND(int max_val);
    float RND(int max_val);
    float FRND();
    float FSIN(float angle);
    float FCOS(float angle);
    float FTAN(float angle);
    float FABS(float value);
    float FLOG(float value);
    float FEXP(float value);
    int FIX(float value);
    
    // File I/O
    int FILE_OPEN_READ(int filename_ptr);
    int FILE_OPEN_WRITE(int filename_ptr);
    int FILE_OPEN_APPEND(int filename_ptr);
    int FILE_CLOSE(int handle);
    int FILE_WRITES(int handle, int string_ptr);
    int FILE_READS(int handle);
    
    // System functions
    void finish();
}

/**
 * Master Registry - Single Source of Truth for Runtime Functions
 * 
 * IMPORTANT: Every runtime function that can be called from BCPL code
 * must have an entry in this array. This is the authoritative list
 * used for both RuntimeManager and SymbolTable registration.
 */
const RuntimeFunctionInfo RUNTIME_FUNCTION_REGISTRY[] = {
    // =============================================================================
    // CORE I/O FUNCTIONS
    // =============================================================================
    {
        "WRITES", reinterpret_cast<void*>(WRITES), "_WRITES", 1,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_ROUTINE,
        "Write a string to output"
    },
    {
        "WRITEN", reinterpret_cast<void*>(WRITEN), "_WRITEN", 1,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_ROUTINE,
        "Write an integer to output"
    },
    {
        "FWRITE", reinterpret_cast<void*>(FWRITE), "_FWRITE", 1,
        FunctionType::FLOAT, VarType::FLOAT, SymbolKind::RUNTIME_FLOAT_ROUTINE,
        "Write a float to output"
    },
    {
        "WRITEF", reinterpret_cast<void*>(WRITEF), "_WRITEF", 1,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_ROUTINE,
        "Write formatted output (base case)"
    },
    {
        "WRITEC", reinterpret_cast<void*>(WRITEC), "_WRITEC", 1,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_ROUTINE,
        "Write a character to output"
    },
    {
        "RDCH", reinterpret_cast<void*>(RDCH), "_RDCH", 0,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_FUNCTION,
        "Read a character from input"
    },
    {
        "NEWLINE", reinterpret_cast<void*>(NEWLINE), "_NEWLINE", 0,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_ROUTINE,
        "Output a newline character"
    },

    // =============================================================================
    // STRING/LIST OPERATIONS
    // =============================================================================
    {
        "SPLIT", reinterpret_cast<void*>(SPLIT), "_SPLIT", 2,
        FunctionType::STANDARD, VarType::POINTER_TO_STRING_LIST, SymbolKind::RUNTIME_LIST_FUNCTION,
        "Split a string into a list using delimiter"
    },
    {
        "JOIN", reinterpret_cast<void*>(JOIN), "_JOIN", 2,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_FUNCTION,
        "Join a list of strings using delimiter"
    },

    // =============================================================================
    // MEMORY MANAGEMENT
    // =============================================================================
    {
        "GETVEC", reinterpret_cast<void*>(bcpl_getvec), "_GETVEC", 1,
        FunctionType::STANDARD, VarType::POINTER_TO_INT_VEC, SymbolKind::RUNTIME_FUNCTION,
        "Allocate integer vector"
    },
    {
        "FGETVEC", reinterpret_cast<void*>(bcpl_fgetvec), "_FGETVEC", 1,
        FunctionType::STANDARD, VarType::POINTER_TO_FLOAT_VEC, SymbolKind::RUNTIME_FUNCTION,
        "Allocate float vector"
    },
    {
        "FREEVEC", reinterpret_cast<void*>(bcpl_free), "_FREEVEC", 1,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_ROUTINE,
        "Free allocated vector"
    },

    // =============================================================================
    // MATH FUNCTIONS
    // =============================================================================
    {
        "RAND", reinterpret_cast<void*>(RAND), "_RAND", 1,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_FUNCTION,
        "Generate random integer (0 to max_val-1)"
    },
    {
        "RND", reinterpret_cast<void*>(RND), "_RND", 1,
        FunctionType::FLOAT, VarType::FLOAT, SymbolKind::RUNTIME_FLOAT_FUNCTION,
        "Generate random float (0.0 to max_val)"
    },
    {
        "FRND", reinterpret_cast<void*>(FRND), "_FRND", 0,
        FunctionType::FLOAT, VarType::FLOAT, SymbolKind::RUNTIME_FLOAT_FUNCTION,
        "Generate random float (0.0 to 1.0)"
    },
    {
        "FSIN", reinterpret_cast<void*>(FSIN), "_FSIN", 1,
        FunctionType::FLOAT, VarType::FLOAT, SymbolKind::RUNTIME_FLOAT_FUNCTION,
        "Sine function (radians)"
    },
    {
        "FCOS", reinterpret_cast<void*>(FCOS), "_FCOS", 1,
        FunctionType::FLOAT, VarType::FLOAT, SymbolKind::RUNTIME_FLOAT_FUNCTION,
        "Cosine function (radians)"
    },
    {
        "FTAN", reinterpret_cast<void*>(FTAN), "_FTAN", 1,
        FunctionType::FLOAT, VarType::FLOAT, SymbolKind::RUNTIME_FLOAT_FUNCTION,
        "Tangent function (radians)"
    },
    {
        "FABS", reinterpret_cast<void*>(FABS), "_FABS", 1,
        FunctionType::FLOAT, VarType::FLOAT, SymbolKind::RUNTIME_FLOAT_FUNCTION,
        "Absolute value (float)"
    },
    {
        "FLOG", reinterpret_cast<void*>(FLOG), "_FLOG", 1,
        FunctionType::FLOAT, VarType::FLOAT, SymbolKind::RUNTIME_FLOAT_FUNCTION,
        "Natural logarithm"
    },
    {
        "FEXP", reinterpret_cast<void*>(FEXP), "_FEXP", 1,
        FunctionType::FLOAT, VarType::FLOAT, SymbolKind::RUNTIME_FLOAT_FUNCTION,
        "Exponential function"
    },
    {
        "FIX", reinterpret_cast<void*>(FIX), "_FIX", 1,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_FUNCTION,
        "Convert float to integer"
    },

    // =============================================================================
    // FILE I/O
    // =============================================================================
    {
        "FILE_OPEN_READ", reinterpret_cast<void*>(FILE_OPEN_READ), "_FILE_OPEN_READ", 1,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_FUNCTION,
        "Open file for reading"
    },
    {
        "FILE_OPEN_WRITE", reinterpret_cast<void*>(FILE_OPEN_WRITE), "_FILE_OPEN_WRITE", 1,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_FUNCTION,
        "Open file for writing"
    },
    {
        "FILE_CLOSE", reinterpret_cast<void*>(FILE_CLOSE), "_FILE_CLOSE", 1,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_FUNCTION,
        "Close file handle"
    },
    {
        "FILE_WRITES", reinterpret_cast<void*>(FILE_WRITES), "_FILE_WRITES", 2,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_FUNCTION,
        "Write string to file"
    },
    {
        "FILE_READS", reinterpret_cast<void*>(FILE_READS), "_FILE_READS", 1,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_FUNCTION,
        "Read string from file"
    },

    // =============================================================================
    // SYSTEM FUNCTIONS
    // =============================================================================
    {
        "FINISH", reinterpret_cast<void*>(finish), "_FINISH", 0,
        FunctionType::STANDARD, VarType::INTEGER, SymbolKind::RUNTIME_ROUTINE,
        "Terminate program execution"
    }
};

// Calculate registry size at the end after the array is complete
const size_t RUNTIME_FUNCTION_COUNT = sizeof(RUNTIME_FUNCTION_REGISTRY) / sizeof(RuntimeFunctionInfo);

// =============================================================================
// IMPLEMENTATION FUNCTIONS
// =============================================================================

void RegisterAllRuntimeFunctions() {
    auto& manager = RuntimeManager::instance();
    
    if (manager.isTracingEnabled()) {
        printf("=== Registering %zu runtime functions from master registry ===\n", RUNTIME_FUNCTION_COUNT);
    }
    
    for (size_t i = 0; i < RUNTIME_FUNCTION_COUNT; ++i) {
        const auto& func = RUNTIME_FUNCTION_REGISTRY[i];
        
        try {
            manager.register_function(
                func.veneer_name,
                func.parameter_count,
                func.function_pointer,
                func.function_type,
                func.return_type
            );
            
            if (manager.isTracingEnabled()) {
                printf("Registered: %-20s -> %p (%s)\n", 
                       func.veneer_name, func.function_pointer, func.description);
            }
        } catch (const std::exception& e) {
            fprintf(stderr, "FATAL: Failed to register runtime function '%s': %s\n", 
                   func.veneer_name, e.what());
            exit(1);
        }
    }
    
    if (manager.isTracingEnabled()) {
        printf("=== Runtime function registration complete ===\n");
    }
}

void RegisterAllRuntimeSymbols(SymbolTable& symbol_table) {
    if (RuntimeManager::instance().isTracingEnabled()) {
        printf("=== Registering %zu runtime symbols from master registry ===\n", RUNTIME_FUNCTION_COUNT);
    }
    
    for (size_t i = 0; i < RUNTIME_FUNCTION_COUNT; ++i) {
        const auto& func = RUNTIME_FUNCTION_REGISTRY[i];
        
        // Create symbol with proper function context ("Global")
        Symbol symbol(func.veneer_name, func.symbol_kind, func.return_type, 0, "Global");
        
        // TODO: Add parameter information if needed
        // symbol.parameters = ...;
        
        if (!symbol_table.addSymbol(symbol)) {
            fprintf(stderr, "WARNING: Could not register runtime symbol '%s' (duplicate symbol)\n", 
                   func.veneer_name);
        } else if (RuntimeManager::instance().isTracingEnabled()) {
            printf("Symbol registered: %-20s (%s)\n", func.veneer_name, func.description);
        }
    }
    
    if (RuntimeManager::instance().isTracingEnabled()) {
        printf("=== Runtime symbol registration complete ===\n");
    }
}

void VerifyRuntimeFunctionRegistration(SymbolTable& symbol_table) {
    std::vector<std::string> failed_registrations;
    
    printf("=== Verifying runtime function registration ===\n");
    
    for (size_t i = 0; i < RUNTIME_FUNCTION_COUNT; ++i) {
        const auto& func = RUNTIME_FUNCTION_REGISTRY[i];
        
        Symbol symbol;
        if (!symbol_table.lookup(func.veneer_name, "Global", symbol)) {
            failed_registrations.push_back(func.veneer_name);
        }
    }
    
    if (!failed_registrations.empty()) {
        fprintf(stderr, "FATAL: Runtime function registration verification failed!\n");
        fprintf(stderr, "Missing functions in symbol table:\n");
        for (const auto& name : failed_registrations) {
            fprintf(stderr, "  - %s\n", name.c_str());
        }
        fprintf(stderr, "\nThis indicates a serious problem with the runtime registration system.\n");
        fprintf(stderr, "Check that RegisterAllRuntimeSymbols() was called after RegisterAllRuntimeFunctions().\n");
        exit(1);
    }
    
    printf("✅ Runtime function registration verification passed (%zu functions)\n", 
           RUNTIME_FUNCTION_COUNT);
}

void ListRuntimeFunctions() {
    printf("=============================================================================\n");
    printf("                         RUNTIME FUNCTION REGISTRY\n");
    printf("=============================================================================\n");
    printf("%-20s %-8s %-25s %s\n", "NAME", "PARAMS", "TYPE", "DESCRIPTION");
    printf("-----------------------------------------------------------------------------\n");
    
    for (size_t i = 0; i < RUNTIME_FUNCTION_COUNT; ++i) {
        const auto& func = RUNTIME_FUNCTION_REGISTRY[i];
        
        // Format type string
        std::string type_str;
        switch (func.symbol_kind) {
            case SymbolKind::RUNTIME_FUNCTION:
                type_str = (func.return_type == VarType::FLOAT) ? "FLOAT_FUNC" : "FUNCTION";
                break;
            case SymbolKind::RUNTIME_FLOAT_FUNCTION:
                type_str = "FLOAT_FUNC";
                break;
            case SymbolKind::RUNTIME_LIST_FUNCTION:
                type_str = "LIST_FUNC";
                break;
            case SymbolKind::RUNTIME_ROUTINE:
                type_str = (func.function_type == FunctionType::FLOAT) ? "FLOAT_ROUTINE" : "ROUTINE";
                break;
            case SymbolKind::RUNTIME_FLOAT_ROUTINE:
                type_str = "FLOAT_ROUTINE";
                break;
            default:
                type_str = "UNKNOWN";
                break;
        }
        
        printf("%-20s %-8d %-25s %s\n", 
               func.veneer_name,
               func.parameter_count,
               type_str.c_str(),
               func.description);
    }
    
    printf("=============================================================================\n");
    printf("Total: %zu runtime functions\n", RUNTIME_FUNCTION_COUNT);
    printf("=============================================================================\n");
}