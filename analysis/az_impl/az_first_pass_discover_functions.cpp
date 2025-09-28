#include "../../AST.h"
#include "../ASTAnalyzer.h"
#include "../../NameMangler.h"
#include <iostream>
#include <string>

/**
 * @brief Discovers all function and routine names in the program and initializes their metrics.
 * This is the first pass of the analysis phase.
 */
void ASTAnalyzer::first_pass_discover_functions(Program& program) {
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] --- PASS 1: Discovering all function definitions ---" << std::endl;
    for (const auto& decl : program.declarations) {
        // --- NEW: Handle ClassDeclarations to get context for methods ---
        if (auto* class_decl = dynamic_cast<ClassDeclaration*>(decl.get())) {
            std::string class_name = class_decl->name;
            
            // Process the class itself
            processClassMethods(class_name);
            
            // Also register the explicitly declared methods
            for (const auto& member : class_decl->members) {
                const auto& member_decl = member.declaration;
                if (auto* func_decl = dynamic_cast<FunctionDeclaration*>(member_decl.get())) {
                    std::string mangled_name = NameMangler::mangleMethod(class_name, func_decl->name);
                    local_function_names_.insert(mangled_name);
                    // Preserve existing parameter types if they exist
                    if (function_metrics_.find(mangled_name) == function_metrics_.end()) {
                        function_metrics_[mangled_name] = FunctionMetrics();
                    }

                    if (func_decl->body && dynamic_cast<FloatValofExpression*>(func_decl->body.get())) {
                        function_return_types_[mangled_name] = VarType::FLOAT;
                        local_float_function_names_.insert(mangled_name);
                    } else {
                        function_return_types_[mangled_name] = VarType::INTEGER;
                    }
                } else if (auto* routine_decl = dynamic_cast<RoutineDeclaration*>(member_decl.get())) {
                    std::string mangled_name = NameMangler::mangleMethod(class_name, routine_decl->name);
                    local_routine_names_.insert(mangled_name);
                    // Preserve existing parameter types if they exist
                    if (function_metrics_.find(mangled_name) == function_metrics_.end()) {
                        function_metrics_[mangled_name] = FunctionMetrics();
                    }

                    function_return_types_[mangled_name] = VarType::INTEGER;
                }
            }
        // --- END NEW ---
        } else if (auto* func_decl = dynamic_cast<FunctionDeclaration*>(decl.get())) {
            // --- FIX: Use a local copy of the function name ---
            std::string func_name = func_decl->name;
            local_function_names_.insert(func_name);
            // Preserve existing parameter types if they exist
            if (function_metrics_.find(func_name) == function_metrics_.end()) {
                function_metrics_[func_name] = FunctionMetrics();
            }

            // Check both the is_float_function flag (for FLET) and FloatValofExpression body
            if (trace_enabled_) {
                std::cout << "[ANALYZER] Checking " << func_name << ": is_float_function=" << func_decl->is_float_function 
                         << ", has_FloatValofExpression=" << (func_decl->body && dynamic_cast<FloatValofExpression*>(func_decl->body.get()) != nullptr) << std::endl;
            }
            
            if (func_decl->is_float_function || 
                (func_decl->body && dynamic_cast<FloatValofExpression*>(func_decl->body.get()))) {
                function_return_types_[func_name] = VarType::FLOAT;
                local_float_function_names_.insert(func_name);
                if (trace_enabled_) {
                    std::cout << "[ANALYZER] Set " << func_name << " return type to FLOAT" << std::endl;
                }
            } else {
                function_return_types_[func_name] = VarType::INTEGER;
                if (trace_enabled_) {
                    std::cout << "[ANALYZER] Set " << func_name << " return type to INTEGER" << std::endl;
                }
            }
        } else if (auto* routine_decl = dynamic_cast<RoutineDeclaration*>(decl.get())) {
            // --- FIX: Use a local copy of the routine name ---
            std::string routine_name = routine_decl->name;
            local_routine_names_.insert(routine_name);
            // Preserve existing parameter types if they exist
            if (function_metrics_.find(routine_name) == function_metrics_.end()) {
                function_metrics_[routine_name] = FunctionMetrics();
            }

            function_return_types_[routine_name] = VarType::INTEGER;
        } else if (auto* let_decl = dynamic_cast<LetDeclaration*>(decl.get())) {
            if (let_decl->names.size() == 1 && let_decl->initializers.size() == 1) {
                // --- FIX: Use a local copy of the let name ---
                std::string let_name = let_decl->names[0];
                if (dynamic_cast<ValofExpression*>(let_decl->initializers[0].get())) {
                    local_function_names_.insert(let_name);
                    // Preserve existing parameter types if they exist
                    if (function_metrics_.find(let_name) == function_metrics_.end()) {
                        function_metrics_[let_name] = FunctionMetrics();
                    }

                    function_return_types_[let_name] = VarType::INTEGER;
                } else if (dynamic_cast<FloatValofExpression*>(let_decl->initializers[0].get())) {
                    local_float_function_names_.insert(let_name);
                    // Preserve existing parameter types if they exist
                    if (function_metrics_.find(let_name) == function_metrics_.end()) {
                        function_metrics_[let_name] = FunctionMetrics();
                    }

                    function_return_types_[let_name] = VarType::FLOAT;
                }
            }
        }
    }
}
