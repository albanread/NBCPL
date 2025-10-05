#ifndef NEW_CODE_GENERATOR_H
#define NEW_CODE_GENERATOR_H

#include "DataTypes.h"
#pragma once
#include "analysis/ASTAnalyzer.h"
#include "analysis/LiveInterval.h"
#include "SymbolTable.h"
#include <unordered_map>

#include "AST.h"
#include "ASTVisitor.h"
#include "DataGenerator.h"
#include "Encoder.h"
#include "InstructionStream.h"
#include "CallFrameManager.h"
#include "RegisterManager.h"
#include "CFGBuilderPass.h"
#include "VeneerManager.h"
#include "ExternalFunctionScanner.h"
#include "VectorCodeGen.h"
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <memory>

// Forward declaration for liveness analysis
class LivenessAnalysisPass;
class ReductionCodeGen;

class LabelManager;
class ASTAnalyzer;
class Identifier;
class LivenessAnalysisPass; // <-- Forward declaration for liveness analysis

// Make NewCodeGenerator inherit from ASTVisitor
class NewCodeGenerator : public ASTVisitor {
public:
    void visit(SuperMethodAccessExpression& node) override;
    NewCodeGenerator(InstructionStream& instruction_stream,
                     RegisterManager& register_manager,
                     LabelManager& label_manager,
                     bool debug,
                     int debug_level,
                     DataGenerator& data_generator,
                     unsigned long long text_segment_size,
                     const CFGBuilderPass& cfg_builder,
                     ASTAnalyzer& analyzer,
                     std::unique_ptr<SymbolTable> symbol_table,
                     const std::map<std::string, std::map<std::string, LiveInterval>>& all_allocations,
                     bool is_jit_mode,
                     ClassTable* class_table,
                     LivenessAnalysisPass& liveness_analyzer,
                     bool bounds_checking_enabled,
                     bool use_neon = true);


    // Main entry point
    void generate_code(Program& program);

    // Public accessors and emitters
    CallFrameManager* get_current_frame_manager() { return current_frame_manager_.get(); }
    void emit(const Instruction& instr);
    void emit(const std::vector<Instruction>& instrs);
    void emit_comment(const std::string& comment); // Stub for emitting comments

    // Symbol table access
    SymbolTable* get_symbol_table() const { return symbol_table_.get(); }
    
    // Helper functions for LetDeclaration
    bool is_function_like_declaration(const ExprPtr& initializer) const;
    void handle_function_like_declaration(const std::string& name, const ExprPtr& initializer);
    void handle_local_variable_declaration(const std::string& name, bool is_float = false);
    void handle_valof_block_variable(const std::string& name);

    // Visitor method for SUPER.method() calls
    void visit(SuperMethodCallExpression& node) override;
    bool is_valof_block_variable(const std::string& name) const;
    void handle_global_variable_declaration(const std::string& name, const ExprPtr& initializer);
    void initialize_variable(const std::string& name, const ExprPtr& initializer, bool is_float_declaration = false);



    // Returns true if this code generator is in JIT mode (not static/exec mode)
    bool is_jit_mode() const { return is_jit_mode_; }

    // Helper to get the class name for an object expression (for method calls)
    std::string get_class_name_for_expression(Expression* expr) const;

    // Register allocator helpers
    std::string get_variable_register(const std::string& var_name);
    void store_variable_register(const std::string& var_name, const std::string& value_to_store_reg);

    // Argument coercion helper - handles type conversion and ABI register placement
    void coerce_to_arg(int arg_num, const std::string& src_reg, VarType expr_type, VarType expected_type = VarType::UNKNOWN);
    
    // New ARM64 ABI compliant argument coercion with separate integer/float register tracking
    void coerce_arguments_to_abi(
        const std::vector<std::string>& arg_regs,
        const std::vector<VarType>& arg_types,
        const std::vector<VarType>& expected_types = {}
    );

