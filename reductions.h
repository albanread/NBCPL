#ifndef REDUCTIONS_H
#define REDUCTIONS_H

#include "DataTypes.h"
#include "AST.h"
#include "Encoder.h"
#include "RegisterManager.h"
#include <string>
#include <vector>

// Forward declarations
class NewCodeGenerator;
class ASTAnalyzer;

/**
 * ReductionCodeGen - Handles vector reduction operations (MIN, MAX, SUM, etc.)
 * 
 * This class provides type-aware reduction operations for all vector types:
 * - PAIRS: Integer pair vectors using SMIN/SMAX NEON instructions
 * - FPAIRS: Float pair vectors using FMIN/FMAX NEON instructions  
 * - VEC: Integer vectors
 * - FVEC: Float vectors
 * 
 * Operations are dispatched based on argument types and generate optimized
 * NEON instruction sequences for maximum performance.
 */
class ReductionCodeGen {
public:
    /**
     * Constructor
     * @param register_manager Reference to register allocation manager
     * @param code_generator Reference to main code generator
     * @param analyzer Reference to AST analyzer for type inference
     */
    ReductionCodeGen(RegisterManager& register_manager, 
                     NewCodeGenerator& code_generator,
                     ASTAnalyzer& analyzer);

    /**
     * Main entry point for reduction operations
     * Analyzes argument types and dispatches to appropriate implementation
     * 
     * @param node FunctionCall node (MIN, MAX, SUM, etc.)
     * @param op_name Operation name ("MIN", "MAX", "SUM")
     */
    void generateReduction(FunctionCall& node, const std::string& op_name);

    /**
     * Check if a function name is a supported reduction operation
     * @param func_name Function name to check
     * @return true if it's a reduction operation
     */
    static bool isReductionOperation(const std::string& func_name);

private:
    // === PAIRS Reductions (Integer) ===
    
    /**
     * Generate element-wise minimum for PAIRS vectors
     * Uses SMIN.4S NEON instructions for 4x parallelism
     */
    void generatePairsMin(const std::vector<ExprPtr>& args);
    
    /**
     * Generate element-wise maximum for PAIRS vectors  
     * Uses SMAX.4S NEON instructions for 4x parallelism
     */
    void generatePairsMax(const std::vector<ExprPtr>& args);
    
    /**
     * Generate element-wise sum for PAIRS vectors
     * Uses ADD.4S NEON instructions for 4x parallelism
     */
    void generatePairsSum(const std::vector<ExprPtr>& args);

    // === FPAIRS Reductions (Float) ===
    
    /**
     * Generate element-wise minimum for FPAIRS vectors
     * Uses FMIN.4S NEON instructions for 4x parallelism
     */
    void generateFPairsMin(const std::vector<ExprPtr>& args);
    
    /**
     * Generate element-wise maximum for FPAIRS vectors
     * Uses FMAX.4S NEON instructions for 4x parallelism  
     */
    void generateFPairsMax(const std::vector<ExprPtr>& args);
    
    /**
     * Generate element-wise sum for FPAIRS vectors
     * Uses FADD.4S NEON instructions for 4x parallelism
     */
    void generateFPairsSum(const std::vector<ExprPtr>& args);

    // === VEC Reductions (Integer vectors) ===
    
    /**
     * Generate minimum reduction for VEC (integer vectors)
     */
    void generateVecMin(const std::vector<ExprPtr>& args);
    
    /**
     * Generate maximum reduction for VEC (integer vectors)
     */
    void generateVecMax(const std::vector<ExprPtr>& args);
    
    /**
     * Generate sum reduction for VEC (integer vectors)
     */
    void generateVecSum(const std::vector<ExprPtr>& args);

    // === FVEC Reductions (Float vectors) ===
    
    /**
     * Generate minimum reduction for FVEC (float vectors)
     */
    void generateFVecMin(const std::vector<ExprPtr>& args);
    
    /**
     * Generate maximum reduction for FVEC (float vectors)
     */
    void generateFVecMax(const std::vector<ExprPtr>& args);
    
    /**
     * Generate sum reduction for FVEC (float vectors)
     */
    void generateFVecSum(const std::vector<ExprPtr>& args);

    // === NEON Instruction Generators ===
    
    /**
     * Generate SMIN.4S instruction for integer minimum
     * @param vd Destination V register
     * @param vn First source V register  
     * @param vm Second source V register
     * @return Encoded instruction
     */
    Instruction vecgen_smin_4s(const std::string& vd, 
                               const std::string& vn, 
                               const std::string& vm);
    
    /**
     * Generate SMAX.4S instruction for integer maximum
     * @param vd Destination V register
     * @param vn First source V register
     * @param vm Second source V register  
     * @return Encoded instruction
     */
    Instruction vecgen_smax_4s(const std::string& vd, 
                               const std::string& vn, 
                               const std::string& vm);
    
    /**
     * Generate FMIN.4S instruction for float minimum
     * @param vd Destination V register
     * @param vn First source V register
     * @param vm Second source V register
     * @return Encoded instruction
     */
    Instruction vecgen_fmin_4s(const std::string& vd, 
                               const std::string& vn, 
                               const std::string& vm);
    
    /**
     * Generate FMAX.4S instruction for float maximum
     * @param vd Destination V register
     * @param vn First source V register
     * @param vm Second source V register
     * @return Encoded instruction
     */
    Instruction vecgen_fmax_4s(const std::string& vd, 
                               const std::string& vn, 
                               const std::string& vm);

    // === Helper Methods ===
    
    /**
     * Get vector size from expression (for loop bounds)
     * @param expr Vector expression
     * @return Number of elements in vector
     */
    size_t getVectorSize(Expression* expr);
    
    /**
     * Allocate result vector of appropriate type and size
     * @param vector_type Type of vector (PAIRS, FPAIRS, etc.)
     * @param vector_size Number of elements
     * @return Register containing allocated vector address
     */
    std::string allocateResultVector(VarType vector_type, size_t vector_size);
    
    /**
     * Emit instruction to instruction stream
     * @param instr Instruction to emit
     */
    void emit(const Instruction& instr);

    // === Member Variables ===
    
    RegisterManager& register_manager_;   // Reference to register manager
    NewCodeGenerator& code_generator_;    // Reference to code generator
    ASTAnalyzer& analyzer_;              // Reference to AST analyzer for type info
};

#endif // REDUCTIONS_H