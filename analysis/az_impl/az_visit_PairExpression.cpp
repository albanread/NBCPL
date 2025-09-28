#include "../../AST.h"
#include "../ASTAnalyzer.h"
#include "../../DataTypes.h"
#include "../../SymbolTable.h"
#include <iostream>

void ASTAnalyzer::visit(PairExpression& node) {
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] Visiting PairExpression" << std::endl;
    }

    // Analyze both expressions in the pair
    if (node.first_expr) {
        node.first_expr->accept(*this);
    }
    if (node.second_expr) {
        node.second_expr->accept(*this);
    }

    // PAIR expressions are always valid if their components are valid
    // The runtime representation will be two consecutive 64-bit values
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] PairExpression analysis complete" << std::endl;
    }
}