    // --- Dispatcher and Helper Declarations for FunctionCall ---
    bool is_special_built_in(const std::string& func_name);
    void handle_special_built_in_call(FunctionCall& node, const std::vector<std::string>& arg_result_regs);
    void handle_method_call(FunctionCall& node, const std::vector<std::string>& arg_result_regs);
    void handle_super_call(FunctionCall& node, const std::vector<std::string>& arg_result_regs);
    void handle_regular_call(FunctionCall& node, const std::vector<std::string>& arg_result_regs);
    void handle_method_call_arguments_for_super(FunctionCall& node, const std::vector<std::string>& arg_result_regs, const std::string& func_name);

    bool is_float_function_call(FunctionCall& node);

    // --- Single-Buffer Veneer Management ---
    /**
     * @brief Initializes the veneer manager with the code buffer base address.
     * @param base_address The base address of the code buffer
     */
    void initialize_veneer_manager(uint64_t base_address);
    
    /**
     * @brief Gets the address of a veneer for an external function.
     * @param function_name The name of the external function
     * @return The absolute address of the veneer, or 0 if not found
     */
    uint64_t get_veneer_address(const std::string& function_name) const;
    
    /**
     * @brief Gets the starting address where main function code should begin.
     * This is immediately after all veneers.
     * @return The address for main code start
     */
    uint64_t get_main_code_start_address() const;
    
    /**
     * @brief Gets the veneer manager instance.
     * @return Reference to the veneer manager
     */
    const VeneerManager& get_veneer_manager() const { return veneer_manager_; }

    // Public helper for type inference during code generation (for VectorCodeGen)
    VarType infer_expression_type_local(const Expression* expr) const;
    
    // Public member for expression result register (for VectorCodeGen access)
    std::string expression_result_reg_;

private:
    static constexpr size_t MAX_LDR_OFFSET = 4095 * 8; // 32,760 bytes
    bool is_jit_mode_ = false;
    bool bounds_checking_enabled_ = true;
    bool use_neon_ = true; // NEON SIMD instructions enabled by default
    
    // Vector code generation helper
    std::unique_ptr<VectorCodeGen> vector_codegen_;

    /**
     * @brief Generates the necessary cleanup code for a symbol that owns heap memory.
     * This includes checking for null, determining the object type, and calling the
     * appropriate deallocation routine (e.g., a RELEASE method or a runtime function).
     * @param symbol The symbol for which to generate cleanup code.
     */
    void generate_cleanup_code_for_symbol(const Symbol& symbol);

    const CFGBuilderPass& cfg_builder_;
    VarType current_function_return_type_; // Tracks the return type of the current function

    // --- Register spill/restore helpers ---
    BasicBlock* current_basic_block_ = nullptr;
    const LivenessAnalysisPass& liveness_analyzer_;

    // Phase 4: Manual spill/restore functions removed
    // LinearScanAllocator now handles call-crossing variables intelligently

    // ASTVisitor Interface Implementations
    void visit(Program& node) override;
    void visit(LetDeclaration& node) override;
    void visit(ManifestDeclaration& node) override;
    void visit(StaticDeclaration& node) override;
    void visit(GlobalDeclaration& node) override;
    void visit(GlobalVariableDeclaration& node) override;
    void visit(FunctionDeclaration& node) override;
    void visit(RoutineDeclaration& node) override;
    void visit(LabelDeclaration& node) override;
    void visit(ClassDeclaration& node) override;
    void visit(NumberLiteral& node) override;
    void visit(StringLiteral& node) override;
    void visit(CharLiteral& node) override;
    void visit(BooleanLiteral& node) override;
    void visit(NullLiteral& node) override;
    void visit(VariableAccess& node) override;
    void visit(BinaryOp& node) override;
    void visit(UnaryOp& node) override;
    void visit(VectorAccess& node) override;
    void visit(CharIndirection& node) override;
    void visit(FloatVectorIndirection& node) override;
    void visit(BitfieldAccessExpression& node) override;
    void visit(FunctionCall& node) override;
    void visit(ConditionalExpression& node) override;
    void visit(TableExpression& node) override;
    void visit(NewExpression& node) override; // New: for NewExpression
    void visit(MemberAccessExpression& node) override; // New: for MemberAccessExpression
    void visit(PairExpression& node) override; // New: for PAIR expressions
    void visit(PairAccessExpression& node) override; // New: for PAIR access (.first/.second)
    void visit(FPairExpression& node) override; // New: for FPAIR expressions
    void visit(FPairAccessExpression& node) override; // New: for FPAIR access (.first/.second)
    void visit(QuadExpression& node) override; // New: for QUAD expressions
    void visit(QuadAccessExpression& node) override; // New: for QUAD access (.first/.second/.third/.fourth)

