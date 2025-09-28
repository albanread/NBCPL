#include "../../AST.h"
#include "../ASTAnalyzer.h"
#include "../../DataTypes.h"
#include "../../SymbolTable.h"
#include <iostream>

void ASTAnalyzer::visit(FPairExpression& node) {
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] Visiting FPairExpression" << std::endl;
    }

    // Analyze both expressions in the fpair
    if (node.first_expr) {
        node.first_expr->accept(*this);
    }
    if (node.second_expr) {
        node.second_expr->accept(*this);
    }

    // FPAIR expressions are always valid if their components are valid
    // The runtime representation will be two consecutive 32-bit floats packed in a 64-bit word
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] FPairExpression analysis complete" << std::endl;
    }
}