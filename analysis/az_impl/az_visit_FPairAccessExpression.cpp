#include "../../AST.h"
#include "../ASTAnalyzer.h"
#include "../../DataTypes.h"
#include "../../SymbolTable.h"
#include <iostream>

void ASTAnalyzer::visit(FPairAccessExpression& node) {
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] Visiting FPairAccessExpression ("
                  << (node.access_type == FPairAccessExpression::FIRST ? "first" : "second") 
                  << ")" << std::endl;
    }

    // Analyze the underlying fpair expression
    if (node.pair_expr) {
        node.pair_expr->accept(*this);
    } else {
        semantic_errors_.push_back("FPairAccessExpression: missing fpair expression");
        return;
    }

    // Verify that the expression being accessed is actually an FPAIR
    VarType pair_type = infer_expression_type(node.pair_expr.get());
    if (pair_type != VarType::FPAIR) {
        semantic_errors_.push_back("FPairAccessExpression: .first/.second can only be used on FPAIR expressions");
    }

    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] FPairAccessExpression analysis complete" << std::endl;
    }
}