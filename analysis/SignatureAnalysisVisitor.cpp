#include "SignatureAnalysisVisitor.h"
#include "../NameMangler.h"
#include <iostream>

SignatureAnalysisVisitor::SignatureAnalysisVisitor(SymbolTable* symbol_table, ASTAnalyzer& analyzer, bool debug)
    : symbol_table_(symbol_table), analyzer_(analyzer), debug_enabled_(debug), current_class_name_("") {
}

void SignatureAnalysisVisitor::analyze_signatures(Program& program) {
    debug_print("Starting signature analysis pass...");
    visit(program);
    
    // Mark signature analysis as complete to prevent ASTAnalyzer from resetting
    analyzer_.set_signature_analysis_complete(true);
    debug_print("Signature analysis complete.");
}

void SignatureAnalysisVisitor::visit(Program& node) {
    debug_print("Analyzing program signatures...");
    
    // Only process declarations that contain function/routine signatures
    for (auto& decl : node.declarations) {
        if (decl) {
            // Only visit function and routine declarations
            if (auto* func_decl = dynamic_cast<FunctionDeclaration*>(decl.get())) {
                decl->accept(*this);
            } else if (auto* routine_decl = dynamic_cast<RoutineDeclaration*>(decl.get())) {
                decl->accept(*this);
            }
            // Skip all other declaration types in signature analysis
        }
    }
}

void SignatureAnalysisVisitor::visit(FunctionDeclaration& node) {
    debug_print("Analyzing function signature: " + node.name);
    
    // Handle class method name mangling
    std::string mangled_name = node.name;
    if (!current_class_name_.empty()) {
        mangled_name = NameMangler::mangleMethod(current_class_name_, node.name);
        
        // Add implicit _this parameter for methods if not already present
        if (std::find(node.parameters.begin(), node.parameters.end(), "_this") == node.parameters.end()) {
            node.parameters.insert(node.parameters.begin(), "_this");
            debug_print("Added _this parameter to method: " + mangled_name);
        }
    }
    
    analyze_function_signature(mangled_name, node.parameters, node.body.get());
}

void SignatureAnalysisVisitor::visit(RoutineDeclaration& node) {
    debug_print("Analyzing routine signature: " + node.name);
    
    // Handle class method name mangling
    std::string mangled_name = node.name;
    if (!current_class_name_.empty()) {
        mangled_name = NameMangler::mangleMethod(current_class_name_, node.name);
        
        // Add implicit _this parameter for methods if not already present
        if (std::find(node.parameters.begin(), node.parameters.end(), "_this") == node.parameters.end()) {
            node.parameters.insert(node.parameters.begin(), "_this");
            debug_print("Added _this parameter to method: " + mangled_name);
        }
    }
    
    analyze_routine_signature(mangled_name, node.parameters, node.body.get());
}

void SignatureAnalysisVisitor::analyze_function_signature(const std::string& function_name, 
                                                        const std::vector<std::string>& parameters, 
                                                        ASTNode* body) {
    debug_print("Processing function signature: " + function_name);
    
    // Get or create function metrics
    auto& metrics = analyzer_.get_function_metrics_mut()[function_name];
    metrics.num_parameters = parameters.size();
    
    // Analyze each parameter
    for (size_t i = 0; i < parameters.size(); ++i) {
        const auto& param = parameters[i];
        VarType param_type;
        
        // Special handling for _this parameter in class methods
        if (param == "_this" && !current_class_name_.empty()) {
            param_type = VarType::POINTER_TO_OBJECT;
            debug_print("Set _this parameter type to POINTER_TO_OBJECT");
        } else {
            // For regular parameters, try to infer type from usage in the function body
            param_type = infer_parameter_type_from_usage(param, function_name, body);
            
            // If inference fails, look up in symbol table
            if (param_type == VarType::UNKNOWN && symbol_table_) {
                Symbol param_symbol;
                if (symbol_table_->lookup(param, function_name, param_symbol)) {
                    param_type = param_symbol.type;
                    debug_print("Got parameter type from symbol table: " + param + " -> " + std::to_string(static_cast<int>(param_type)));
                }
            }
            
            // Final fallback to INTEGER
            if (param_type == VarType::UNKNOWN) {
                param_type = VarType::INTEGER;
                debug_print("Defaulting parameter to INTEGER: " + param);
            }
        }
        
        // Register parameter type in function metrics
        metrics.parameter_types[param] = param_type;
        metrics.variable_types[param] = param_type;
        metrics.parameter_indices[param] = i;
        
        // Update symbol table with the determined type
        if (symbol_table_) {
            symbol_table_->updateSymbolType(param, param_type);
        }
        
        debug_print("Registered parameter: " + param + " (type: " + std::to_string(static_cast<int>(param_type)) + ")");
    }
}

