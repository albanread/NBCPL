#ifndef VECTOR_CODE_GEN_H
#define VECTOR_CODE_GEN_H

#include "DataTypes.h"
#include "AST.h"
#include "RegisterManager.h"
#include "Encoder.h"
#include <string>
#include <vector>
#include <functional>

// Forward declarations
class NewCodeGenerator;
class RegisterManager;

/**
 * @brief Helper class for generating SIMD vector code with dual-path support
 * 
 * This class handles code generation for OCT and FOCT vector types, providing
 * both high-performance ARM NEON instructions and scalar fallback implementations.
 */
class VectorCodeGen {
public:
    /**
     * @brief Constructor
     * @param code_gen Reference to the main code generator
     * @param reg_manager Reference to the register manager
     * @param emit_func Function to emit instructions
     */
    VectorCodeGen(NewCodeGenerator& code_gen, 
                  RegisterManager& reg_manager,
                  std::function<void(const Instruction&)> emit_func);

    /**
     * @brief Generate code for SIMD binary operations (OCT and FOCT)
     * @param node The binary operation AST node
     * @param use_neon Whether to use NEON instructions or scalar fallback
     */
    void generateSimdBinaryOp(BinaryOp& node, bool use_neon);

    /**
     * @brief Generate code for lane read operations (vector.|n|)
     * @param node The lane access expression AST node
     * @param use_neon Whether to use NEON instructions or scalar fallback
     * @return Register containing the extracted lane value
     */
    std::string generateLaneRead(LaneAccessExpression& node, bool use_neon);

    /**
     * @brief Generate code for lane write operations (vector.|n| = value)
     * @param node The lane access expression AST node
     * @param value_expr The expression to write to the lane
     * @param use_neon Whether to use NEON instructions or scalar fallback
     */
    void generateLaneWrite(LaneAccessExpression& node, ExprPtr& value_expr, bool use_neon);

    /**
     * @brief Generate code for OCT literal construction
     * @param node The OCT expression AST node
     * @param use_neon Whether to use NEON instructions or scalar fallback
     * @return Register containing the constructed OCT vector
     */
    std::string generateOctConstruction(OctExpression& node, bool use_neon);

    /**
     * @brief Generate code for FOCT literal construction
     * @param node The FOCT expression AST node
     * @param use_neon Whether to use NEON instructions or scalar fallback
     * @return Register containing the constructed FOCT vector
     */
    std::string generateFOctConstruction(FOctExpression& node, bool use_neon);

    /**
     * @brief Check if a binary operation involves SIMD vector types
     * @param left_type Type of left operand
     * @param right_type Type of right operand
     * @return True if this is a SIMD vector operation
     */
    static bool isSimdOperation(VarType left_type, VarType right_type);

    /**
     * @brief Get the number of lanes for a vector type
     * @param type The vector type
     * @return Number of lanes (2, 4, or 8)
     */
    static int getLaneCount(VarType type);

    /**
     * @brief Get the element type for a vector type
     * @param type The vector type
     * @return Element type (INTEGER or FLOAT)
     */
    static VarType getElementType(VarType type);

    /**
     * @brief Get the NEON arrangement specifier for a vector type
     * @param type The vector type
     * @return NEON arrangement string (e.g., "2S", "4S", "8B")
     */
    static std::string getNeonArrangement(VarType type);

private:
    NewCodeGenerator& code_gen_;
    RegisterManager& register_manager_;
    std::function<void(const Instruction&)> emit_;

    // NEON-specific helper methods
    void generateNeonBinaryOp(BinaryOp& node, VarType result_type);
    void generateNeonLaneRead(LaneAccessExpression& node, VarType vector_type, std::string& result_reg);
    void generateNeonLaneWrite(LaneAccessExpression& node, VarType vector_type, const std::string& value_reg);
    void generateNeonVectorConstruction(const std::vector<const ExprPtr*>& elements, VarType vector_type, std::string& result_reg);

    // Scalar fallback helper methods
    void generateScalarBinaryOp(BinaryOp& node, VarType result_type);
    void generateScalarLaneRead(LaneAccessExpression& node, VarType vector_type, std::string& result_reg);
    void generateScalarLaneWrite(LaneAccessExpression& node, VarType vector_type, const std::string& value_reg);
    void generateScalarVectorConstruction(const std::vector<const ExprPtr*>& elements, VarType vector_type, std::string& result_reg);

    // Utility methods
    void loadVectorToNeon(const std::string& src_reg, const std::string& neon_reg, VarType vector_type);
    void storeNeonToVector(const std::string& neon_reg, const std::string& dst_reg, VarType vector_type);
    void broadcastScalarToNeon(const std::string& scalar_reg, const std::string& neon_reg, VarType vector_type);
    int calculateLaneOffset(int lane_index, VarType vector_type);
    
    // Custom vector instruction encoders (vecgen_* methods)
    Instruction vecgen_fadd_vector(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement);
    Instruction vecgen_fsub_vector(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement);
    Instruction vecgen_fmul_vector(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement);
    Instruction vecgen_fdiv_vector(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement);
    Instruction vecgen_add_vector(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement);
    Instruction vecgen_sub_vector(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement);
    Instruction vecgen_mul_vector(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement);
    
    // Lane operations  
    Instruction vecgen_ins_element(const std::string& vd, int dst_lane, const std::string& vn, int src_lane, const std::string& size);
    Instruction vecgen_ins_general(const std::string& vd, int lane, const std::string& rn, const std::string& size);
    Instruction vecgen_umov_sized(const std::string& rd, const std::string& vn, int lane, const std::string& size);
    Instruction vecgen_mov_element(const std::string& rd, const std::string& vn, int lane, const std::string& size);
    Instruction vecgen_dup_general(const std::string& vd, const std::string& rn, const std::string& arrangement);
    Instruction vecgen_dup_scalar(const std::string& vd, const std::string& vn, const std::string& arrangement);
    
    // Memory operations for Q registers
    Instruction vecgen_ldr_q(const std::string& qt, const std::string& base, int offset);
    Instruction vecgen_str_q(const std::string& qt, const std::string& base, int offset);
    
    // FMOV operations for vector register transfers
    Instruction vecgen_fmov_x_to_d(const std::string& dd, const std::string& xn);
    Instruction vecgen_fmov_d_to_x(const std::string& xd, const std::string& dn);
    Instruction vecgen_fmov_w_to_s(const std::string& sd, const std::string& wn);
    Instruction vecgen_fmov_s_to_w(const std::string& wd, const std::string& sn);
    Instruction vecgen_fmov_s_lane(const std::string& sd, const std::string& vn, int lane);
    Instruction vecgen_umov(const std::string& wd, const std::string& vn, int lane);
    Instruction vecgen_smov(const std::string& wd, const std::string& vn, int lane, const std::string& size);
    Instruction vecgen_fcvt_s_to_d(const std::string& dd, const std::string& sn);
    
    // Register name conversion utilities
    std::string qreg_to_vreg(const std::string& qreg);
    std::string normalizeArrangementForAssembly(const std::string& arrangement);
    int parse_register_number(const std::string& reg);
    
    // Debug and logging
    void debug_print(const std::string& message);
};

#endif // VECTOR_CODE_GEN_H