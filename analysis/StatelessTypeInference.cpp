#include "StatelessTypeInference.h"
#include "../SymbolTable.h"
#include <iostream>

VarType StatelessTypeInference::infer_expression_type(const Expression* expr, const SymbolTable* symbol_table) {
    if (!expr) return VarType::UNKNOWN;

    // Number literals
    if (const auto* num_lit = dynamic_cast<const NumberLiteral*>(expr)) {
        return infer_literal_type(expr);
    }

    // String literals
    if (const auto* str_lit = dynamic_cast<const StringLiteral*>(expr)) {
        return VarType::STRING;
    }

    // Character literals
    if (const auto* char_lit = dynamic_cast<const CharLiteral*>(expr)) {
        return VarType::INTEGER;
    }

    // Boolean literals
    if (const auto* bool_lit = dynamic_cast<const BooleanLiteral*>(expr)) {
        return VarType::INTEGER;
    }

    // Null literals
    if (const auto* null_lit = dynamic_cast<const NullLiteral*>(expr)) {
        return VarType::POINTER_TO_INT;
    }

    // Variable access
    if (const auto* var_access = dynamic_cast<const VariableAccess*>(expr)) {
        return infer_variable_access_type(var_access, symbol_table);
    }

    // Binary operations
    if (const auto* bin_op = dynamic_cast<const BinaryOp*>(expr)) {
        return infer_binary_op_type(bin_op, symbol_table);
    }

    // Unary operations
    if (const auto* unary_op = dynamic_cast<const UnaryOp*>(expr)) {
        return infer_unary_op_type(unary_op, symbol_table);
    }

    // Function calls
    if (const auto* func_call = dynamic_cast<const FunctionCall*>(expr)) {
        return infer_function_call_type(func_call, symbol_table);
    }

    // Vector access
    if (const auto* vec_access = dynamic_cast<const VectorAccess*>(expr)) {
        // Infer element type based on the vector type
        VarType vector_type = infer_expression_type(vec_access->vector_expr.get(), symbol_table);
        
        // If accessing a PAIRS vector, return PAIR
        if ((static_cast<int64_t>(vector_type) & static_cast<int64_t>(VarType::PAIRS)) != 0) {
            return VarType::PAIR;
        }
        // If accessing a FPAIRS vector, return FPAIR
        if ((static_cast<int64_t>(vector_type) & static_cast<int64_t>(VarType::FPAIRS)) != 0) {
            return VarType::FPAIR;
        }
        // If accessing a float vector, return float
        if ((static_cast<int64_t>(vector_type) & static_cast<int64_t>(VarType::FLOAT)) != 0) {
            return VarType::FLOAT;
        }
        // Default to integer for regular vectors
        return VarType::INTEGER;
    }

    // Lane access (vector.|n|)
    if (const auto* lane_access = dynamic_cast<const LaneAccessExpression*>(expr)) {
        return VarType::INTEGER; // Lane access always returns INTEGER
    }

    // Vector allocation
    if (const auto* vec_alloc = dynamic_cast<const VecAllocationExpression*>(expr)) {
        return VarType::POINTER_TO_INT_VEC;
    }

    // Float vector allocation
    if (const auto* fvec_alloc = dynamic_cast<const FVecAllocationExpression*>(expr)) {
        return VarType::POINTER_TO_FLOAT_VEC;
    }

    // PAIRS allocation
    if (const auto* pairs_alloc = dynamic_cast<const PairsAllocationExpression*>(expr)) {
        return VarType::POINTER_TO_PAIRS;
    }

    // FPAIRS allocation
    if (const auto* fpairs_alloc = dynamic_cast<const FPairsAllocationExpression*>(expr)) {
        return VarType::POINTER_TO_FPAIRS;
    }

    // String allocation
    if (const auto* str_alloc = dynamic_cast<const StringAllocationExpression*>(expr)) {
        return VarType::STRING;
    }

    // Member access
    if (const auto* member_access = dynamic_cast<const MemberAccessExpression*>(expr)) {
        return VarType::UNKNOWN; // Would need class table to determine
    }

    // Default to unknown
    return VarType::UNKNOWN;
}

