#ifndef EXTERNAL_FUNCTION_SCANNER_H
#define EXTERNAL_FUNCTION_SCANNER_H

#include "AST.h"
#include "ASTVisitor.h"
#include "RuntimeManager.h"
#include <set>
#include <string>

/**
 * @brief Scans an AST to find all unique external runtime functions that need veneers.
 * 
 * This scanner performs a pre-analysis pass to identify all external function calls
 * before code generation begins. This allows us to:
 * 1. Calculate the exact size needed for the veneer section
 * 2. Generate all veneers at the beginning of the code buffer
 * 3. Eliminate the need for complex relocations and multi-segment linking
 */
class ExternalFunctionScanner : public ASTVisitor {
public:
    ExternalFunctionScanner() = default;
    
    /**
     * @brief Scans the given AST node and collects all external function calls.
     * @param node The root AST node to scan (typically a Program or Function)
     * @return A set of unique external function names that need veneers
     */
    std::set<std::string> scan(ASTNode& node);
    
    /**
     * @brief Gets the collected external function names.
     * @return Set of unique external function names
     */
    const std::set<std::string>& get_external_functions() const { return external_functions_; }
    
    /**
     * @brief Calculates the total size needed for all veneers.
     * Each veneer consists of:
     * - LDR X10, #16      (4 bytes) - Load function pointer
     * - BR X10            (4 bytes) - Branch to function
     * - .quad <address>   (8 bytes) - 64-bit function pointer
     * @return Total size in bytes for the veneer section
     */
    size_t calculate_veneer_section_size() const;
    
    /**
     * @brief Gets the size of a single veneer in bytes.
     * @return Size of one veneer (16 bytes: LDR + BR + pointer)
     */
    static constexpr size_t get_veneer_size() { return 16; }

    // ASTVisitor interface - robust traversal for all relevant nodes
    void visit(Program& node) override;
    void visit(FunctionCall& node) override;
    void visit(RoutineCallStatement& node) override;
    void visit(LetDeclaration& node) override;
    void visit(FunctionDeclaration& node) override;
    void visit(RoutineDeclaration& node) override;
    void visit(BinaryOp& node) override;
    void visit(UnaryOp& node) override;
    void visit(VectorAccess& node) override;
    void visit(ConditionalExpression& node) override;
    void visit(AssignmentStatement& node) override;
    void visit(IfStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(BlockStatement& node) override;
    void visit(CompoundStatement& node) override;
    void visit(ValofExpression& node) override;
    void visit(FloatValofExpression& node) override;
    void visit(ForEachStatement& node) override;
    void visit(UnlessStatement& node) override;
    void visit(TestStatement& node) override;
    void visit(RepeatStatement& node) override;
    void visit(UntilStatement& node) override;
    void visit(SwitchonStatement& node) override;
    void visit(ResultisStatement& node) override;
    void visit(FreeStatement& node) override;
    void visit(NewExpression& node) override;
    void visit(MemberAccessExpression& node) override;
    void visit(SuperMethodCallExpression& node) override;
    void visit(VecAllocationExpression& node) override;
    void visit(VecInitializerExpression& node) override;
    void visit(FVecAllocationExpression& node) override;
    void visit(PairsAllocationExpression& node) override;
    void visit(StringAllocationExpression& node) override;
    void visit(TableExpression& node) override;
    void visit(ListExpression& node) override;

    // Empty implementations for true leaf nodes and rarely-used nodes
    void visit(ManifestDeclaration& node) override {}
    void visit(StaticDeclaration& node) override {}
    void visit(GlobalDeclaration& node) override {}
    void visit(GlobalVariableDeclaration& node) override {}
    void visit(LabelDeclaration& node) override {}
    void visit(ClassDeclaration& node) override {}
    void visit(NumberLiteral& node) override {}
    void visit(StringLiteral& node) override {}
    void visit(CharLiteral& node) override {}
    void visit(BooleanLiteral& node) override {}
    void visit(NullLiteral& node) override {}
    void visit(VariableAccess& node) override {}
    void visit(CharIndirection& node) override {}
    void visit(FloatVectorIndirection& node) override {}
    void visit(BitfieldAccessExpression& node) override {}
    void visit(SuperMethodAccessExpression& node) override {}
    void visit(BrkStatement& node) override {}
    void visit(CaseStatement& node) override {}
    void visit(DefaultStatement& node) override {}
    void visit(GotoStatement& node) override {}
    void visit(ReturnStatement& node) override {}
    void visit(FinishStatement& node) override {}
    void visit(BreakStatement& node) override {}
    void visit(LoopStatement& node) override {}
    void visit(EndcaseStatement& node) override {}
    void visit(LabelTargetStatement& node) override {}
    void visit(ConditionalBranchStatement& node) override {}
    void visit(StringStatement& node) override {}
    void visit(SysCall& node) override {}

private:
    std::set<std::string> external_functions_;
    
    /**
     * @brief Helper to check if a function name is an external runtime function.
     * @param function_name The name of the function to check
     * @return True if it's an external runtime function, false otherwise
     */
    bool is_external_function(const std::string& function_name) const;
};

#endif // EXTERNAL_FUNCTION_SCANNER_H