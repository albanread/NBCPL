#include "../NewCodeGenerator.h"
#include "../LabelManager.h"
#include "../analysis/ASTAnalyzer.h" // Needed for analyzer_
#include <iostream>
#include <stdexcept>

// IMPORTANT: This file should ONLY contain the implementation of
// NewCodeGenerator::visit(RoutineDeclaration& node)
// The implementation of generate_function_like_code will go into NewCodeGenerator.cpp
// This file will now delegate to that common helper.

void NewCodeGenerator::visit(RoutineDeclaration& node) {
    debug_print("DEBUG: Visiting RoutineDeclaration node (Name: " + node.name + ").");


    // Delegate to the common helper method for all function-like code generation.
    // The body of a RoutineDeclaration is a Statement.
    
    // Check if we're in a class context to use the qualified name
    std::string function_name = node.name;
    // Only qualify if we're in a class context AND the name isn't already qualified
    if (!current_class_name_.empty() && node.name.find("::") == std::string::npos) {
        function_name = current_class_name_ + "::" + node.name;
        debug_print("Using qualified name for method: " + function_name);
    }
    generate_function_like_code(function_name, node.parameters, *node.body, false); // The 'false' indicates it's a non-value-returning routine.

    debug_print("Finished visiting RoutineDeclaration node."); //
}