VarType StatelessTypeInference::infer_variable_type(const std::string& var_name, 
                                                  const std::string& function_scope,
                                                  const SymbolTable* symbol_table) {
    if (!symbol_table) return VarType::UNKNOWN;

    Symbol symbol;
    if (symbol_table->lookup(var_name, symbol)) {
        return symbol.type;
    }

    return VarType::UNKNOWN;
}

bool StatelessTypeInference::is_constant_expression(const Expression* expr, int64_t* value) {
    if (!expr) return false;

    if (const auto* num_lit = dynamic_cast<const NumberLiteral*>(expr)) {
        if (value) *value = num_lit->int_value;
        return true;
    }

    if (const auto* bool_lit = dynamic_cast<const BooleanLiteral*>(expr)) {
        if (value) *value = bool_lit->value ? 1 : 0;
        return true;
    }

    if (const auto* char_lit = dynamic_cast<const CharLiteral*>(expr)) {
        if (value) *value = static_cast<int64_t>(char_lit->value);
        return true;
    }

    // For binary operations with constant operands
    if (const auto* bin_op = dynamic_cast<const BinaryOp*>(expr)) {
        int64_t left_val, right_val;
        if (is_constant_expression(bin_op->left.get(), &left_val) &&
            is_constant_expression(bin_op->right.get(), &right_val)) {
            
            if (value) {
                switch (bin_op->op) {
                    case BinaryOp::Operator::Add:
                        *value = left_val + right_val;
                        break;
                    case BinaryOp::Operator::Subtract:
                        *value = left_val - right_val;
                        break;
                    case BinaryOp::Operator::Multiply:
                        *value = left_val * right_val;
                        break;
                    case BinaryOp::Operator::Divide:
                        *value = right_val != 0 ? left_val / right_val : 0;
                        break;
                    case BinaryOp::Operator::Remainder:
                        *value = right_val != 0 ? left_val % right_val : 0;
                        break;
                    default:
                        return false;
                }
            }
            return true;
        }
    }

    return false;
}

bool StatelessTypeInference::is_float_operation(VarType left_type, VarType right_type, BinaryOp::Operator op) {
    return (left_type == VarType::FLOAT || right_type == VarType::FLOAT) &&
           is_arithmetic_operator(op);
}

VarType StatelessTypeInference::infer_binary_op_type(const BinaryOp* bin_op, const SymbolTable* symbol_table) {
    if (!bin_op) return VarType::UNKNOWN;

    VarType left_type = infer_expression_type(bin_op->left.get(), symbol_table);
    VarType right_type = infer_expression_type(bin_op->right.get(), symbol_table);

    // Comparison operations always return integers (boolean)
    if (is_comparison_operator(bin_op->op)) {
        return VarType::INTEGER;
    }

    // Logical operations always return integers (boolean)
    if (is_logical_operator(bin_op->op)) {
        return VarType::INTEGER;
    }

    // Arithmetic operations
    if (is_arithmetic_operator(bin_op->op)) {
        // If either operand is float, result is float
        if (left_type == VarType::FLOAT || right_type == VarType::FLOAT) {
            return VarType::FLOAT;
        }
        // Otherwise, result is integer
        return VarType::INTEGER;
    }

    // Default to integer for unknown operations
    return VarType::INTEGER;
}

VarType StatelessTypeInference::infer_unary_op_type(const UnaryOp* unary_op, const SymbolTable* symbol_table) {
    if (!unary_op) return VarType::UNKNOWN;

    VarType operand_type = infer_expression_type(unary_op->operand.get(), symbol_table);

    switch (unary_op->op) {
        case UnaryOp::Operator::Negate:
            return operand_type; // Preserve the operand type
        case UnaryOp::Operator::LogicalNot:
            return VarType::INTEGER; // Logical not always returns boolean (integer)
        case UnaryOp::Operator::BitwiseNot:
            return VarType::INTEGER; // Bitwise not always returns integer
        case UnaryOp::Operator::AddressOf:
            // Address-of creates a pointer to the operand type
            if (operand_type == VarType::INTEGER) return VarType::POINTER_TO_INT;
            if (operand_type == VarType::FLOAT) return VarType::POINTER_TO_FLOAT;
            return VarType::POINTER_TO_INT;
        case UnaryOp::Operator::Indirection:
            // Indirection removes one level of pointer indirection
            if (operand_type == VarType::POINTER_TO_INT) return VarType::INTEGER;
            if (operand_type == VarType::POINTER_TO_FLOAT) return VarType::FLOAT;
            return VarType::INTEGER; // Default assumption
        default:
            return VarType::UNKNOWN;
    }
}

