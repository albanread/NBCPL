#include "../../AST.h"
#include "../ASTAnalyzer.h"
#include "../../DataTypes.h"
#include "../../SymbolTable.h"
#include <iostream>

void ASTAnalyzer::visit(PairAccessExpression& node) {
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] Visiting PairAccessExpression ("
                  << (node.access_type == PairAccessExpression::FIRST ? "first" : "second") 
                  << ")" << std::endl;
    }

    // Analyze the pair expression being accessed
    if (node.pair_expr) {
        node.pair_expr->accept(*this);
    } else {
        semantic_errors_.push_back("PairAccessExpression: missing pair expression");
        return;
    }

    // Validate that we're accessing a valid pair
    // In a more sophisticated type system, we would check that pair_expr
    // actually evaluates to a pair type, but for now we assume it's valid
    // if it parsed correctly

    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] PairAccessExpression analysis complete" << std::endl;
    }
}