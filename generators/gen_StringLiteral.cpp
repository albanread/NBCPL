#include "NewCodeGenerator.h"
#include "AST.h"
#include "Encoder.h"
#include "LabelManager.h"
#include "RegisterManager.h"

void NewCodeGenerator::visit(StringLiteral& node) {
    debug_print("Generating code for StringLiteral.");

    // 1. Register the string literal with the DataGenerator.
    std::string string_label = data_generator_.add_string_literal(node.value);

    // 2. Allocate a register to hold the address of the string.
    expression_result_reg_ = register_manager_.get_free_register(*this);
    debug_print("Allocated register " + expression_result_reg_ + " for the string address.");

    // 3. Emit instructions to load the base address of the string data block.
    // Use the optimized version that combines label + offset in a single ADD instruction.
    emit(Encoder::create_adrp(expression_result_reg_, string_label));
    emit(Encoder::create_add_literal_with_offset(expression_result_reg_, expression_result_reg_, string_label, 8));

    debug_print("Emitted optimized ADRP/ADD+offset sequence for string literal '" + node.value + "'.");
}