void SignatureAnalysisVisitor::analyze_routine_signature(const std::string& routine_name, 
                                                       const std::vector<std::string>& parameters, 
                                                       ASTNode* body) {
    debug_print("Processing routine signature: " + routine_name);
    
    // Get or create function metrics (routines use the same metrics structure)
    auto& metrics = analyzer_.get_function_metrics_mut()[routine_name];
    metrics.num_parameters = parameters.size();
    
    // Analyze each parameter
    for (size_t i = 0; i < parameters.size(); ++i) {
        const auto& param = parameters[i];
        VarType param_type;
        
        // Special handling for _this parameter in class methods
        if (param == "_this" && !current_class_name_.empty()) {
            param_type = VarType::POINTER_TO_OBJECT;
            debug_print("Set _this parameter type to POINTER_TO_OBJECT");
        } else {
            // For regular parameters, try to infer type from usage in the routine body
            param_type = infer_parameter_type_from_usage(param, routine_name, body);
            
            // If inference fails, look up in symbol table
            if (param_type == VarType::UNKNOWN && symbol_table_) {
                Symbol param_symbol;
                if (symbol_table_->lookup(param, routine_name, param_symbol)) {
                    param_type = param_symbol.type;
                    debug_print("Got parameter type from symbol table: " + param + " -> " + std::to_string(static_cast<int>(param_type)));
                }
            }
            
            // Final fallback to INTEGER
            if (param_type == VarType::UNKNOWN) {
                param_type = VarType::INTEGER;
                debug_print("Defaulting parameter to INTEGER: " + param);
            }
        }
        
        // Register parameter type in function metrics
        metrics.parameter_types[param] = param_type;
        metrics.variable_types[param] = param_type;
        metrics.parameter_indices[param] = i;
        
        // Update symbol table with the determined type
        if (symbol_table_) {
            symbol_table_->updateSymbolType(param, param_type);
        }
        
        debug_print("Registered parameter: " + param + " (type: " + std::to_string(static_cast<int>(param_type)) + ")");
    }
}

VarType SignatureAnalysisVisitor::infer_parameter_type_from_usage(const std::string& param_name, 
                                                                const std::string& function_name, 
                                                                ASTNode* body) {
    // For now, we'll use a simple heuristic: default to INTEGER
    // In a more sophisticated implementation, we could do limited AST traversal
    // to look for type hints (e.g., parameter used in float operations)
    
    // TODO: Implement more sophisticated type inference based on usage patterns
    // This could include:
    // - Looking for arithmetic operations that suggest numeric types
    // - Looking for function calls that expect specific parameter types
    // - Looking for assignments to typed variables
    
    debug_print("Simple type inference for parameter: " + param_name + " -> INTEGER (default)");
    return VarType::INTEGER;
}

void SignatureAnalysisVisitor::debug_print(const std::string& message) const {
    if (debug_enabled_) {
        std::cout << "[SignatureAnalysis] " << message << std::endl;
    }
}