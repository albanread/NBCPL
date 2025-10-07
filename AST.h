#ifndef AST_H
#define AST_H

#include <vector>
#include <string>
#include <memory>
#include <cstdint>
#include <iostream>

// Custom nullable type since std::optional might not be available
template<typename T>
class OptionalValue {
    bool has_value_;
    T value_;
public:
    OptionalValue() : has_value_(false) {}
    OptionalValue(T value) : has_value_(true), value_(value) {}
    bool has_value() const { return has_value_; }
    T value() const { return value_; }
    void reset() { has_value_ = false; }
};

// Include DataTypes.h for ExprPtr definition
#include "DataTypes.h"

// Forward declarations
class Reducer;

// Visibility enum for class members
enum class Visibility {
    Public,
    Private,
    Protected
};

// Smart pointer typedefs for AST nodes
typedef std::unique_ptr<class ASTNode> ASTNodePtr;
typedef std::unique_ptr<class Program> ProgramPtr;
typedef std::unique_ptr<class Declaration> DeclPtr;
// ExprPtr is already defined in DataTypes.h
typedef std::unique_ptr<class Statement> StmtPtr;

// Forward declarations for smart pointers
class ASTNode;
class Program;
class Declaration;
class Expression;
class Statement;
class LabelTargetStatement;
class ConditionalBranchStatement;
class CaseStatement;
class DefaultStatement;
class TableExpression;

// Forward declaration of the ASTVisitor interface
class ASTVisitor;

// Helper function to clone a single unique_ptr (defined in AST.cpp)
template <typename T>
std::unique_ptr<T> clone_unique_ptr(const std::unique_ptr<T>& original_ptr);

// Base class for all AST nodes
class ASTNode {
public:
    enum class NodeType {
        Program, Declaration, Expression, Statement,
        LetDecl, ManifestDecl, StaticDecl, GlobalDecl, FunctionDecl, RoutineDecl, LabelDecl,
        NumberLit, StringLit, CharLit, BooleanLit, NullLit, VariableAccessExpr, BinaryOpExpr, UnaryOpExpr,
        VectorAccessExpr, CharIndirectionExpr, FloatVectorIndirectionExpr, FunctionCallExpr, SysCallExpr,
        BitfieldAccessExpr, // <-- Added for bitfield operator
        ListExpr, // <-- Added for LIST syntax
        ClassDecl, NewExpr, MemberAccessExpr, // New: Class, New, Member Access
        SuperMethodCallExpr, // New: For SUPER.method() calls
        SuperMethodAccessExpr, // New: For SUPER.method access
        PairExpr, PairAccessExpr, // New: For pair(x,y) and p.first/p.second
        FPairExpr, FPairAccessExpr, // New: For fpair(x,y) and fp.first/fp.second
        QuadExpr, QuadAccessExpr, // New: For quad(a,b,c,d) and q.first/q.second/q.third/q.fourth
        FQuadExpr, FQuadAccessExpr, // New: For fquad(a,b,c,d) and fq.first/fq.second/fq.third/fq.fourth
        OctExpr, FOctExpr, // New: For oct(...) and foct(...) 8-lane vectors
        LaneAccessExpr, // New: For vector.|n| lane access
        ConditionalExpr, ValofExpr, FloatValofExpr, VecAllocationExpr, FVecAllocationExpr, PairsAllocationExpr, FPairsAllocationExpr, StringAllocationExpr, TableExpr,
        VecInitializerExpr, // Add this new type
        AssignmentStmt, RoutineCallStmt, IfStmt, UnlessStmt, TestStmt, WhileStmt, UntilStmt,
        RepeatStmt, ForStmt, ForEachStmt, SwitchonStmt, GotoStmt, ReturnStmt, FinishStmt, BreakStmt,
        LoopStmt, EndcaseStmt, ResultisStmt, CompoundStmt, BlockStmt, StringStmt, FreeStmt,
        CaseStmt, DefaultStmt, BrkStatement, LabelTargetStmt, ConditionalBranchStmt,
        DeferStmt, // <-- Added for DEFER statement
        RetainStmt, // <-- Added for RETAIN statement
        RemanageStmt, // <-- Added for REMANAGE statement
        MinStmt, // <-- Added for MIN statement (DEPRECATED - use ReductionStmt)
        MaxStmt, // <-- Added for MAX statement (DEPRECATED - use ReductionStmt)  
        SumStmt, // <-- Added for SUM statement (DEPRECATED - use ReductionStmt)
        ReductionStmt, // <-- Added for generic reduction statement
        ReductionLoopStmt, // <-- Added for reduction loop metadata
        PairwiseReductionLoopStmt // <-- Added for pairwise reduction loop metadata
    };

    ASTNode(NodeType type) : type_(type) {}
    virtual ~ASTNode();

    virtual NodeType getType() const { return type_; }

    virtual void accept(ASTVisitor& visitor) = 0;
    virtual ASTNodePtr clone() const = 0;
    virtual void replace_with(ASTNodePtr new_node) {
        // Default implementation does nothing.
        // This should be overridden by nodes that can be replaced.
    }

private:
    NodeType type_;
};

// --- Program --- //
class Program : public ASTNode {
public:
    std::vector<DeclPtr> declarations;
    std::vector<StmtPtr> statements;

    Program() : ASTNode(NodeType::Program) {}
    virtual ~Program();
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

// --- Declarations --- //
class Declaration : public ASTNode {
public:
    Declaration(NodeType type) : ASTNode(type) {}
    virtual ~Declaration(); // Make Declaration polymorphic
};

// --- Statements --- //

// --- FreeStatement for FREEVEC and FREELIST ---

class Statement : public ASTNode {
public:
    Statement(NodeType type) : ASTNode(type) {}
    virtual ~Statement();
    virtual void accept(ASTVisitor& visitor) = 0;
    virtual ASTNodePtr clone() const = 0;

    // --- Live interval/liveness analysis helpers ---
    virtual std::vector<std::string> get_used_variables() const { return {}; }
    virtual std::vector<std::string> get_defined_variables() const { return {}; }
};

// --- RetainStatement for RETAIN keyword ---
class RetainStatement : public Statement {
public:
    std::vector<std::string> variable_names;

