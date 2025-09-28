#pragma once

#include "../AST.h"
#include "../DataTypes.h"
#include "../SymbolTable.h"
#include <string>

/**
 * @class StatelessTypeInference
 * @brief A stateless utility for type inference that doesn't modify AST nodes or maintain state.
 * 
 * This class provides type inference capabilities without the state management issues
 * that can occur with the ASTAnalyzer singleton. It should be used by optimization
 * passes and other components that need type information but shouldn't modify the AST.
 */
class StatelessTypeInference {
public:
    /**
     * @brief Infers the type of an expression without modifying the AST.
     * @param expr The expression to analyze
     * @param symbol_table Optional symbol table for variable type lookup
     * @return The inferred VarType
     */
    static VarType infer_expression_type(const Expression* expr, const SymbolTable* symbol_table = nullptr);

    /**
     * @brief Infers the type of a variable without modifying the AST.
     * @param var_name The variable name
     * @param function_scope The current function scope
     * @param symbol_table Symbol table for lookup
     * @return The inferred VarType
     */
    static VarType infer_variable_type(const std::string& var_name, 
                                     const std::string& function_scope,
                                     const SymbolTable* symbol_table);

    /**
     * @brief Checks if an expression is a constant without modifying the AST.
     * @param expr The expression to check
     * @param value Output parameter for the constant value if found
     * @return True if the expression is a constant
     */
    static bool is_constant_expression(const Expression* expr, int64_t* value = nullptr);

    /**
     * @brief Checks if a binary operation results in a float type.
     * @param left_type Type of the left operand
     * @param right_type Type of the right operand
     * @param op The binary operator
     * @return True if the operation results in a float
     */
    static bool is_float_operation(VarType left_type, VarType right_type, BinaryOp::Operator op);

private:
    // Helper methods for type inference
    static VarType infer_binary_op_type(const BinaryOp* bin_op, const SymbolTable* symbol_table);
    static VarType infer_unary_op_type(const UnaryOp* unary_op, const SymbolTable* symbol_table);
    static VarType infer_function_call_type(const FunctionCall* func_call, const SymbolTable* symbol_table);
    static VarType infer_variable_access_type(const VariableAccess* var_access, const SymbolTable* symbol_table);
    static VarType infer_literal_type(const Expression* expr);
    
    // Utility methods
    static bool is_arithmetic_operator(BinaryOp::Operator op);
    static bool is_comparison_operator(BinaryOp::Operator op);
    static bool is_logical_operator(BinaryOp::Operator op);
};