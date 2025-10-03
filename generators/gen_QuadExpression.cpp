#include "../NewCodeGenerator.h"
#include "../AST.h"
#include "../RegisterManager.h"
#include "../InstructionStream.h"
#include "../Encoder.h"
#include <iostream>

void NewCodeGenerator::visit(QuadExpression& node) {
    debug_print("Generating code for QuadExpression using ARM64 bit field instructions");
    
    // Check if this is a literal quad (all expressions are literals)
    if (node.is_literal()) {
        debug_print("QuadExpression is literal - loading from rodata");
        
        // Extract literal values
        int64_t first_val = 0, second_val = 0, third_val = 0, fourth_val = 0;
        if (auto* first_lit = dynamic_cast<NumberLiteral*>(node.first_expr.get())) {
            first_val = first_lit->int_value;
        }
        if (auto* second_lit = dynamic_cast<NumberLiteral*>(node.second_expr.get())) {
            second_val = second_lit->int_value;
        }
        if (auto* third_lit = dynamic_cast<NumberLiteral*>(node.third_expr.get())) {
            third_val = third_lit->int_value;
        }
        if (auto* fourth_lit = dynamic_cast<NumberLiteral*>(node.fourth_expr.get())) {
            fourth_val = fourth_lit->int_value;
        }
        
        // Register the quad literal in rodata
        std::string quad_label = data_generator_.add_quad_literal(first_val, second_val, third_val, fourth_val);
        
        // Allocate register and load the literal from rodata
        auto& register_manager = RegisterManager::getInstance();
        std::string result_reg = register_manager.get_free_register(*this);
        
        // Load the quad literal value
        emit(Encoder::create_adrp(result_reg, quad_label));
        emit(Encoder::create_add_literal(result_reg, result_reg, quad_label));
        emit(Encoder::create_ldr_imm(result_reg, result_reg, 0));
        
        expression_result_reg_ = result_reg;
        debug_print("Loaded literal QUAD from rodata label: " + quad_label);
        return;
    }
    
    // Non-literal path: A QUAD combines four 16-bit values into a single 64-bit word
    // First expression goes into bits 0-15 (lower 16 bits)
    // Second expression goes into bits 16-31
    // Third expression goes into bits 32-47
    // Fourth expression goes into bits 48-63 (upper 16 bits)
    
    auto& register_manager = RegisterManager::getInstance();
    std::string result_reg = register_manager.get_free_register(*this);
    
    // Initialize result register to 0
    emit(Encoder::create_movz_imm(result_reg, 0));
    
    // Generate code for first expression (bits 0-15)
    if (node.first_expr) {
        node.first_expr->accept(*this);
        std::string first_reg = expression_result_reg_;
        
        // Use BFI to insert the lower 16 bits of first_reg into bits 0-15 of result
        // BFI will take the lower 16 bits from first_reg (treating it as signed 16-bit value)
        emit(Encoder::opt_create_bfi(result_reg, first_reg, 0, 16));
        
        register_manager.release_register(first_reg);
        debug_print("Inserted first expression into bits 0-15 using BFI");
    }
    
    // Generate code for second expression (bits 16-31)
    if (node.second_expr) {
        node.second_expr->accept(*this);
        std::string second_reg = expression_result_reg_;
        
        // Use BFI to insert the lower 16 bits of second_reg into bits 16-31 of result
        emit(Encoder::opt_create_bfi(result_reg, second_reg, 16, 16));
        
        register_manager.release_register(second_reg);
        debug_print("Inserted second expression into bits 16-31 using BFI");
    }
    
    // Generate code for third expression (bits 32-47)
    if (node.third_expr) {
        node.third_expr->accept(*this);
        std::string third_reg = expression_result_reg_;
        
        // Use BFI to insert the lower 16 bits of third_reg into bits 32-47 of result
        emit(Encoder::opt_create_bfi(result_reg, third_reg, 32, 16));
        
        register_manager.release_register(third_reg);
        debug_print("Inserted third expression into bits 32-47 using BFI");
    }
    
    // Generate code for fourth expression (bits 48-63)
    if (node.fourth_expr) {
        node.fourth_expr->accept(*this);
        std::string fourth_reg = expression_result_reg_;
        
        // Use BFI to insert the lower 16 bits of fourth_reg into bits 48-63 of result
        emit(Encoder::opt_create_bfi(result_reg, fourth_reg, 48, 16));
        
        register_manager.release_register(fourth_reg);
        debug_print("Inserted fourth expression into bits 48-63 using BFI");
    }
    
    // The combined 64-bit value is our result
    expression_result_reg_ = result_reg;
    
    debug_print("QuadExpression code generation complete - used ARM64 bit field instructions");
}