#include "ASTAnalyzer.h"

// Visitor implementation for UnaryOp nodes
void ASTAnalyzer::visit(UnaryOp& node) {
    if (node.operand) {
        node.operand->accept(*this);
        
        // Type check for list operations
        if (node.op == UnaryOp::Operator::HeadOf || node.op == UnaryOp::Operator::TailOf || node.op == UnaryOp::Operator::TailOfNonDestructive) {
            // Check if the operand is a variable access
            if (auto var_access = dynamic_cast<VariableAccess*>(node.operand.get())) {
                std::string var_name = var_access->name;
                
                // Look up the variable in symbol table
                Symbol symbol;
                if (symbol_table_->lookup(var_name, symbol)) {
                    VarType var_type = symbol.type;
                    
                    // Check if it's a list type using bitfield logic
                    int64_t type_bits = static_cast<int64_t>(var_type);
                    bool has_pointer_flag = (type_bits & static_cast<int64_t>(VarType::POINTER_TO)) != 0;
                    bool has_list_flag = (type_bits & static_cast<int64_t>(VarType::LIST)) != 0;
                    bool is_list_type = has_pointer_flag && has_list_flag;
                    
                    std::cerr << "[DEBUG] Variable '" << var_name << "' type_bits=" << type_bits 
                              << ", POINTER_TO=" << static_cast<int64_t>(VarType::POINTER_TO)
                              << ", LIST=" << static_cast<int64_t>(VarType::LIST)
                              << ", POINTER_TO_STRING_LIST=" << static_cast<int64_t>(VarType::POINTER_TO_STRING_LIST)
                              << ", has_pointer=" << has_pointer_flag 
                              << ", has_list=" << has_list_flag 
                              << ", is_list_type=" << is_list_type << std::endl;
                    
                    if (!is_list_type) {
                        std::string op_name;
                        switch (node.op) {
                            case UnaryOp::Operator::HeadOf: op_name = "HD"; break;
                            case UnaryOp::Operator::TailOf: op_name = "TL"; break;
                            case UnaryOp::Operator::TailOfNonDestructive: op_name = "REST"; break;
                            default: op_name = "unknown"; break;
                        }
                        
                        std::string error_msg = "Type error: " + op_name + " operation can only be applied to list types, but '" + 
                                              var_name + "' has type " + vartype_to_string(var_type);
                        std::cerr << "[SEMANTIC ERROR] " << error_msg << std::endl;
                        semantic_errors_.push_back(error_msg);
                    }
                }
            }
        }
    }
}
