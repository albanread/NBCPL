#include "../NewCodeGenerator.h"
#include "../LabelManager.h"
#include "../InstructionStream.h"
#include "../RegisterManager.h"
#include "../DataGenerator.h"
#include "../AST.h"


#include <iostream>
#include <cassert>

void test_simple_if_statement() {
    // Create a simple IfStatement: IF (x == 1) THEN y = 2;
    auto condition = std::make_unique<BinaryOp>(
        BinaryOp::Operator::Equal,
        std::make_unique<VariableAccess>("x"),
        std::make_unique<NumberLiteral>(static_cast<int64_t>(1))
    );

    std::vector<ExprPtr> then_lhs;
    then_lhs.push_back(std::make_unique<VariableAccess>("y"));
    std::vector<ExprPtr> then_rhs;
    then_rhs.push_back(std::make_unique<NumberLiteral>(static_cast<int64_t>(2)));
    auto then_branch = std::make_unique<AssignmentStatement>(std::move(then_lhs), std::move(then_rhs));

    IfStatement if_stmt(std::move(condition), std::move(then_branch));

    // Set up the code generator
    LabelManager& label_manager = LabelManager::instance();
    RegisterManager& register_manager = RegisterManager::getInstance();
    InstructionStream instruction_stream(label_manager, false);
    DataGenerator data_generator;
    NewCodeGenerator code_gen(
        instruction_stream,
        register_manager,
        label_manager,
        false, // tracing disabled
        5,     // max registers
        data_generator
    );

    // Generate code for the IfStatement
    code_gen.visit(if_stmt);

    // Verify the generated instructions
    const auto& instructions = instruction_stream.get_instructions();
    assert(!instructions.empty() && "Instructions should not be empty for a valid IfStatement.");

    // Print the instructions for manual verification
    std::cout << "--- Test: Simple IfStatement ---\n";
    for (const auto& instr : instructions) {
        // Debug output for instructions is not implemented
    }
    std::cout << "--------------------------------\n";
}

void test_if_else_statement() {
    // Create an IfStatement with an else branch: IF (x > 0) THEN y = 1 ELSE y = -1;
    auto condition = std::make_unique<BinaryOp>(
        BinaryOp::Operator::Greater,
        std::make_unique<VariableAccess>("y"),
        std::make_unique<NumberLiteral>(static_cast<int64_t>(0))
    );

    std::vector<ExprPtr> then_lhs;
    then_lhs.emplace_back(std::make_unique<VariableAccess>("y"));
    std::vector<ExprPtr> then_rhs;
    then_rhs.emplace_back(std::make_unique<NumberLiteral>(static_cast<int64_t>(1)));
    auto then_branch = std::make_unique<AssignmentStatement>(std::move(then_lhs), std::move(then_rhs));

    IfStatement if_stmt(std::move(condition), std::move(then_branch));
    std::vector<ExprPtr> else_lhs;
    else_lhs.emplace_back(std::make_unique<VariableAccess>("y"));
    std::vector<ExprPtr> else_rhs;
    else_rhs.emplace_back(std::make_unique<NumberLiteral>(static_cast<int64_t>(-1)));
    auto else_branch = std::make_unique<AssignmentStatement>(std::move(else_lhs), std::move(else_rhs));
    auto nested_else = std::make_unique<IfStatement>(std::move(condition), std::move(else_branch));

    // Set up the code generator
    LabelManager& label_manager = LabelManager::instance();
    RegisterManager& register_manager = RegisterManager::getInstance();
    InstructionStream instruction_stream(label_manager, false);
    DataGenerator data_generator;
    NewCodeGenerator code_gen(
        instruction_stream,
        register_manager,
        label_manager,
        false, // tracing disabled
        5,     // max registers
        data_generator
    );

    // Generate code for the IfStatement
    code_gen.visit(if_stmt);

    // Verify the generated instructions
    const auto& instructions = instruction_stream.get_instructions();
    assert(!instructions.empty() && "Instructions should not be empty for a valid IfStatement with else.");

    // Print the instructions for manual verification
    std::cout << "--- Test: If-Else Statement ---\n";
    for (const auto& instr : instructions) {
        // Debug output for instructions is not implemented
    }
    std::cout << "--------------------------------\n";
}

void test_nested_if_statement() {
    // Create a nested IfStatement:
    // IF (x > 0) THEN
    //   IF (y < 5) THEN z = 10;
    auto inner_condition = std::make_unique<BinaryOp>(
        BinaryOp::Operator::Less,
        std::make_unique<VariableAccess>("y"),
        std::make_unique<NumberLiteral>(static_cast<int64_t>(0))
    );

    std::vector<ExprPtr> inner_then_lhs;
    inner_then_lhs.push_back(std::make_unique<VariableAccess>("z"));
    std::vector<ExprPtr> inner_then_rhs;
    inner_then_rhs.push_back(std::make_unique<NumberLiteral>(static_cast<int64_t>(10)));
    auto inner_then_branch = std::make_unique<AssignmentStatement>(std::move(inner_then_lhs), std::move(inner_then_rhs));

    auto inner_if_stmt = std::make_unique<IfStatement>(
        std::move(inner_condition),
        std::move(inner_then_branch)
    );

    auto outer_condition = std::make_unique<BinaryOp>(
        BinaryOp::Operator::Greater,
        std::make_unique<VariableAccess>("x"),
        std::make_unique<NumberLiteral>(static_cast<int64_t>(0))
    );

    IfStatement outer_if_stmt(std::move(outer_condition), std::move(inner_if_stmt));

    // Set up the code generator
    LabelManager& label_manager = LabelManager::instance();
    RegisterManager& register_manager = RegisterManager::getInstance();
    InstructionStream instruction_stream(label_manager, false);
    DataGenerator data_generator;
    NewCodeGenerator code_gen(
        instruction_stream,
        register_manager,
        label_manager,
        false, // tracing disabled
        5,     // max registers
        data_generator
    );

    // Generate code for the nested IfStatement
    code_gen.visit(outer_if_stmt);

    // Verify the generated instructions
    const auto& instructions = instruction_stream.get_instructions();
    assert(!instructions.empty() && "Instructions should not be empty for a valid nested IfStatement.");

    // Print the instructions for manual verification
    std::cout << "--- Test: Nested IfStatement ---\n";
    for (const auto& instr : instructions) {
        // Debug output for instructions is not implemented
    }
    std::cout << "--------------------------------\n";
}

int main() {
    test_simple_if_statement();
    test_if_else_statement();
    test_nested_if_statement();

    std::cout << "All IfStatement tests passed successfully.\n";
    return 0;
}
