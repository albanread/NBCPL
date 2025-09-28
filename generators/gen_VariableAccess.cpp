#include "NewCodeGenerator.h"
#include "LabelManager.h"
#include "analysis/ASTAnalyzer.h"
#include "RuntimeManager.h"
#include <iostream>
#include <stdexcept>

void NewCodeGenerator::visit(VariableAccess& node) {
    debug_print("Visiting VariableAccess node for '" + node.name + "'.");
    debug_print("  [DEBUG] About to call get_variable_register for '" + node.name + "' in function '" + current_function_name_ + "'");

    // --- CSE DEBUG: Check if this is a CSE temp variable ---
    if (node.name.find("_cse_temp_") == 0) {
        debug_print("  [CSE DEBUG] *** PROCESSING CSE TEMP VARIABLE: " + node.name + " ***");
    }

    // The get_variable_register method now handles local value tracking internally
    // It will automatically detect known addresses and generate optimal code
    expression_result_reg_ = get_variable_register(node.name);

    debug_print("Variable '" + node.name + "' value loaded into " + expression_result_reg_);
    debug_print("  [DEBUG] get_variable_register returned: '" + expression_result_reg_ + "'");
    
    if (expression_result_reg_.empty()) {
        debug_print("  [ERROR] get_variable_register returned empty string for variable '" + node.name + "'!");
        throw std::runtime_error("get_variable_register returned empty string for variable '" + node.name + "' in function '" + current_function_name_ + "'");
    }
}