    RetainStatement(std::vector<std::string> names)
        : Statement(NodeType::RetainStmt), variable_names(std::move(names)) {}

    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

// --- RemanageStatement for REMANAGE keyword ---
class RemanageStatement : public Statement {
public:
    std::vector<std::string> variable_names;

    RemanageStatement(std::vector<std::string> names)
        : Statement(NodeType::RemanageStmt), variable_names(std::move(names)) {}

    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};




// --- SuperMethodCallExpression for SUPER.method() ---
// Move this definition after Expression is fully defined

// --- DeferStatement for DEFER ---
class DeferStatement : public Statement {
public:
    StmtPtr deferred_statement;

    explicit DeferStatement(StmtPtr deferred)
        : Statement(NodeType::DeferStmt), deferred_statement(std::move(deferred)) {}

    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
    NodeType getType() const override { return NodeType::DeferStmt; }
};

// --- ForEachStatement for FOREACH/FFOREACH ---
class ForEachStatement : public Statement {
public:
    std::string loop_variable_name;
    std::string type_variable_name; // NEW: holds the type tag variable, if present (or second destructuring variable)
    ExprPtr collection_expression;
    StmtPtr body;
    VarType filter_type; // INTEGER for FOREACH, FLOAT for FFOREACH
    VarType inferred_element_type = VarType::UNKNOWN; // <-- Added for type-aware FOREACH
    bool is_destructuring = false; // NEW: true for FOREACH (X, Y) IN ... syntax

