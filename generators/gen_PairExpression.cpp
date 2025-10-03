#include "../NewCodeGenerator.h"
#include "../AST.h"
#include "../RegisterManager.h"
#include "../InstructionStream.h"
#include "../Encoder.h"
#include <iostream>

void NewCodeGenerator::visit(PairExpression& node) {
    debug_print("Generating code for PairExpression using ARM64 bit field instructions");
    
    // Check if this is a literal pair (both expressions are literals)
    if (node.is_literal()) {
        debug_print("PairExpression is literal - loading from rodata");
        
        // Extract literal values
        int64_t first_val = 0, second_val = 0;
        if (auto* first_lit = dynamic_cast<NumberLiteral*>(node.first_expr.get())) {
            first_val = first_lit->int_value;
        }
        if (auto* second_lit = dynamic_cast<NumberLiteral*>(node.second_expr.get())) {
            second_val = second_lit->int_value;
        }
        
        // Register the pair literal in rodata
        std::string pair_label = data_generator_.add_pair_literal(first_val, second_val);
        
        // Allocate register and load the literal from rodata
        auto& register_manager = RegisterManager::getInstance();
        std::string result_reg = register_manager.get_free_register(*this);
        
        // Load the pair literal value
        emit(Encoder::create_adrp(result_reg, pair_label));
        emit(Encoder::create_add_literal(result_reg, result_reg, pair_label));
        emit(Encoder::create_ldr_imm(result_reg, result_reg, 0));
        
        expression_result_reg_ = result_reg;
        debug_print("Loaded literal PAIR from rodata label: " + pair_label);
        return;
    }
    
    // Non-literal path: A PAIR combines two 32-bit values into a single 64-bit word
    // First expression goes into bits 0-31 (lower 32 bits)
    // Second expression goes into bits 32-63 (upper 32 bits)
    
    auto& register_manager = RegisterManager::getInstance();
    std::string result_reg = register_manager.get_free_register(*this);
    
    // Initialize result register to 0
    emit(Encoder::create_movz_imm(result_reg, 0));
    
    // Generate code for first expression (bits 0-31)
    if (node.first_expr) {
        node.first_expr->accept(*this);
        std::string first_reg = expression_result_reg_;
        
        // Use BFI to insert the lower 32 bits of first_reg into bits 0-31 of result
        // BFI will take the lower 32 bits from first_reg (treating it as signed 32-bit value)
        emit(Encoder::opt_create_bfi(result_reg, first_reg, 0, 32));
        
        register_manager.release_register(first_reg);
        debug_print("Inserted lower 32 bits into bits 0-31 using BFI");
    }
    
    // Generate code for second expression (bits 32-63)
    if (node.second_expr) {
        node.second_expr->accept(*this);
        std::string second_reg = expression_result_reg_;
        
        // Use BFI to insert the lower 32 bits of second_reg into bits 32-63 of result
        // BFI will take the lower 32 bits from second_reg (treating it as signed 32-bit value)
        emit(Encoder::opt_create_bfi(result_reg, second_reg, 32, 32));
        
        register_manager.release_register(second_reg);
        debug_print("Inserted lower 32 bits into bits 32-63 using BFI");
    }
    
    // The combined 64-bit value is our result
    expression_result_reg_ = result_reg;
    
    debug_print("PairExpression code generation complete - used ARM64 bit field instructions");
}