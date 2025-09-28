#pragma once

#include "../AST.h"
#include "../ASTVisitor.h"
#include <map>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include "../DataTypes.h"
#include "../SymbolTable.h"
#include "../ClassTable.h"

// VarType is now defined in DataTypes.h
// Removed custom optional implementation

// Forward declaration for WRITEF validation - actual definition in gen_RoutineCallStatement.cpp
struct FormatSpecifier;



/**
 * @class ASTAnalyzer
 * @brief Traverses the Abstract Syntax Tree to gather semantic information.
 */
class ASTAnalyzer : public ASTVisitor {
  public:
    // VarType now in global namespace
    const std::map<std::string, VarType>& get_function_return_types() const { return function_return_types_; }

    static ASTAnalyzer& getInstance();
    void analyze(Program& program, SymbolTable* symbol_table = nullptr, ClassTable* class_table = nullptr);
    void transform(Program& program);
    void print_report() const;

    // Get the symbol table
    void visit(ListExpression& node) override;
    void visit(MemberAccessExpression& node) override;
    void visit(SuperMethodAccessExpression& node) override;
    SymbolTable* get_symbol_table() const { return symbol_table_; }

    const std::map<std::string, FunctionMetrics>& get_function_metrics() const { return function_metrics_; }
    std::map<std::string, FunctionMetrics>& get_function_metrics_mut() { return function_metrics_; }
    const std::map<std::string, std::string>& get_variable_definitions() const { return variable_definitions_; }
    const ForStatement& get_for_statement(const std::string& unique_name) const;
    
    // Safe parameter type setter that preserves SignatureAnalysisVisitor results
    void set_parameter_type_safe(const std::string& function_name, const std::string& param_name, VarType new_type);
    
    // Return a list of instruction indices where function calls occur in the given function
    const std::vector<int>& get_call_sites_for(const std::string& function_name) const;
    
    // Register a call site for injected function calls (e.g., SAMM scope calls)
    void register_call_site(const std::string& function_name, int instruction_index);
    
    // Register a function call in the call graph
    void register_function_call(const std::string& caller, const std::string& callee);
    
    // Get the set of functions called by the given function
    const std::set<std::string>& get_callees_for(const std::string& function_name) const;
    
    // Propagate heap allocation information through the call graph
    void propagate_heap_allocation_info();

    // Infer the type of an expression (INTEGER, FLOAT, POINTER, etc.)
    VarType infer_expression_type(const Expression* expr) const;
    bool function_accesses_globals(const std::string& function_name) const;
    int64_t evaluate_constant_expression(Expression* expr, bool* has_value) const;
    bool is_local_routine(const std::string& name) const;
    bool is_local_function(const std::string& name) const;
    bool is_local_float_function(const std::string& name) const;

    // --- New Public Query Methods ---
    // Returns true if the function makes no calls.
    bool is_leaf_function(const std::string& function_name) const;
    // Returns true if the function is a trivial class accessor method.
    bool is_trivial_accessor_method(const std::string& function_name) const;
    // Returns true if the function is a trivial setter method.
    bool is_trivial_setter_method(const std::string& function_name) const;
    // Returns true if a ListExpression contains only literal values
    bool list_contains_only_literals(const ListExpression& node) const;
    // --- End New Public Query Methods ---

    VarType get_variable_type(const std::string& function_name, const std::string& var_name) const;
    VarType get_class_member_type(const MemberAccessExpression* member_access) const;
    std::string infer_object_class_name(const Expression* expr) const;
    FunctionType get_runtime_function_type(const std::string& name) const;

    // Enable or disable trace output
    void set_trace_enabled(bool enabled) { trace_enabled_ = enabled; }
    bool is_trace_enabled() const { return trace_enabled_; }
    
    // Methods for signature analysis coordination
    void set_signature_analysis_complete(bool complete) { signature_analysis_complete_ = complete; }
    bool is_signature_analysis_complete() const { return signature_analysis_complete_; }