    ForEachStatement(std::string value_var, std::string type_var, ExprPtr collection, StmtPtr body, VarType type, bool destructuring = false)
        : Statement(NodeType::ForEachStmt), loop_variable_name(std::move(value_var)), type_variable_name(std::move(type_var)),
          collection_expression(std::move(collection)), body(std::move(body)), filter_type(type), is_destructuring(destructuring) {}

    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class LetDeclaration : public Declaration {
public:
    std::vector<std::string> names;
    std::vector<ExprPtr> initializers;
    bool is_float_declaration = false;  // Flag to indicate if this was declared with FLET
    VarType explicit_type = VarType::UNKNOWN; // New: explicit type annotation (AS ...)
    bool is_retained = false; // <-- ADD THIS LINE
    LetDeclaration(std::vector<std::string> names, std::vector<ExprPtr> initializers)
        : Declaration(NodeType::LetDecl), names(std::move(names)), initializers(std::move(initializers)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class ManifestDeclaration : public Declaration {
public:
    std::string name;
    int64_t value;

    ManifestDeclaration(std::string name, int64_t value)
        : Declaration(NodeType::ManifestDecl), name(std::move(name)), value(value) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class StaticDeclaration : public Declaration {
public:
    std::string name;
    ExprPtr initializer;
    bool is_float_declaration = false;

    StaticDeclaration(std::string name, ExprPtr initializer, bool is_float_declaration = false)
        : Declaration(NodeType::StaticDecl), name(std::move(name)), initializer(std::move(initializer)), is_float_declaration(is_float_declaration) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};



class GlobalDeclaration : public Declaration {
public:
    std::vector<std::pair<std::string, int>> globals;

    GlobalDeclaration(std::vector<std::pair<std::string, int>> globals)
        : Declaration(NodeType::GlobalDecl), globals(std::move(globals)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

// --- New: GlobalVariableDeclaration for top-level LET/FLET --- //
class GlobalVariableDeclaration : public Declaration {
public:
    std::vector<std::string> names;
    std::vector<ExprPtr> initializers;
    bool is_float_declaration = false;
    VarType explicit_type = VarType::UNKNOWN; // <-- Added for top-level LET/FLET type annotation

    GlobalVariableDeclaration(std::vector<std::string> names, std::vector<ExprPtr> initializers)
        : Declaration(NodeType::GlobalDecl), names(std::move(names)), initializers(std::move(initializers)) {}

    void accept(ASTVisitor& visitor) override; // To be implemented in AST.cpp
    ASTNodePtr clone() const override;         // To be implemented in AST.cpp
};

// --- New: ClassDeclaration for CLASS --- //
// Struct to pair a member declaration with its visibility
struct ClassMember {
    DeclPtr declaration;
    Visibility visibility;
    
    ClassMember(DeclPtr decl, Visibility vis) 
        : declaration(std::move(decl)), visibility(vis) {}
};

class ClassDeclaration : public Declaration {
public:
    std::string name;
    std::string parent_class_name; // Empty if no explicit parent
    std::vector<ClassMember> members; // Members with their visibility

    ClassDeclaration(std::string name, std::string parent_class_name, std::vector<ClassMember> members)
        : Declaration(NodeType::ClassDecl), name(std::move(name)), parent_class_name(std::move(parent_class_name)), members(std::move(members)) {}

    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class FunctionDeclaration : public Declaration {
public:
    std::string name;
    std::vector<std::string> parameters;
    ExprPtr body;
    bool is_virtual = false;
    bool is_final = false;
    bool is_float_function = false; // True if declared with FLET

    FunctionDeclaration(std::string name, std::vector<std::string> parameters, ExprPtr body, bool is_float = false)
        : Declaration(NodeType::FunctionDecl), name(std::move(name)), parameters(std::move(parameters)), body(std::move(body)), is_float_function(is_float) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class RoutineDeclaration : public Declaration {
public:
    std::string name;
    std::vector<std::string> parameters;
    StmtPtr body;
    bool is_virtual = false;
    bool is_final = false;

    RoutineDeclaration(std::string name, std::vector<std::string> parameters, StmtPtr body)
        : Declaration(NodeType::RoutineDecl), name(std::move(name)), parameters(std::move(parameters)), body(std::move(body)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class LabelDeclaration : public Declaration {
public:
    std::string name;
    StmtPtr command;

    LabelDeclaration(std::string name, StmtPtr command)
        : Declaration(NodeType::LabelDecl), name(std::move(name)), command(std::move(command)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};



// --- Expressions --- //
class Expression : public ASTNode {
public:
    Expression(NodeType type) : ASTNode(type) {}
    virtual ~Expression();
    virtual bool is_literal() const { return false; }
};

// --- ListExpression for LIST syntax ---
class ListExpression : public Expression {
public:
    std::vector<ExprPtr> initializers;
    bool is_manifest = false;
    bool contains_literals = false; // True if this list contains only literal values
    bool is_empty; // Flag for empty collection optimization

    ListExpression(std::vector<ExprPtr> initializers, bool is_manifest = false)
        : Expression(NodeType::ListExpr), initializers(std::move(initializers)), is_manifest(is_manifest), is_empty(this->initializers.empty()) {}

    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

// --- NewExpression for NEW --- //
class NewExpression : public Expression {
public:
    std::string class_name;
    std::vector<ExprPtr> constructor_arguments;
    std::string assigned_variable_name; // NEW: To link object to its variable for DEFER

    NewExpression(std::string class_name, std::vector<ExprPtr> args, std::string var_name = "")
        : Expression(NodeType::NewExpr), class_name(std::move(class_name)),
          constructor_arguments(std::move(args)), assigned_variable_name(std::move(var_name)) {}

    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

// --- SuperMethodCallExpression for SUPER.method() ---
class SuperMethodCallExpression : public Expression {
public:
    std::string member_name;
    std::vector<ExprPtr> arguments;

    SuperMethodCallExpression(const std::string& member, std::vector<ExprPtr>&& args)
        : Expression(NodeType::SuperMethodCallExpr), member_name(member), arguments(std::move(args)) {}

    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
    virtual ~SuperMethodCallExpression();
};

// --- SuperMethodAccessExpression for SUPER.method ---
struct SuperMethodAccessExpression : public Expression {
    std::string member_name;

    SuperMethodAccessExpression(std::string name)
        : Expression(NodeType::SuperMethodAccessExpr), member_name(std::move(name)) {}

    ASTNode::NodeType getType() const override { return ASTNode::NodeType::SuperMethodAccessExpr; }
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
    virtual ~SuperMethodAccessExpression();
};

// --- MemberAccessExpression for . and ! --- //
class MemberAccessExpression : public Expression {
public:
    ExprPtr object_expr;
    std::string member_name;

    MemberAccessExpression(ExprPtr object, std::string member)
        : Expression(NodeType::MemberAccessExpr), object_expr(std::move(object)), member_name(std::move(member)) {}

    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class VecInitializerExpression : public Expression {
public:
    std::vector<ExprPtr> initializers;
    bool is_empty; // Flag for empty collection optimization

    explicit VecInitializerExpression(std::vector<ExprPtr> inits)
        : Expression(NodeType::VecInitializerExpr), initializers(std::move(inits)), is_empty(this->initializers.empty()) {}

    NodeType getType() const override { return NodeType::VecInitializerExpr; }
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class NumberLiteral : public Expression {
public:
    enum class LiteralType { Integer, Float };
    LiteralType literal_type;
    int64_t int_value;
    double float_value;

    NumberLiteral(int64_t value) : Expression(NodeType::NumberLit), literal_type(LiteralType::Integer), int_value(value), float_value(0.0) {}
    NumberLiteral(double value) : Expression(NodeType::NumberLit), literal_type(LiteralType::Float), int_value(0), float_value(value) {}

    bool is_literal() const override { return true; }
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class StringLiteral : public Expression {
public:
    std::string value;
    StringLiteral(std::string value) : Expression(NodeType::StringLit), value(std::move(value)) {}

    bool is_literal() const override { return true; }
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class CharLiteral : public Expression {
public:
    char value;
    CharLiteral(char value) : Expression(NodeType::CharLit), value(value) {}

    bool is_literal() const override { return true; }
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class BooleanLiteral : public Expression {
public:
    bool value;
    BooleanLiteral(bool value) : Expression(NodeType::BooleanLit), value(value) {}

    bool is_literal() const override { return true; }
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class NullLiteral : public Expression {
public:
    NullLiteral() : Expression(NodeType::NullLit) {}

    bool is_literal() const override { return true; }
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class VariableAccess : public Expression {
public:
    std::string name;
    std::string unique_name; // New field for unique name if it's a FOR loop variable
    VariableAccess(std::string name) : Expression(NodeType::VariableAccessExpr), name(std::move(name)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class BinaryOp : public Expression {
public:
    enum class Operator {
        Add, Subtract, Multiply, Divide, Remainder,
        Equal, NotEqual, Less, LessEqual, Greater, GreaterEqual,
        LogicalAnd, BitwiseAnd, LogicalOr, BitwiseOr, Equivalence, NotEquivalence,
        LeftShift, RightShift
    };
    Operator op;
    ExprPtr left;
    ExprPtr right;

    BinaryOp(Operator op, ExprPtr left, ExprPtr right)
        : Expression(NodeType::BinaryOpExpr), op(op), left(std::move(left)), right(std::move(right)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class UnaryOp : public Expression {
public:
    enum class Operator {
        AddressOf, Indirection, LogicalNot, BitwiseNot, Negate, FloatConvert, IntegerConvert, FloatSqrt, FloatFloor, FloatTruncate,
        LengthOf, HeadOf, TailOf, TailOfNonDestructive, HeadOfAsFloat,
        TypeOf,         // TYPEOF(expr) - runtime type tag
        TypeAsString    // TYPE(expr) - compile-time type string macro
    };
    Operator op;
    ExprPtr operand;
    VarType element_type = VarType::UNKNOWN; // For FOREACH context: type of elements in list

    UnaryOp(Operator op, ExprPtr operand)
        : Expression(NodeType::UnaryOpExpr), op(op), operand(std::move(operand)) {}
    
    UnaryOp(Operator op, ExprPtr operand, VarType elem_type)
        : Expression(NodeType::UnaryOpExpr), op(op), operand(std::move(operand)), element_type(elem_type) {}
    
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class VectorAccess : public Expression {
public:
    ExprPtr vector_expr;
    ExprPtr index_expr;
    VectorAccess(ExprPtr vector_expr, ExprPtr index_expr)
        : Expression(NodeType::VectorAccessExpr), vector_expr(std::move(vector_expr)), index_expr(std::move(index_expr)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class CharIndirection : public Expression {
public:
    ExprPtr string_expr;
    ExprPtr index_expr;
    CharIndirection(ExprPtr string_expr, ExprPtr index_expr)
        : Expression(NodeType::CharIndirectionExpr), string_expr(std::move(string_expr)), index_expr(std::move(index_expr)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class FloatVectorIndirection : public Expression {
public:
    ExprPtr vector_expr;
    ExprPtr index_expr;
    FloatVectorIndirection(ExprPtr vector_expr, ExprPtr index_expr)
        : Expression(NodeType::FloatVectorIndirectionExpr), vector_expr(std::move(vector_expr)), index_expr(std::move(index_expr)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

// --- BitfieldAccessExpression --- //
class BitfieldAccessExpression : public Expression {
public:
    ExprPtr base_expr;
    ExprPtr start_bit_expr;
    ExprPtr width_expr;

    BitfieldAccessExpression(ExprPtr base, ExprPtr start, ExprPtr width)
        : Expression(NodeType::BitfieldAccessExpr),
          base_expr(std::move(base)),
          start_bit_expr(std::move(start)),
          width_expr(std::move(width)) {}

    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class FunctionCall : public Expression {
public:
    ExprPtr function_expr;
    std::vector<ExprPtr> arguments;
    // Flag for tail call optimization
    bool is_tail_recursive_call = false;

    // Tail call optimization flag
    bool is_tail_call = false;

    FunctionCall(ExprPtr function_expr, std::vector<ExprPtr> arguments)
        : Expression(NodeType::FunctionCallExpr), function_expr(std::move(function_expr)), arguments(std::move(arguments)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class SysCall : public Expression {
public:
    std::string function_name; // Name of the syscall function
    ExprPtr syscall_number; // Expression for the syscall number (e.g., 0x2000001)
    std::vector<ExprPtr> arguments; // A vector for the syscall arguments (e.g., exit code)

    SysCall(std::string function_name, ExprPtr syscall_number, std::vector<ExprPtr> arguments)
        : Expression(NodeType::SysCallExpr),
          function_name(std::move(function_name)),
          syscall_number(std::move(syscall_number)),
          arguments(std::move(arguments)) {}

    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class ConditionalExpression : public Expression {
public:
    ExprPtr condition;
    ExprPtr true_expr;
    ExprPtr false_expr;
    ConditionalExpression(ExprPtr condition, ExprPtr true_expr, ExprPtr false_expr)
        : Expression(NodeType::ConditionalExpr), condition(std::move(condition)), true_expr(std::move(true_expr)), false_expr(std::move(false_expr)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class ValofExpression : public Expression {
public:
    StmtPtr body;
    VarType explicit_return_type = VarType::UNKNOWN; // For VALOF AS <type>
    ValofExpression(StmtPtr body)
        : Expression(NodeType::ValofExpr), body(std::move(body)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class FloatValofExpression : public Expression {
public:
    StmtPtr body;
    FloatValofExpression(StmtPtr body)
        : Expression(NodeType::FloatValofExpr), body(std::move(body)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class VecAllocationExpression : public Expression {
public:
    ExprPtr size_expr;
    std::string variable_name; // Name of the variable being allocated
    VecAllocationExpression(ExprPtr size_expr)
        : Expression(NodeType::VecAllocationExpr), size_expr(std::move(size_expr)), variable_name("") {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    // Accessor for the variable name
    const std::string& get_variable_name() const { return variable_name; }
};

class StringAllocationExpression : public Expression {
public:
    ExprPtr size_expr;
    StringAllocationExpression(ExprPtr size_expr)
        : Expression(NodeType::StringAllocationExpr), size_expr(std::move(size_expr)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

// --- FVEC Allocation Expression --- //
class FVecAllocationExpression : public Expression {
public:
    ExprPtr size_expr;
    std::string variable_name; // Name of the variable being allocated (optional, for symmetry)
    FVecAllocationExpression(ExprPtr size_expr)
        : Expression(NodeType::FVecAllocationExpr), size_expr(std::move(size_expr)), variable_name("") {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    // Accessor for the variable name
    const std::string& get_variable_name() const { return variable_name; }
};

class TableExpression : public Expression {
public:
    std::vector<ExprPtr> initializers;
    bool is_float_table = false;
    bool is_empty; // Flag for empty collection optimization
    TableExpression(std::vector<ExprPtr> initializers, bool is_float_table = false)
        : Expression(NodeType::TableExpr), initializers(std::move(initializers)), is_float_table(is_float_table), is_empty(this->initializers.empty()) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class PairsAllocationExpression : public Expression {
public:
    ExprPtr size_expr;
    std::string variable_name; // Name of the variable being allocated
    PairsAllocationExpression(ExprPtr size_expr)
        : Expression(NodeType::PairsAllocationExpr), size_expr(std::move(size_expr)), variable_name("") {}
    
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class FPairsAllocationExpression : public Expression {
public:
    ExprPtr size_expr;
    std::string variable_name; // Name of the variable being allocated
    FPairsAllocationExpression(ExprPtr size_expr)
        : Expression(NodeType::FPairsAllocationExpr), size_expr(std::move(size_expr)), variable_name("") {}
    
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class PairExpression : public Expression {
public:
    ExprPtr first_expr;
    ExprPtr second_expr;
    
    PairExpression(ExprPtr first, ExprPtr second) 
        : Expression(NodeType::PairExpr), first_expr(std::move(first)), second_expr(std::move(second)) {}
        
    bool is_literal() const override { 
        return first_expr && second_expr && first_expr->is_literal() && second_expr->is_literal(); 
    }
    
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class PairAccessExpression : public Expression {
public:
    enum AccessType { FIRST, SECOND };
    ExprPtr pair_expr;
    AccessType access_type;
    
    PairAccessExpression(ExprPtr pair, AccessType type)
        : Expression(NodeType::PairAccessExpr), pair_expr(std::move(pair)), access_type(type) {}
        
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class FPairExpression : public Expression {
public:
    ExprPtr first_expr;
    ExprPtr second_expr;
    
    FPairExpression(ExprPtr first, ExprPtr second) 
        : Expression(NodeType::FPairExpr), first_expr(std::move(first)), second_expr(std::move(second)) {}
        
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class QuadExpression : public Expression {
public:
    ExprPtr first_expr;
    ExprPtr second_expr;
    ExprPtr third_expr;
    ExprPtr fourth_expr;
    
    QuadExpression(ExprPtr first, ExprPtr second, ExprPtr third, ExprPtr fourth) 
        : Expression(NodeType::QuadExpr), first_expr(std::move(first)), second_expr(std::move(second)), 
          third_expr(std::move(third)), fourth_expr(std::move(fourth)) {}
        
    bool is_literal() const override { 
        return first_expr && second_expr && third_expr && fourth_expr &&
               first_expr->is_literal() && second_expr->is_literal() && 
               third_expr->is_literal() && fourth_expr->is_literal(); 
    }
        
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class QuadAccessExpression : public Expression {
public:
    enum AccessType { FIRST, SECOND, THIRD, FOURTH };
    
    ExprPtr quad_expr;
    AccessType access_type;
    
    QuadAccessExpression(ExprPtr quad, AccessType type) 
        : Expression(NodeType::QuadAccessExpr), quad_expr(std::move(quad)), access_type(type) {}
        
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class FPairAccessExpression : public Expression {
public:
    enum AccessType { FIRST, SECOND };
    ExprPtr pair_expr;
    AccessType access_type;
    
    FPairAccessExpression(ExprPtr pair, AccessType type)
        : Expression(NodeType::FPairAccessExpr), pair_expr(std::move(pair)), access_type(type) {}
        
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class OctExpression : public Expression {
public:
    ExprPtr first_expr;
    ExprPtr second_expr;
    ExprPtr third_expr;
    ExprPtr fourth_expr;
    ExprPtr fifth_expr;
    ExprPtr sixth_expr;
    ExprPtr seventh_expr;
    ExprPtr eighth_expr;
    
    OctExpression(ExprPtr first, ExprPtr second, ExprPtr third, ExprPtr fourth,
                  ExprPtr fifth, ExprPtr sixth, ExprPtr seventh, ExprPtr eighth) 
        : Expression(NodeType::OctExpr), 
          first_expr(std::move(first)), second_expr(std::move(second)), 
          third_expr(std::move(third)), fourth_expr(std::move(fourth)),
          fifth_expr(std::move(fifth)), sixth_expr(std::move(sixth)),
          seventh_expr(std::move(seventh)), eighth_expr(std::move(eighth)) {}
        
    bool is_literal() const override { 
        return first_expr && second_expr && third_expr && fourth_expr &&
               fifth_expr && sixth_expr && seventh_expr && eighth_expr &&
               first_expr->is_literal() && second_expr->is_literal() && 
               third_expr->is_literal() && fourth_expr->is_literal() &&
               fifth_expr->is_literal() && sixth_expr->is_literal() &&
               seventh_expr->is_literal() && eighth_expr->is_literal(); 
    }
        
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class FOctExpression : public Expression {
public:
    ExprPtr first_expr;
    ExprPtr second_expr;
    ExprPtr third_expr;
    ExprPtr fourth_expr;
    ExprPtr fifth_expr;
    ExprPtr sixth_expr;
    ExprPtr seventh_expr;
    ExprPtr eighth_expr;
    
    FOctExpression(ExprPtr first, ExprPtr second, ExprPtr third, ExprPtr fourth,
                   ExprPtr fifth, ExprPtr sixth, ExprPtr seventh, ExprPtr eighth) 
        : Expression(NodeType::FOctExpr), 
          first_expr(std::move(first)), second_expr(std::move(second)), 
          third_expr(std::move(third)), fourth_expr(std::move(fourth)),
          fifth_expr(std::move(fifth)), sixth_expr(std::move(sixth)),
          seventh_expr(std::move(seventh)), eighth_expr(std::move(eighth)) {}
        
    bool is_literal() const override { 
        return first_expr && second_expr && third_expr && fourth_expr &&
               fifth_expr && sixth_expr && seventh_expr && eighth_expr &&
               first_expr->is_literal() && second_expr->is_literal() && 
               third_expr->is_literal() && fourth_expr->is_literal() &&
               fifth_expr->is_literal() && sixth_expr->is_literal() &&
               seventh_expr->is_literal() && eighth_expr->is_literal(); 
    }
        
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class FQuadExpression : public Expression {
public:
    ExprPtr first_expr;
    ExprPtr second_expr;
    ExprPtr third_expr;
    ExprPtr fourth_expr;
    
    FQuadExpression(ExprPtr first, ExprPtr second, ExprPtr third, ExprPtr fourth) 
        : Expression(NodeType::FQuadExpr), first_expr(std::move(first)), second_expr(std::move(second)), 
          third_expr(std::move(third)), fourth_expr(std::move(fourth)) {}
        
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class FQuadAccessExpression : public Expression {
public:
    enum AccessType { FIRST, SECOND, THIRD, FOURTH };
    
    ExprPtr quad_expr;
    AccessType access_type;
    
    FQuadAccessExpression(ExprPtr quad, AccessType type) 
        : Expression(NodeType::FQuadAccessExpr), quad_expr(std::move(quad)), access_type(type) {}
        
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class LaneAccessExpression : public Expression {
public:
    ExprPtr vector_expr;
    int index;
    
    LaneAccessExpression(ExprPtr vector, int idx) 
        : Expression(NodeType::LaneAccessExpr), vector_expr(std::move(vector)), index(idx) {}
        
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};


// Forward declare
class AssignmentStatement : public Statement {
public:
    std::vector<ExprPtr> lhs;
    std::vector<ExprPtr> rhs;
    std::string statement_role; // Added for metadata/debugging

    AssignmentStatement(std::vector<ExprPtr> lhs, std::vector<ExprPtr> rhs, std::string role = "")
        : Statement(NodeType::AssignmentStmt), lhs(std::move(lhs)), rhs(std::move(rhs)), statement_role(std::move(role)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    // --- Live interval/liveness analysis helpers ---
    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        // Collect variable names from all RHS expressions
        for (const auto& expr : rhs) {
            // For simplicity, only handle VariableAccess for now
            if (expr && expr->getType() == ASTNode::NodeType::VariableAccessExpr) {
                const auto* var = static_cast<const VariableAccess*>(expr.get());
                used.push_back(var->name);
            }
        }
        return used;
    }
};

// --- FreeStatement for FREEVEC and FREELIST ---
class FreeStatement : public Statement {
public:
    ExprPtr list_expr;
    explicit FreeStatement(ExprPtr expr) : Statement(NodeType::FreeStmt), list_expr(std::move(expr)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
    NodeType getType() const override { return NodeType::FreeStmt; }
};

class RoutineCallStatement : public Statement {
public:
    ExprPtr routine_expr;
    std::vector<ExprPtr> arguments;
    RoutineCallStatement(ExprPtr routine_expr, std::vector<ExprPtr> arguments)
        : Statement(NodeType::RoutineCallStmt), routine_expr(std::move(routine_expr)), arguments(std::move(arguments)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        // Routine expression itself might be a variable
        if (routine_expr && routine_expr->getType() == ASTNode::NodeType::VariableAccessExpr) {
            const auto* var = static_cast<const VariableAccess*>(routine_expr.get());
            used.push_back(var->name);
        }
        // Arguments
        for (const auto& arg : arguments) {
            if (arg && arg->getType() == ASTNode::NodeType::VariableAccessExpr) {
                const auto* var = static_cast<const VariableAccess*>(arg.get());
                used.push_back(var->name);
            }
        }
        return used;
    }
};

class IfStatement : public Statement {
public:
    ExprPtr condition;
    StmtPtr then_branch;
    IfStatement(ExprPtr condition, StmtPtr then_branch)
        : Statement(NodeType::IfStmt), condition(std::move(condition)), then_branch(std::move(then_branch)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        if (condition && condition->getType() == ASTNode::NodeType::VariableAccessExpr) {
            const auto* var = static_cast<const VariableAccess*>(condition.get());
            used.push_back(var->name);
        }
        // Optionally, add variables used in then_branch
        if (then_branch) {
            auto branch_vars = then_branch->get_used_variables();
            used.insert(used.end(), branch_vars.begin(), branch_vars.end());
        }
        return used;
    }
};

class UnlessStatement : public Statement {
public:
    ExprPtr condition;
    StmtPtr then_branch;
    UnlessStatement(ExprPtr condition, StmtPtr then_branch)
        : Statement(NodeType::UnlessStmt), condition(std::move(condition)), then_branch(std::move(then_branch)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        if (condition && condition->getType() == ASTNode::NodeType::VariableAccessExpr) {
            const auto* var = static_cast<const VariableAccess*>(condition.get());
            used.push_back(var->name);
        }
        // Optionally, add variables used in then_branch
        if (then_branch) {
            auto branch_vars = then_branch->get_used_variables();
            used.insert(used.end(), branch_vars.begin(), branch_vars.end());
        }
        return used;
    }
};

class TestStatement : public Statement {
public:
    ExprPtr condition;
    StmtPtr then_branch;
    StmtPtr else_branch;
    TestStatement(ExprPtr condition, StmtPtr then_branch, StmtPtr else_branch)
        : Statement(NodeType::TestStmt), condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        if (condition && condition->getType() == ASTNode::NodeType::VariableAccessExpr) {
            const auto* var = static_cast<const VariableAccess*>(condition.get());
            used.push_back(var->name);
        }
        if (then_branch) {
            auto branch_vars = then_branch->get_used_variables();
            used.insert(used.end(), branch_vars.begin(), branch_vars.end());
        }
        if (else_branch) {
            auto else_vars = else_branch->get_used_variables();
            used.insert(used.end(), else_vars.begin(), else_vars.end());
        }
        return used;
    }
};

class WhileStatement : public Statement {
public:
    ExprPtr condition;
    StmtPtr body;
    WhileStatement(ExprPtr condition, StmtPtr body)
        : Statement(NodeType::WhileStmt), condition(std::move(condition)), body(std::move(body)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        if (condition && condition->getType() == ASTNode::NodeType::VariableAccessExpr) {
            const auto* var = static_cast<const VariableAccess*>(condition.get());
            used.push_back(var->name);
        }
        if (body) {
            auto body_vars = body->get_used_variables();
            used.insert(used.end(), body_vars.begin(), body_vars.end());
        }
        return used;
    }
};

class UntilStatement : public Statement {
public:
    ExprPtr condition;
    StmtPtr body;
    UntilStatement(ExprPtr condition, StmtPtr body)
        : Statement(NodeType::UntilStmt), condition(std::move(condition)), body(std::move(body)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        if (condition && condition->getType() == ASTNode::NodeType::VariableAccessExpr) {
            const auto* var = static_cast<const VariableAccess*>(condition.get());
            used.push_back(var->name);
        }
        if (body) {
            auto body_vars = body->get_used_variables();
            used.insert(used.end(), body_vars.begin(), body_vars.end());
        }
        return used;
    }
};

class RepeatStatement : public Statement {
public:
    enum class LoopType { Repeat, RepeatWhile, RepeatUntil };
    LoopType loop_type;
    StmtPtr body;
    ExprPtr condition;
    RepeatStatement(LoopType type, StmtPtr body, ExprPtr condition = nullptr)
        : Statement(NodeType::RepeatStmt), loop_type(type), body(std::move(body)), condition(std::move(condition)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        if (body) {
            auto body_vars = body->get_used_variables();
            used.insert(used.end(), body_vars.begin(), body_vars.end());
        }
        if (condition && condition->getType() == ASTNode::NodeType::VariableAccessExpr) {
            const auto* var = static_cast<const VariableAccess*>(condition.get());
            used.push_back(var->name);
        }
        return used;
    }
};

class ForStatement : public Statement {
public:
    std::string loop_variable;
    std::string unique_loop_variable_name; // New field
    std::string unique_step_variable_name;      // To hold the name for the step value's slot
    std::string unique_end_variable_name;       // To hold the name for the (optional) hoisted end value
    ExprPtr start_expr;
    ExprPtr end_expr;
    ExprPtr step_expr;
    StmtPtr body;

    // Constant analysis fields for register optimization
    bool is_end_expr_constant = false;
    int64_t constant_end_value = 0;
    bool is_step_expr_constant = false;
    int64_t constant_step_value = 1;  // Default step is 1

    ForStatement(std::string loop_variable, ExprPtr start_expr, ExprPtr end_expr, StmtPtr body, ExprPtr step_expr = nullptr)
        : Statement(NodeType::ForStmt), loop_variable(std::move(loop_variable)), start_expr(std::move(start_expr)),
          end_expr(std::move(end_expr)), body(std::move(body)), step_expr(std::move(step_expr)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        if (start_expr && start_expr->getType() == ASTNode::NodeType::VariableAccessExpr) {
            const auto* var = static_cast<const VariableAccess*>(start_expr.get());
            used.push_back(var->name);
        }
        if (end_expr && end_expr->getType() == ASTNode::NodeType::VariableAccessExpr) {
            const auto* var = static_cast<const VariableAccess*>(end_expr.get());
            used.push_back(var->name);
        }
        if (step_expr && step_expr->getType() == ASTNode::NodeType::VariableAccessExpr) {
            const auto* var = static_cast<const VariableAccess*>(step_expr.get());
            used.push_back(var->name);
        }
        if (body) {
            auto body_vars = body->get_used_variables();
            used.insert(used.end(), body_vars.begin(), body_vars.end());
        }
        return used;
    }

    std::vector<std::string> get_defined_variables() const override {
        // The loop variable is defined by the for statement
        return {loop_variable};
    }
};

// === Reduction Statements ===

class MinStatement : public Statement {
public:
    std::string result_variable;
    ExprPtr left_operand;
    ExprPtr right_operand;
    
    MinStatement(std::string result_var, ExprPtr left, ExprPtr right)
        : Statement(NodeType::MinStmt), result_variable(std::move(result_var)), 
          left_operand(std::move(left)), right_operand(std::move(right)) {}
    
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
    
    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        if (auto* var = dynamic_cast<VariableAccess*>(left_operand.get())) {
            used.push_back(var->name);
        }
        if (auto* var = dynamic_cast<VariableAccess*>(right_operand.get())) {
            used.push_back(var->name);
        }
        return used;
    }
    
    std::vector<std::string> get_defined_variables() const override {
        return {result_variable};
    }
};

class MaxStatement : public Statement {
public:
    std::string result_variable;
    ExprPtr left_operand;
    ExprPtr right_operand;
    
    MaxStatement(std::string result_var, ExprPtr left, ExprPtr right)
        : Statement(NodeType::MaxStmt), result_variable(std::move(result_var)), 
          left_operand(std::move(left)), right_operand(std::move(right)) {}
    
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
    
    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        if (auto* var = dynamic_cast<VariableAccess*>(left_operand.get())) {
            used.push_back(var->name);
        }
        if (auto* var = dynamic_cast<VariableAccess*>(right_operand.get())) {
            used.push_back(var->name);
        }
        return used;
    }
    
    std::vector<std::string> get_defined_variables() const override {
        return {result_variable};
    }
};

class SumStatement : public Statement {
public:
    std::string result_variable;
    ExprPtr left_operand;
    ExprPtr right_operand;
    
    SumStatement(std::string result_var, ExprPtr left, ExprPtr right)
        : Statement(NodeType::SumStmt), result_variable(std::move(result_var)), 
          left_operand(std::move(left)), right_operand(std::move(right)) {}
    
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
    
    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        if (auto* var = dynamic_cast<VariableAccess*>(left_operand.get())) {
            used.push_back(var->name);
        }
        if (auto* var = dynamic_cast<VariableAccess*>(right_operand.get())) {
            used.push_back(var->name);
        }
        return used;
    }
    
    std::vector<std::string> get_defined_variables() const override {
        return {result_variable};
    }
};

// === ReductionLoopStatement - CFG Metadata ===

class ReductionStatement : public Statement {
public:
    std::unique_ptr<Reducer> reducer;
    std::string result_variable;
    ExprPtr left_operand;
    ExprPtr right_operand;
    
    // Constructor declared here, implemented in AST.cpp to avoid incomplete type issues
    ReductionStatement(std::unique_ptr<Reducer> reducer, std::string result_var, ExprPtr left, ExprPtr right);
    
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
    
    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        if (left_operand && left_operand->getType() == ASTNode::NodeType::VariableAccessExpr) {
            if (auto* var = dynamic_cast<VariableAccess*>(left_operand.get())) {
                used.push_back(var->name);
            }
        }
        if (right_operand && right_operand->getType() == ASTNode::NodeType::VariableAccessExpr) {
            if (auto* var = dynamic_cast<VariableAccess*>(right_operand.get())) {
                used.push_back(var->name);
            }
        }
        return used;
    }
};

class ReductionLoopStatement : public Statement {
public:
    std::string left_temp;
    std::string right_temp;
    std::string result_temp;
    std::string index_name;
    std::string chunks_name;
    std::string result_variable;
    int reduction_op; // ReductionOp enum value
    
    ReductionLoopStatement(std::string left, std::string right, std::string result,
                          std::string index, std::string chunks, std::string result_var, int op)
        : Statement(NodeType::ReductionLoopStmt), left_temp(std::move(left)), 
          right_temp(std::move(right)), result_temp(std::move(result)),
          index_name(std::move(index)), chunks_name(std::move(chunks)),
          result_variable(std::move(result_var)), reduction_op(op) {}
    
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class PairwiseReductionLoopStatement : public Statement {
public:
    std::string vector_a_name;
    std::string vector_b_name;
    std::string result_vector_name;
    std::string intrinsic_name;  // NEON intrinsic to use (e.g., "llvm.arm.neon.vpmin")
    int reduction_op; // Operation code for type identification
    
    PairwiseReductionLoopStatement(std::string vector_a, std::string vector_b, 
                                  std::string result_vec, std::string intrinsic, int op)
        : Statement(NodeType::PairwiseReductionLoopStmt), 
          vector_a_name(std::move(vector_a)), vector_b_name(std::move(vector_b)),
          result_vector_name(std::move(result_vec)), intrinsic_name(std::move(intrinsic)),
          reduction_op(op) {}
    
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
    
    std::vector<std::string> get_used_variables() const override {
        return {vector_a_name, vector_b_name};
    }
};

#include <optional> // Add this include for std::optional

class CaseStatement : public Statement {
public:
    ExprPtr constant_expr;
    StmtPtr command;
    // NEW: Store the resolved constant value here after analysis
    OptionalValue<int64_t> resolved_constant_value;

    CaseStatement(ExprPtr constant_expr, StmtPtr command)
        : Statement(NodeType::CaseStmt), constant_expr(std::move(constant_expr)), command(std::move(command)), resolved_constant_value() {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        if (constant_expr && constant_expr->getType() == ASTNode::NodeType::VariableAccessExpr) {
            const auto* var = static_cast<const VariableAccess*>(constant_expr.get());
            used.push_back(var->name);
        }
        if (command) {
            auto cmd_vars = command->get_used_variables();
            used.insert(used.end(), cmd_vars.begin(), cmd_vars.end());
        }
        return used;
    }
};

class DefaultStatement : public Statement {
public:
    StmtPtr command;
    DefaultStatement(StmtPtr command)
        : Statement(NodeType::DefaultStmt), command(std::move(command)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        if (command) {
            auto cmd_vars = command->get_used_variables();
            used.insert(used.end(), cmd_vars.begin(), cmd_vars.end());
        }
        return used;
    }
};

class SwitchonStatement : public Statement {
public:
    ExprPtr expression;

    std::vector<std::unique_ptr<CaseStatement>> cases;
    std::unique_ptr<DefaultStatement> default_case;

    SwitchonStatement(ExprPtr expression, std::vector<std::unique_ptr<CaseStatement>> cases, std::unique_ptr<DefaultStatement> default_case = nullptr)
        : Statement(NodeType::SwitchonStmt), expression(std::move(expression)), cases(std::move(cases)), default_case(std::move(default_case)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        if (expression && expression->getType() == ASTNode::NodeType::VariableAccessExpr) {
            const auto* var = static_cast<const VariableAccess*>(expression.get());
            used.push_back(var->name);
        }
        for (const auto& case_stmt : cases) {
            auto case_vars = case_stmt->get_used_variables();
            used.insert(used.end(), case_vars.begin(), case_vars.end());
        }
        if (default_case) {
            auto default_vars = default_case->get_used_variables();
            used.insert(used.end(), default_vars.begin(), default_vars.end());
        }
        return used;
    }
};

class GotoStatement : public Statement {
public:
    ExprPtr label_expr;
    GotoStatement(ExprPtr label_expr) : Statement(NodeType::GotoStmt), label_expr(std::move(label_expr)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement() : Statement(NodeType::ReturnStmt) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class FinishStatement : public Statement {
public:
    ExprPtr syscall_number; // Expression for the syscall number (e.g., 0x2000001)
    std::vector<ExprPtr> arguments; // A vector for the syscall arguments (e.g., exit code)

    FinishStatement()
        : Statement(NodeType::FinishStmt),
          syscall_number(nullptr),
          arguments() {}

    FinishStatement(ExprPtr syscall_number, std::vector<ExprPtr> arguments)
        : Statement(NodeType::FinishStmt),
          syscall_number(std::move(syscall_number)),
          arguments(std::move(arguments)) {}

    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class BreakStatement : public Statement {
public:
    BreakStatement() : Statement(NodeType::BreakStmt) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class BrkStatement : public Statement {
public:
    BrkStatement() : Statement(NodeType::BrkStatement) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class LoopStatement : public Statement {
public:
    LoopStatement() : Statement(NodeType::LoopStmt) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class EndcaseStatement : public Statement {
public:
    EndcaseStatement() : Statement(NodeType::EndcaseStmt) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class ResultisStatement : public Statement {
public:
    ExprPtr expression;
    bool is_send = false; // Flag to indicate if this came from SEND syntax
    ResultisStatement(ExprPtr expression, bool is_send = false) : Statement(NodeType::ResultisStmt), expression(std::move(expression)), is_send(is_send) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        if (expression && expression->getType() == ASTNode::NodeType::VariableAccessExpr) {
            const auto* var = static_cast<const VariableAccess*>(expression.get());
            used.push_back(var->name);
        }
        return used;
    }
};

class CompoundStatement : public Statement {
public:
    std::vector<StmtPtr> statements;
    CompoundStatement(std::vector<StmtPtr> statements) : Statement(NodeType::CompoundStmt), statements(std::move(statements)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        for (const auto& stmt : statements) {
            auto stmt_vars = stmt->get_used_variables();
            used.insert(used.end(), stmt_vars.begin(), stmt_vars.end());
        }
        return used;
    }

    std::vector<std::string> get_defined_variables() const override {
        std::vector<std::string> defs;
        for (const auto& stmt : statements) {
            auto stmt_defs = stmt->get_defined_variables();
            defs.insert(defs.end(), stmt_defs.begin(), stmt_defs.end());
        }
        return defs;
    }
};

class BlockStatement : public Statement {
public:
    std::vector<DeclPtr> declarations;
    std::vector<StmtPtr> statements;
    BlockStatement(std::vector<DeclPtr> declarations, std::vector<StmtPtr> statements)
        : Statement(NodeType::BlockStmt), declarations(std::move(declarations)), statements(std::move(statements)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;

    std::vector<std::string> get_used_variables() const override {
        std::vector<std::string> used;
        for (const auto& stmt : statements) {
            auto stmt_vars = stmt->get_used_variables();
            used.insert(used.end(), stmt_vars.begin(), stmt_vars.end());
        }
        return used;
    }

    std::vector<std::string> get_defined_variables() const override {
        std::vector<std::string> defs;
        for (const auto& stmt : statements) {
            auto stmt_defs = stmt->get_defined_variables();
            defs.insert(defs.end(), stmt_defs.begin(), stmt_defs.end());
        }
        return defs;
    }
};

class StringStatement : public Statement {
public:
    ExprPtr size_expr;
    StringStatement(ExprPtr size_expr) : Statement(NodeType::StringStmt), size_expr(std::move(size_expr)) {}
    void accept(ASTVisitor& visitor) override;
    ASTNodePtr clone() const override;
};

class LabelTargetStatement : public Statement {
public:
    std::string labelName;

    // Change Statement() to Statement(NodeType::LabelTargetStmt)
    LabelTargetStatement(std::string name)
        : Statement(NodeType::LabelTargetStmt), labelName(std::move(name)) {}


    void accept(ASTVisitor& visitor) override;
    ASTNode::NodeType getType() const override { return ASTNode::NodeType::LabelTargetStmt; }
    ASTNodePtr clone() const override;
};

class ConditionalBranchStatement : public Statement {
public:
    std::string condition;
    std::string targetLabel;
    ExprPtr condition_expr; // Changed from string

    // Updated constructor to take ExprPtr for condition_expr
    ConditionalBranchStatement(std::string cond, std::string label, ExprPtr cond_expr)
        : Statement(NodeType::ConditionalBranchStmt), condition(std::move(cond)), targetLabel(std::move(label)), condition_expr(std::move(cond_expr)) {}

    void accept(ASTVisitor& visitor) override;
    ASTNode::NodeType getType() const override { return ASTNode::NodeType::ConditionalBranchStmt; }
    ASTNodePtr clone() const override;
};

#endif // AST_H
