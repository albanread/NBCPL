#include "NewCodeGenerator.h"
#include "LabelManager.h"
#include "analysis/ASTAnalyzer.h"
#include "NewCodeGenerator.h"
#include <iostream>
#include <stdexcept>

void NewCodeGenerator::visit(NullLiteral& node) {
    debug_print("Visiting NullLiteral node.");
    auto& register_manager = register_manager_;
    std::string dest_reg = register_manager.get_free_register(*this); // Get a free temporary register

    // Null literal compiles to 0 (null pointer)
    emit(Encoder::create_movz_imm(dest_reg, 0, 0));
    expression_result_reg_ = dest_reg;
    debug_print("Loaded null literal (?) into " + dest_reg + ".");
}