VarType StatelessTypeInference::infer_function_call_type(const FunctionCall* func_call, const SymbolTable* symbol_table) {
    if (!func_call) return VarType::UNKNOWN;

    // Check if it's a variable access (function name)
    if (const auto* var_access = dynamic_cast<const VariableAccess*>(func_call->function_expr.get())) {
        const std::string& func_name = var_access->name;

        // First check symbol table for runtime functions if available
        if (symbol_table) {
            Symbol symbol;
            if (symbol_table->lookup(func_name, symbol)) {
                if (symbol.is_runtime_function()) {
                    return symbol.type;
                }
            }
        }

        // Fallback to hardcoded runtime functions for compatibility
        if (func_name == "READN" || func_name == "LENGTH" || func_name == "STRCMP") {
            return VarType::INTEGER;
        }
        if (func_name == "READF" || func_name == "SIN" || func_name == "COS" || func_name == "SQRT") {
            return VarType::FLOAT;
        }
        if (func_name == "READS" || func_name == "NEWVEC") {
            return VarType::POINTER_TO_INT;
        }
        if (func_name == "GETVEC") {
            return VarType::POINTER_TO_INT_VEC;
        }
        if (func_name == "FGETVEC") {
            return VarType::POINTER_TO_FLOAT_VEC;
        }
        if (func_name == "SPLIT") {
            return VarType::POINTER_TO_STRING_LIST;
        }

        // For user-defined functions, we'd need function return type information
        // which would typically come from the ASTAnalyzer's function_return_types_
        // For now, default to INTEGER
        return VarType::INTEGER;
    }

    return VarType::UNKNOWN;
}

VarType StatelessTypeInference::infer_variable_access_type(const VariableAccess* var_access, const SymbolTable* symbol_table) {
    if (!var_access || var_access->name.empty()) return VarType::UNKNOWN;

    // Use symbol table lookup if available
    if (symbol_table) {
        Symbol symbol;
        if (symbol_table->lookup(var_access->name, symbol)) {
            return symbol.type;
        }
    }

    // Default assumption for unknown variables
    return VarType::INTEGER;
}

VarType StatelessTypeInference::infer_literal_type(const Expression* expr) {
    if (const auto* num_lit = dynamic_cast<const NumberLiteral*>(expr)) {
        switch (num_lit->literal_type) {
            case NumberLiteral::LiteralType::Integer:
                return VarType::INTEGER;
            case NumberLiteral::LiteralType::Float:
                return VarType::FLOAT;
            default:
                return VarType::INTEGER;
        }
    }
    return VarType::UNKNOWN;
}

bool StatelessTypeInference::is_arithmetic_operator(BinaryOp::Operator op) {
    switch (op) {
        case BinaryOp::Operator::Add:
        case BinaryOp::Operator::Subtract:
        case BinaryOp::Operator::Multiply:
        case BinaryOp::Operator::Divide:
        case BinaryOp::Operator::Remainder:
            return true;
        default:
            return false;
    }
}

bool StatelessTypeInference::is_comparison_operator(BinaryOp::Operator op) {
    switch (op) {
        case BinaryOp::Operator::Equal:
        case BinaryOp::Operator::NotEqual:
        case BinaryOp::Operator::Less:
        case BinaryOp::Operator::LessEqual:
        case BinaryOp::Operator::Greater:
        case BinaryOp::Operator::GreaterEqual:
            return true;
        default:
            return false;
    }
}

bool StatelessTypeInference::is_logical_operator(BinaryOp::Operator op) {
    switch (op) {
        case BinaryOp::Operator::LogicalAnd:
        case BinaryOp::Operator::LogicalOr:
        case BinaryOp::Operator::BitwiseAnd:
        case BinaryOp::Operator::BitwiseOr:
        case BinaryOp::Operator::LeftShift:
        case BinaryOp::Operator::RightShift:
            return true;
        default:
            return false;
    }
}