    void visit(FreeStatement& node) override;

    void visit(ListExpression& node) override;

    void visit(ForEachStatement& node) override;

    void visit(OctExpression& node) override;
    void visit(FOctExpression& node) override;
    void visit(LaneAccessExpression& node) override;
    void visit(ValofExpression& node) override;
    void visit(FloatValofExpression& node) override;
    
    // Short-circuit evaluation methods
    void generate_short_circuit_and(BinaryOp& node);
    void generate_short_circuit_or(BinaryOp& node);
    
    // NEON SIMD methods for vector PAIR operations
    bool is_vector_pair_operation(const BinaryOp& node);
    void generate_neon_vector_pair_operation(BinaryOp& node);
    void generate_simple_pair_loop(const std::string& left_addr,
                                   const std::string& right_addr,
                                   const std::string& result_addr,
                                   size_t vector_size,
                                   BinaryOp::Operator op);
    size_t get_vector_size(ExprPtr& expr);
    void visit(VecAllocationExpression& node) override;
    void visit(VecInitializerExpression& node) override;
    void visit(FVecAllocationExpression& node) override;
    void visit(PairsAllocationExpression& node) override;
    void visit(StringAllocationExpression& node) override;
    void visit(AssignmentStatement& node) override;
    void visit(RoutineCallStatement& node) override;
    void visit(IfStatement& node) override;
    void visit(UnlessStatement& node) override;
    void visit(TestStatement& node) override;
    void visit(BrkStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(UntilStatement& node) override;
    void visit(RepeatStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(SwitchonStatement& node) override;
    void visit(CaseStatement& node) override;
    void visit(DefaultStatement& node) override;
    void visit(GotoStatement& node) override;
    void visit(ReturnStatement& node) override;
    void visit(FinishStatement& node) override;
    void visit(BreakStatement& node) override;
    void visit(LoopStatement& node) override;
    void visit(EndcaseStatement& node) override;
    void visit(ResultisStatement& node) override;
    void visit(LabelTargetStatement& node) override;
    void visit(ConditionalBranchStatement& node) override;
    void visit(CompoundStatement& node) override;
    void visit(BlockStatement& node) override;
    void visit(StringStatement& node) override;
    void visit(SysCall& node) override;
    void visit(Expression& node);
    void visit(Declaration& node);

    // Assignment helpers
    void handle_variable_assignment(VariableAccess* var_access, const std::string& value_to_store_reg);
    void handle_member_variable_assignment(const std::string& var_name, const std::string& value_to_store_reg);
    void handle_member_access_assignment(MemberAccessExpression* member_access, const std::string& value_to_store_reg);
    void handle_vector_assignment(VectorAccess* vec_access, const std::string& value_to_store_reg);
    void handle_char_indirection_assignment(CharIndirection* char_indirection, const std::string& value_to_store_reg);
    void handle_float_vector_indirection_assignment(FloatVectorIndirection* float_vec_indirection, const std::string& value_to_store_reg);
    void handle_indirection_assignment(UnaryOp* unary_op, const std::string& value_to_store_reg);

private:
    // Tracks the current class context for mangling method names
    std::string current_class_name_;

    uint64_t text_segment_size_;
    uint64_t data_segment_base_addr_ = 0;
    InstructionStream& instruction_stream_;
    RegisterManager& register_manager_;
    LabelManager& label_manager_;
    DataGenerator& data_generator_;
    std::unique_ptr<CallFrameManager> current_frame_manager_;

    bool debug_enabled_;
    int debug_level;
    bool x28_is_loaded_in_current_function_;
    

    std::string current_function_name_;
    std::vector<std::string> current_function_parameters_;
    std::string current_scope_name_;
    size_t block_id_counter_ = 0;

    std::map<std::string, int> current_scope_symbols_;
    std::stack<std::map<std::string, int>> scope_stack_;

    // --- Function Epilogue Label ---
    std::string current_function_epilogue_label_;

    // --- Function Body Entry Label (for TCO) ---
    std::string current_function_body_label_;

    // Private Helper Methods
    void generate_function_like_code(const std::string& name, const std::vector<std::string>& parameters, ASTNode& body_node, bool is_function_returning_value);
    void enter_scope();
    void exit_scope();

    // --- UnaryOp helpers ---
    void generate_list_intrinsic_code(UnaryOp& node);
    void generate_memory_op_code(UnaryOp& node);
    void generate_len_op_code(UnaryOp& node);
    void generate_float_op_code(UnaryOp& node);
    void generate_integer_op_code(UnaryOp& node);
    void debug_print(const std::string& message) const;
    void debug_print_level(const std::string& message, int level) const;
    bool is_local_variable(const std::string& name) const;
    void set_current_function_allocation(const std::string& function_name);
    bool lookup_symbol(const std::string& name, Symbol& symbol) const;
    void generate_block_epilogue(BasicBlock* block);
    void generate_function_epilogue();
    void generate_expression_code(Expression& expr);
    void generate_statement_code(Statement& stmt);
    void process_declarations(const std::vector<DeclPtr>& declarations);
    void process_declaration(Declaration& decl);

    // --- Linear Scan Register Allocation ---
    // REMOVED: performLinearScan method - all register allocation must be done upfront
    void update_spill_offsets();
    std::map<std::string, LiveInterval> current_function_allocation_;
    size_t max_int_pressure_ = 0;  // Maximum integer register pressure observed
    size_t max_fp_pressure_ = 0;   // Maximum floating-point register pressure observed
    void generate_float_to_int_truncation(const std::string& dest_x_reg, const std::string& src_d_reg);
    
    // Helper to determine if a live interval crosses any function call sites
    bool does_interval_cross_call(const LiveInterval& interval, const std::vector<int>& call_sites) const;

    // --- At-Risk Parameter Detection and Saving ---
    struct AtRiskParameterInfo {
        std::string name;
        std::string abi_register;
        VarType type;
    };
    
    std::vector<AtRiskParameterInfo> find_at_risk_parameters(
        const std::string& function_name, 
        const std::vector<std::string>& parameters, 
        ASTNode& body_node
    );

    // CFG-driven codegen helpers
    
    // Symbol table
    std::unique_ptr<SymbolTable> symbol_table_;
    ClassTable* class_table_ = nullptr; // New: ClassTable reference
    
    // Register allocation
    const std::map<std::string, std::map<std::string, LiveInterval>>& all_allocations_;
    ASTAnalyzer& analyzer_; // Reference to the ASTAnalyzer

    
    // Bounds checking helpers
    std::string get_bounds_error_label_for_current_function();
    std::map<std::string, bool> function_needs_bounds_error_handler_;
    
    // Single-buffer veneer management
    VeneerManager veneer_manager_;
    ExternalFunctionScanner external_scanner_;
    uint64_t code_buffer_base_address_ = 0;
    
    // Helper methods for veneer system
    void update_code_buffer_base_address(uint64_t base_address);
    
    // --- Local Value Tracking for Address Calculations ---
    // Map from variable name to canonical address expression it holds
    std::unordered_map<std::string, std::string> local_value_map_;
    
    // Helper methods for local value tracking
    void register_canonical_address(const std::string& var_name, const std::string& canonical_form);
    bool has_known_address_value(const std::string& var_name) const;
    std::string get_canonical_address_form(const std::string& var_name) const;
    void invalidate_variable_tracking(const std::string& var_name);
    void clear_local_value_tracking(); // Called at function boundaries
    std::string get_expression_canonical_form(const Expression* expr) const;
};

#endif // NEW_CODE_GENERATOR_H