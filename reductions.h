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
class CFGBuilderPass;

// Reduction operation types
enum class ReductionOp {
    MIN,    // Element-wise minimum
    MAX,    // Element-wise maximum  
    SUM     // Element-wise sum
};

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

    // === CFG-Based Reduction Infrastructure ===
    
    /**
     * Create synthetic ForStatement for PAIRS reduction loops
     * @param left_var Left operand variable access
     * @param right_var Right operand variable access  
     * @param op Reduction operation type
     * @return Synthetic ForStatement AST node
     */
    std::unique_ptr<ForStatement> createReductionLoop(
        VariableAccess* left_var,
        VariableAccess* right_var,
        ReductionOp op
    );
    
    /**
     * Inject synthetic loop into current CFG being built
     * @param loop ForStatement to inject
     */
    void injectIntoCurrentCFG(std::unique_ptr<ForStatement> loop);
    
    /**
     * Check if we have access to current CFG builder
     * @return true if CFG integration is available
     */
    bool hasCFGAccess() const;

private:
    // === PAIRS Reductions (Integer) ===
    
    /**
     * Unified PAIRS reduction method (NEON-optimized)
     * Uses SMIN/SMAX/ADD.4S NEON instructions for 4x parallelism
     */
    void generatePairsReduction(const std::vector<ExprPtr>& args, ReductionOp op);
    
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
     * Unified VEC reduction method (NEON-optimized)
     */
    void generateVecReduction(const std::vector<ExprPtr>& args, ReductionOp op);
    
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
     * Unified FVEC reduction method (NEON-optimized)
     */
    void generateFVecReduction(const std::vector<ExprPtr>& args, ReductionOp op);
    
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

    // === Scalar Fallback (--no-neon) ===
    
    /**
     * Pure scalar reduction implementation (no NEON)
     * Used when --no-neon flag is specified or Q-registers unavailable
     */
    void generateScalarReduction(const std::vector<ExprPtr>& args, ReductionOp op);

    /**
     * Chunked processing for large vectors (33+ PAIRS)
     * Uses cache-optimized streaming SIMD with constant register pressure
     */
    void generateChunkedReduction(const std::vector<ExprPtr>& args, ReductionOp op);

    /**
     * Helper function to process a single PAIR using scalar operations
     * Used for remainder handling in chunked processing
     */
    void generateScalarPairReduction(const std::string& left_addr, 
                                    const std::string& right_addr,
                                    const std::string& result_addr,
                                    size_t pair_idx, ReductionOp op);

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

    // === Scalar Operation Helpers ===
    
    /**
     * Emit scalar operation for --no-neon fallback
     * @param result Destination X register
     * @param left First source X register
     * @param right Second source X register
     * @param op Operation to perform (MIN/MAX/SUM)
     */
    void emitScalarOperation(const std::string& result, const std::string& left,
                            const std::string& right, ReductionOp op);

    // === Helper Methods ===
    
    /**
     * Get vector size from BCPL vector register (reads from vec_addr - 8)
     * @param vec_reg Register containing vector address
     * @return Register containing number of elements in vector
     */
    std::string getVectorSize(const std::string& vec_reg);
    
    /**
     * Allocate result vector using GETVEC with same size as input vector
     * @param size_reg Register containing number of elements to allocate
     * @return Register containing allocated vector address
     */
    std::string allocateResultVector(const std::string& size_reg);
    
    /**
     * Emit instruction to instruction stream
     * @param instr Instruction to emit
     */
    void emit(const Instruction& instr);
    
    /**
     * Generate structured reduction loop with proper basic blocks
     * @param left_addr Left vector address register
     * @param right_addr Right vector address register  
     * @param result_addr Result vector address register
     * @param chunks_reg Register containing number of chunks to process
     * @param op Reduction operation type
     */
    void generateStructuredReductionLoop(
        const std::string& left_addr,
        const std::string& right_addr,
        const std::string& result_addr, 
        const std::string& chunks_reg,
        ReductionOp op
    );

    // === Member Variables ===
    
    RegisterManager& register_manager_;   // Reference to register manager
    NewCodeGenerator& code_generator_;    // Reference to code generator
    ASTAnalyzer& analyzer_;              // Reference to AST analyzer for type info
    CFGBuilderPass* current_cfg_builder_; // Pointer to current CFG builder (if available)
};

#endif // REDUCTIONS_H