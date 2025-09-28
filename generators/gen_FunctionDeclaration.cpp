#include "../NewCodeGenerator.h"

#include "../analysis/ASTAnalyzer.h" // Needed for analyzer_



// IMPORTANT: This file should ONLY contain the implementation of
// NewCodeGenerator::visit(FunctionDeclaration& node)
// The implementation of generate_function_like_code will go into NewCodeGenerator.cpp
// This file will now delegate to that common helper.

void NewCodeGenerator::visit(FunctionDeclaration& node) {
    debug_print("Visiting FunctionDeclaration node (Name: " + node.name + ")."); //
    auto metrics_it = ASTAnalyzer::getInstance().get_function_metrics().find(node.name);
    if (metrics_it != ASTAnalyzer::getInstance().get_function_metrics().end()) {
        const auto& metrics = metrics_it->second;
        debug_print("Function metrics for " + node.name + ": runtime_calls=" + std::to_string(metrics.num_runtime_calls) + //
                    ", local_function_calls=" + std::to_string(metrics.num_local_function_calls) + //
                    ", local_routine_calls=" + std::to_string(metrics.num_local_routine_calls)); //
    } else {
        debug_print("Function metrics for " + node.name + " not found.");
    }

    // Delegate to the common helper method for all function-like code generation.
    // The body of a FunctionDeclaration is an Expression.
    if (node.body) {
        // Check if we're in a class context to use the qualified name
        std::string function_name = node.name;
        // Only qualify if we're in a class context AND the name isn't already qualified
        if (!current_class_name_.empty() && node.name.find("::") == std::string::npos) {
            function_name = current_class_name_ + "::" + node.name;
            debug_print("Using qualified name for method: " + function_name);
        }
        generate_function_like_code(function_name, node.parameters, *node.body, true); // The 'true' indicates it's a value-returning function.
    } else {
        throw std::runtime_error("FunctionDeclaration: Body is null for function " + node.name);
    }

    debug_print("Finished visiting FunctionDeclaration node."); //
}
