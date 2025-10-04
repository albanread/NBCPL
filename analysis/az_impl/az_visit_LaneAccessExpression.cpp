#include "../ASTAnalyzer.h"
#include <iostream>

void ASTAnalyzer::visit(LaneAccessExpression& node) {
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] Visiting LaneAccessExpression" << std::endl;
    }
    
    // Track that we're processing a lane access expression
    if (!current_function_scope_.empty()) {
        auto& metrics = function_metrics_[current_function_scope_];
        // Lane access operations are relatively lightweight
        metrics.instruction_count += 2; // Approximate instructions for lane extraction/insertion
    }
    
    // Visit the vector expression
    if (node.vector_expr) {
        node.vector_expr->accept(*this);
    }
    
    // Get the type of the vector expression to validate lane index
    VarType vector_type = get_expression_type(*node.vector_expr);
    
    // Validate that the vector expression is actually a vector type
    if (vector_type != VarType::PAIR && vector_type != VarType::FPAIR && 
        vector_type != VarType::QUAD && vector_type != VarType::OCT && 
        vector_type != VarType::FOCT) {
        std::string error = "Lane access operator .|n| can only be used on vector types (PAIR, FPAIR, QUAD, OCT, FOCT)";
        semantic_errors_.push_back(error);
        return;
    }
    
    // Determine the maximum valid lane index for this vector type
    int max_lane_index = 0;
    switch (vector_type) {
        case VarType::PAIR:
        case VarType::FPAIR:
            max_lane_index = 1; // 0, 1
            break;
        case VarType::QUAD:
            max_lane_index = 3; // 0, 1, 2, 3
            break;
        case VarType::OCT:
        case VarType::FOCT:
            max_lane_index = 7; // 0, 1, 2, 3, 4, 5, 6, 7
            break;
        default:
            break;
    }
    
    // Validate lane index is within bounds
    if (node.index < 0 || node.index > max_lane_index) {
        std::string vector_type_name;
        switch (vector_type) {
            case VarType::PAIR: vector_type_name = "PAIR"; break;
            case VarType::FPAIR: vector_type_name = "FPAIR"; break;
            case VarType::QUAD: vector_type_name = "QUAD"; break;
            case VarType::OCT: vector_type_name = "OCT"; break;
            case VarType::FOCT: vector_type_name = "FOCT"; break;
            default: vector_type_name = "UNKNOWN"; break;
        }
        
        std::string error = "Lane index " + std::to_string(node.index) + 
                          " is out of bounds for " + vector_type_name + 
                          " (valid range: 0-" + std::to_string(max_lane_index) + ")";
        semantic_errors_.push_back(error);
    }
    
    // Set the expression type based on the vector's element type
    VarType element_type = VarType::INTEGER;
    if (vector_type == VarType::FPAIR || vector_type == VarType::FOCT) {
        element_type = VarType::FLOAT;
    }
    
    // Store the inferred type for this lane access expression
    // expression_types_[&node] = element_type;  // TODO: Implement expression type tracking if needed
    
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] LaneAccessExpression analysis complete - element type: " 
                  << vartype_to_string(element_type) << std::endl;
    }
}