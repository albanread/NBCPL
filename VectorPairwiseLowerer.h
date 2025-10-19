#pragma once

#include "AST.h"
#include "ASTVisitor.h"
#include "DataTypes.h"
#include "SymbolTable.h"
#include "analysis/ASTAnalyzer.h"
#include <memory>
#include <vector>
#include <string>

/**
 * @class VectorPairwiseLowerer
 * @brief AST transformation pass that converts vector operations into explicit loops
 * 
 * This pass runs after ASTAnalyzer and transforms high-level vector operations
 * like `vec1 + vec2` (where both are PAIRS[]) into explicit ForLoop structures
 * that use element-wise PAIR + PAIR operations.
 * 
 * The lowering enables reuse of existing NEON SIMD codegen for PAIR operations
 * without requiring new vector instruction encoders.
 * 
 * Example transformation:
 *   Before: LET z = x + y  // where x, y are PAIRS[]
 *   After:  LET __new_vec = PAIRS(LEN(x))
 *           FOR __i = 0 TO LEN(x) - 1 DO {
 *               __new_vec[__i] = x[__i] + y[__i]  // Uses PAIR + PAIR SIMD
 *           }
 *           LET z = __new_vec
 * 
 * Supported vector types:
 * - PAIRS (vectors of PAIR elements)
 * - FPAIRS (vectors of FPAIR elements) 
 * - QUADS (vectors of QUAD elements)
 * - FQUADS (vectors of FQUAD elements)
 * 
 * Supported operations:
 * - Add, Subtract, Multiply, Divide
 */
class VectorPairwiseLowerer : public ASTVisitor {
public:
    /**
     * Constructor
     * @param symbol_table Pointer to the symbol table for registering temporary variables
     * @param analyzer Pointer to the ASTAnalyzer for analyzing new nodes
     * @param enable_debug Enable debug output for transformation process
     */
    explicit VectorPairwiseLowerer(SymbolTable* symbol_table, ASTAnalyzer* analyzer, bool enable_debug = false);
    
    /**
     * Main entry point - lower vector operations in the program
     * @param program The AST program to transform
     * @return true if any transformations were made, false otherwise
     */
    bool lower(Program& program);

    // ASTVisitor interface - we only need to override nodes that can contain BinaryOp
    void visit(Program& node) override;
    void visit(FunctionDeclaration& node) override;
    void visit(RoutineDeclaration& node) override;
    void visit(BlockStatement& node) override;
    void visit(AssignmentStatement& node) override;
    void visit(IfStatement& node) override;
    void visit(UnlessStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(UntilStatement& node) override;
    void visit(RepeatStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(ValofExpression& node) override;
    void visit(FloatValofExpression& node) override;
    void visit(ConditionalExpression& node) override;

private:
    SymbolTable* symbol_table_;
    ASTAnalyzer* analyzer_;
    std::string current_function_name_;
    bool debug_enabled_;
    bool transformations_made_;
    int temp_var_counter_;
    std::string last_result_var_name_;
    
    /**
     * Check if a BinaryOp represents a vector operation that needs lowering
     * @param binary_op The binary operation to check
     * @return true if this is a vector operation, false otherwise
     */
    bool isVectorOperation(const BinaryOp* binary_op) const;
    
    /**
     * Get the element type for a vector type
     * @param vector_type The vector type (PAIRS, FPAIRS, etc.)
     * @return The element type (PAIR, FPAIR, etc.)
     */
    VarType getElementType(VarType vector_type) const;
    
    /**
     * Get the allocation expression type for a vector type
     * @param vector_type The vector type (PAIRS, FPAIRS, etc.)
     * @return The corresponding allocation expression type
     */
    std::string getAllocationExpressionType(VarType vector_type) const;
    
    /**
     * Transform a vector BinaryOp into a loop structure
     * @param binary_op The vector binary operation to transform
     * @return A BlockStatement containing the lowered loop code
     */
    std::unique_ptr<BlockStatement> lowerVectorOperation(const BinaryOp* binary_op);
    
    /**
     * Create a unique temporary variable name
     * @param prefix The prefix for the variable name
     * @return A unique variable name
     */
    std::string createTempVarName(const std::string& prefix);
    
    /**
     * Transform expressions recursively, looking for vector operations
     * @param expr The expression to transform (may be modified in place)
     * @return true if any transformations were made in this expression tree
     */
    bool transformExpression(ExprPtr& expr);
    
    /**
     * Transform a statement list, looking for vector operations in assignments
     * @param statements The statement list to transform
     * @return true if any transformations were made
     */
    bool transformStatements(std::vector<StmtPtr>& statements);
    
    /**
     * Print debug information about a transformation
     * @param message The debug message to print
     */
    void debugPrint(const std::string& message) const;
    
    /**
     * Register a temporary variable in the symbol table
     * @param var_name The variable name to register
     * @param var_type The type of the variable
     */
    void registerTempVariable(const std::string& var_name, VarType var_type);
    
    /**
     * Set the current function context for symbol table operations
     * @param function_name The name of the current function being processed
     */
    void setCurrentFunction(const std::string& function_name);
};