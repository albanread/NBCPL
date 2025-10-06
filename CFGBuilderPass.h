#ifndef CFG_BUILDER_PASS_H
#define CFG_BUILDER_PASS_H

#include "AST.h"
#include "ASTVisitor.h"
#include "ControlFlowGraph.h"
#include "HeapManager/HeapManager.h"
#include "reductions.h"
#include "Reducer.h"
#include "Reducers.h"
#include <optional>
#include <unordered_map>
#include <utility> // For std::pair
#include <string>
#include <vector>
#include <memory>

// Forward declaration for SymbolTable
class SymbolTable;

// The CFGBuilderPass traverses the AST and constructs a Control Flow Graph
// for each function and routine.
//
// MEMORY CLEANUP RESPONSIBILITY:
// CFGBuilderPass is the SOLE AUTHORITY for automatic memory cleanup of local variables.
// This design ensures cleanup occurs on ALL control flow exit paths (normal exit, 
// early returns, breaks, exceptions, etc.). ASTAnalyzer cleanup logic is permanently 
// disabled to prevent double-free crashes. See visit(BlockStatement) for implementation.
class CFGBuilderPass : public ASTVisitor {
public:
   void visit(ClassDeclaration& node) override;
   CFGBuilderPass(SymbolTable* symbol_table, bool trace_enabled = false);
   std::string getName() const { return "CFG Builder Pass"; }
   void visit(BrkStatement& node) override;
   void build(Program& program);
   void dump_cfgs(); // Debug helper to print all CFGs
   const std::unordered_map<std::string, std::unique_ptr<ControlFlowGraph>>& get_cfgs() const { return function_cfgs; }

private:
   std::string current_class_name_;
    void visit(GlobalVariableDeclaration& node) override;
    void visit(GlobalDeclaration& node) override;

private:
    std::unordered_map<std::string, std::unique_ptr<ControlFlowGraph>> function_cfgs; // Stores a CFG for each function/routine
    ControlFlowGraph* current_cfg; // Pointer to the CFG currently being built
    BasicBlock* current_basic_block; // Pointer to the basic block currently being populated
    std::unordered_map<std::string, BasicBlock*> label_targets; // Maps label names to their corresponding basic blocks
    int block_id_counter; // Counter for generating unique basic block IDs within a function

    // Stacks to keep track of control flow targets for BREAK, LOOP, ENDCASE
    std::vector<BasicBlock*> break_targets; 
    std::vector<BasicBlock*> loop_targets; 
    std::vector<BasicBlock*> endcase_targets; 

    // A list to store all GOTO statements and the block they terminate.
    std::vector<std::pair<GotoStatement*, BasicBlock*>> unresolved_gotos_;

    // --- DEFER support ---
    // Holds deferred statements for the current function/routine being processed.
    
    // --- FOREACH Optimization Support ---
    // Maps variable names to their constant vector sizes (for VecInitializerExpression optimization)
    std::unordered_map<std::string, size_t> constant_vector_sizes_;
    std::vector<StmtPtr> deferred_statements_;

    bool trace_enabled_; // Flag to control debug output

    // Helper for printing debug messages
    void debug_print(const std::string& message);

    // Helper to check if legacy cleanup should be performed (disabled when SAMM is active)
    bool should_perform_legacy_cleanup() const;

    // SymbolTable pointer for symbol lifetime and cleanup logic
    SymbolTable* symbol_table_;

    // Block tracking for cleanup
    int current_block_id_counter; // Counter for generating unique block IDs
    std::vector<std::vector<std::string>> block_variable_stack; // Stack of variables declared in each block


    // Helper to create a new basic block and add it to the current CFG
    BasicBlock* create_new_basic_block(const std::string& id_prefix = "BB_");

    // Helper to end the current basic block and start a new one, adding a fall-through edge
    void end_current_block_and_start_new();
    
    // Resolves all pending GOTO statements by creating edges in the CFG
    void resolve_gotos();

    // Helper method to validate the CFG structure, especially for loops
    void validate_block_structure(const std::string& function_name);

