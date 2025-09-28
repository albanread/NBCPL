#include "../../AST.h"
#include "../ASTAnalyzer.h"
#include <iostream>
#include <string>

/**
 * @brief Process all methods (including inherited ones) for a class
 * @param class_name The name of the class to process
 */
void ASTAnalyzer::processClassMethods(const std::string& class_name) {
    if (!class_table_) {
        if (trace_enabled_) std::cout << "[ANALYZER WARNING] Class table not available, inheritance analysis skipped" << std::endl;
        return;
    }

    // Get the class entry
    ClassTableEntry* class_entry = class_table_->get_class(class_name);
    if (!class_entry) {
        if (trace_enabled_) std::cout << "[ANALYZER WARNING] Class '" << class_name << "' not found in class table" << std::endl;
        return;
    }

    // Loop through all methods in the class table entry
    for (const auto& [qual_name, method_info] : class_entry->member_methods) {
        // Register the method with the analyzer
        if (qual_name.find("::") != std::string::npos) {
            // Determine if it's a function or routine by looking at the symbol table
            Symbol symbol;
            if (symbol_table_ && symbol_table_->lookup(qual_name, symbol)) {
                if (symbol.kind == SymbolKind::FUNCTION) {
                    local_function_names_.insert(qual_name);
                    // Check if it's a float function
                    if (symbol.type == VarType::FLOAT) {
                        local_float_function_names_.insert(qual_name);
                        function_return_types_[qual_name] = VarType::FLOAT;
                    } else {
                        function_return_types_[qual_name] = VarType::INTEGER;
                    }
                } else if (symbol.kind == SymbolKind::ROUTINE) {
                    local_routine_names_.insert(qual_name);
                    function_return_types_[qual_name] = VarType::INTEGER;
                }
            } else {
                // Default to treating it as a regular function if symbol info is missing
                local_function_names_.insert(qual_name);
                function_return_types_[qual_name] = VarType::INTEGER;
            }

            // Always initialize metrics for the method
            function_metrics_[qual_name] = FunctionMetrics();
        }
    }
}