    // Visitor methods
    void visit(Program& node) override;
    void visit(LetDeclaration& node) override;
    void visit(FunctionDeclaration& node) override;
    void visit(RoutineDeclaration& node) override;
    void visit(BlockStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(SuperMethodCallExpression& node) override;
    void visit(ForEachStatement& node) override;
    void visit(FunctionCall& node) override;
    void visit(RoutineCallStatement& node) override;
    void visit(VariableAccess& node) override;
    void visit(VecAllocationExpression& node) override;
    void visit(FVecAllocationExpression& node) override;
    void visit(VecInitializerExpression& node) override;
    void visit(NewExpression& node) override;
    void visit(SwitchonStatement& node) override;
    void visit(ManifestDeclaration& node) override;
    void visit(StaticDeclaration& node) override;
    void visit(GlobalDeclaration& node) override;
    void visit(LabelDeclaration& node) override;
    void visit(NumberLiteral& node) override;
    void visit(StringLiteral& node) override;
    void visit(CharLiteral& node) override;
    void visit(BooleanLiteral& node) override;
    void visit(BinaryOp& node) override;
    void visit(UnaryOp& node) override;
    void visit(VectorAccess& node) override;
    void visit(CharIndirection& node) override;
    void visit(FloatVectorIndirection& node) override;
    void visit(BitfieldAccessExpression& node) override;
    void visit(ConditionalExpression& node) override;
    void visit(ValofExpression& node) override;
    void visit(StringAllocationExpression& node) override;
    void visit(TableExpression& node) override;
    void visit(AssignmentStatement& node) override;
    void visit(DeferStatement& node) override;
    void visit(RetainStatement& node) override; // RETAIN semantic handler
    void visit(RemanageStatement& node) override; // REMANAGE semantic handler
    void visit(ResultisStatement& node) override;
    void visit(TestStatement& node) override;
    void visit(UnlessStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(IfStatement& node) override;
    void visit(ReturnStatement& node) override;
    void visit(UntilStatement& node) override;
    void visit(RepeatStatement& node) override;
    void visit(CaseStatement& node) override;
    void visit(DefaultStatement& node) override;
    void visit(GotoStatement& node) override;

    void visit(FinishStatement& node) override;
    void visit(BreakStatement& node) override;
    void visit(LoopStatement& node) override;
    void visit(EndcaseStatement& node) override;

    void visit(CompoundStatement& node) override;
    void visit(StringStatement& node) override;
    void visit(GlobalVariableDeclaration& node) override;
    void visit(BrkStatement& node) override;
    void visit(FreeStatement& node) override;
    void visit(LabelTargetStatement& node) override;
    void visit(ConditionalBranchStatement& node) override;
    void visit(SysCall& node) override;
    void visit(ClassDeclaration& node) override; // New: for ClassDeclaration
    void visit(PairExpression& node) override; // New: for pair(x,y) expressions
    void visit(PairAccessExpression& node) override; // New: for p.first/p.second access
    void visit(FPairExpression& node) override; // New: for fpair(x,y) expressions
    void visit(FPairAccessExpression& node) override; // New: for fp.first/fp.second access

  private:
    std::string current_class_name_; // Tracks the current class context for mangling method names
    std::vector<std::string> semantic_errors_; // Collects semantic errors for reporting

    ASTAnalyzer();
    ASTAnalyzer(const ASTAnalyzer&) = delete;
    ASTAnalyzer& operator=(const ASTAnalyzer&) = delete;

    void reset_state();
    std::string get_effective_variable_name(const std::string& original_name) const;
    void first_pass_discover_functions(Program& program);
    void transform_let_declarations(std::vector<DeclPtr>& declarations);
    void processClassMethods(const std::string& class_name);

    // --- New Private Analysis Helper ---
    // Analyzes a function body AST node to determine if it's a trivial accessor.
    // Returns the name of the accessed member if true, otherwise an empty string.
    std::string identify_trivial_accessor(const ASTNode* body, const std::string& class_name);

    // Analyzes a routine body AST node to determine if it's a trivial setter.
    // Returns the name of the member being set if true, otherwise an empty string.
    std::string identify_trivial_setter(const ASTNode* body, const std::string& class_name);

    // --- Parameter Type Inference ---
    // Analyzes how parameters are used in a function body to infer their types
    void infer_parameter_types(const std::string& function_name, const std::vector<std::string>& parameters, ASTNode* body);
    
    // Helper to infer parameter type from usage context
    void infer_parameter_type_from_context(const std::string& function_name, const std::string& param_name, VarType context_type);
    
    // Helper methods for parameter type inference
    VarType analyze_parameter_usage(ASTNode* node, const std::string& param_name, const std::string& function_name);
    bool uses_parameter(ASTNode* node, const std::string& param_name);
    // --- End New Private Analysis Helper ---

    // --- WRITEF Format String Validation ---
    void validate_writef_format_types(RoutineCallStatement& node);
    std::vector<char> parse_writef_format_string(const std::string& format_str);
    VarType get_expected_type_for_writef_specifier(char specifier);
    bool are_types_compatible_for_writef(VarType actual, VarType expected);
    std::string var_type_to_string(VarType type);
    // --- End WRITEF Validation ---

    // --- Modular Type Inference Helpers ---
    VarType infer_literal_type(const Expression* expr) const;
    VarType infer_variable_access_type(const VariableAccess* var_access) const;
    VarType infer_function_call_type(const FunctionCall* func_call) const;
    VarType infer_binary_op_type(const BinaryOp* bin_op) const;
    VarType infer_unary_op_type(const UnaryOp* un_op) const;
    VarType infer_collection_type(const Expression* expr) const;
    VarType infer_access_type(const Expression* expr) const;
    VarType infer_allocation_type(const Expression* expr) const;
    VarType infer_conditional_type(const ConditionalExpression* cond_expr) const;
    VarType infer_valof_type(const Expression* expr) const;
    
    // Helper method to check if a list type is const/read-only
    bool is_const_list_type(VarType type) const;

    // --- Scope Management ---
    std::string current_function_scope_;
    std::string current_lexical_scope_;

public:
    // Expose setter and getter for current_function_scope_
    void set_current_function_scope(const std::string& scope) { current_function_scope_ = scope; }
    std::string get_current_function_scope() const { return current_function_scope_; }

    // Helper method to lookup class name from symbol table
    std::string lookup_variable_class_name(const std::string& var_name) const;

    // --- Data Members ---
    std::map<std::string, FunctionMetrics> function_metrics_;
    std::map<std::string, VarType> function_return_types_;
    std::map<std::string, std::string> variable_definitions_;
    std::set<std::string> local_function_names_;
    std::set<std::string> local_routine_names_;
    std::set<std::string> local_float_function_names_;
    int for_loop_var_counter_ = 0;
    std::map<std::string, std::string> for_variable_unique_aliases_;

    // Tracks the parameters of the currently visited routine (for setter detection)
    std::vector<std::string> current_routine_parameters_;
    
    // Map from function name to a list of instruction points that are function calls
    std::map<std::string, std::vector<int>> function_call_sites_;
    // Empty vector to return when a function has no call sites
    mutable std::vector<int> empty_call_sites_;
    
    // Call graph: maps caller function name to set of callee function names
    std::map<std::string, std::set<std::string>> call_graph_;
    // Empty set to return when a function has no callees
    mutable std::set<std::string> empty_callees_;

    // Visitor for FloatValofExpression
    void visit(FloatValofExpression& node) override;
    
    // Helper methods for visibility checking
    bool check_member_access(Visibility member_visibility, 
                            const std::string& defining_class, 
                            const std::string& accessing_class);
    void log_access_violation(const std::string& member_type,
                             const std::string& member_name,
                             const std::string& defining_class,
                             Visibility visibility);
    
    std::stack<std::map<std::string, std::string>> active_for_loop_scopes_;
    int for_loop_instance_suffix_counter = 0;
    std::map<std::string, const ForStatement*> for_statements_;
    bool trace_enabled_ = false;
    bool signature_analysis_complete_ = false; // Flag to prevent resetting after signature analysis
    
    // Track current loop context to prevent FOR loop state leakage
    enum class LoopContext { NONE, FOR_LOOP, WHILE_LOOP, UNTIL_LOOP, REPEAT_LOOP, FOREACH_LOOP };
    std::stack<LoopContext> loop_context_stack_;
    
    // Symbol table reference (owned by the compiler)
    SymbolTable* symbol_table_ = nullptr;
    ClassTable* class_table_ = nullptr; // New: ClassTable reference
    // Getter for semantic errors
    const std::vector<std::string>& getSemanticErrors() const { return semantic_errors_; }
};