    // Helper to generate cleanup code for all variables in current scope and outer scopes
    // Returns the final cleanup block that should be connected to the target
    BasicBlock* generate_scope_cleanup_chain(const std::string& cleanup_prefix);

    // ASTVisitor overrides
    void visit(Program& node) override;
    void visit(DeferStatement& node) override;
    void visit(ForEachStatement& node) override;

    // Helpers for FOREACH/FFOREACH CFG construction
    void build_vector_foreach_cfg(ForEachStatement& node);
    void build_list_foreach_cfg(ForEachStatement& node);
    void build_destructuring_list_foreach_cfg(ForEachStatement& node);
    
    // Optimization helpers for FOREACH loop register pressure reduction
    bool is_simple_variable_access(Expression* expr, std::string& var_name);
    bool is_constant_size_collection(Expression* expr, int& size);
    void visit(LetDeclaration& node) override;
    void visit(ManifestDeclaration& node) override;
    void visit(StaticDeclaration& node) override;

    void visit(FunctionDeclaration& node) override;
    void visit(RoutineDeclaration& node) override;
    void visit(LabelDeclaration& node) override; // Keep to satisfy pure virtual requirement

    void visit(NumberLiteral& node) override;
    void visit(StringLiteral& node) override;
    void visit(CharLiteral& node) override;
    void visit(BooleanLiteral& node) override;
    void visit(VariableAccess& node) override;
    void visit(BinaryOp& node) override;
    void visit(UnaryOp& node) override;
    void visit(VectorAccess& node) override;
    void visit(CharIndirection& node) override;
    void visit(FloatVectorIndirection& node) override;
    void visit(FunctionCall& node) override;
    void visit(ConditionalExpression& node) override;
    void visit(ValofExpression& node) override;


    void visit(AssignmentStatement& node) override;
    void visit(RoutineCallStatement& node) override;
    void visit(IfStatement& node) override;
    void visit(UnlessStatement& node) override;
    void visit(TestStatement& node) override;
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
    void visit(CompoundStatement& node) override;
    void visit(BlockStatement& node) override;
    void visit(StringStatement& node) override;
    void visit(FreeStatement& node) override;
    void visit(LabelTargetStatement& node) override;
    void visit(ConditionalBranchStatement& node) override;
    void visit(SysCall& node) override;
    void visit(VecAllocationExpression& node) override;
    void visit(StringAllocationExpression& node) override;
    void visit(TableExpression& node) override;

    // FloatValofExpression visitor
    void visit(FloatValofExpression& node) override;
    
    // Reduction statement visitors
    void visit(MinStatement& node) override;
    void visit(MaxStatement& node) override;
    void visit(SumStatement& node) override;
    void visit(ReductionStatement& node) override;  // New modular reduction visitor
    
    // Helper method for generating reduction CFG structures
    void generateReductionCFG(Expression* left_expr, Expression* right_expr,
                             const std::string& result_var, int op);
    
    // Overloaded version that accepts Reducer interface (modern approach)
    void generateReductionCFG(Expression* left_expr, Expression* right_expr,
                             const std::string& result_var, const Reducer& reducer);
    
    // Pairwise reduction CFG generation for NEON intrinsics
    void generatePairwiseReductionCFG(Expression* left_expr, Expression* right_expr,
                                     const std::string& result_var, const PairwiseMinReducer& reducer);
    void generatePairwiseReductionCFG(Expression* left_expr, Expression* right_expr,
                                     const std::string& result_var, const PairwiseMaxReducer& reducer);
    void generatePairwiseReductionCFG(Expression* left_expr, Expression* right_expr,
                                     const std::string& result_var, const PairwiseAddReducer& reducer);
    
    // Component-wise reduction CFG generation for PAIRS with NEON optimization
    void generateComponentWiseReductionCFG(Expression* left_expr, Expression* right_expr,
                                          const std::string& result_var, const Reducer& reducer);
    
    // Helper method to detect PAIRS type
    bool isPairsType(Expression* expr);
};

#endif // CFG_BUILDER_PASS_H
