#include "NewCodeGenerator.h"
#include "ClassTable.h"
#include "Encoder.h"
#include <stdexcept>

void NewCodeGenerator::generate_virtual_method_call(const ClassMethodInfo& method_info) {
    debug_print("Generating virtual method call for: " + method_info.name);
    
    if (is_jit_mode_) {
        generate_jit_virtual_call(method_info);
    } else {
        generate_aot_virtual_call(method_info);
    }
}

void NewCodeGenerator::generate_direct_method_call(const ClassMethodInfo& method_info) {
    debug_print("Generating direct method call for: " + method_info.qualified_name);
    
    // Generate direct address to non-virtual method
    std::string method_reg = register_manager_.acquire_scratch_reg(*this);
    emit(Encoder::create_adrp(method_reg, method_info.qualified_name));
    emit(Encoder::create_add_literal(method_reg, method_reg, method_info.qualified_name));
    
    // The result of this expression is the callable address of the method
    expression_result_reg_ = method_reg;
    
    debug_print("Direct method call address loaded into: " + method_reg);
}

void NewCodeGenerator::generate_aot_virtual_call(const ClassMethodInfo& method_info) {
    debug_print("Generating AOT virtual method call for: " + method_info.name);
    
    // Standard vtable dispatch for AOT compilation
    size_t vtable_slot = method_info.vtable_slot;
    size_t vtable_offset = vtable_slot * 8; // Each entry is an 8-byte pointer
    
    debug_print("Method vtable slot: " + std::to_string(vtable_slot));
    debug_print("Method vtable offset: " + std::to_string(vtable_offset) + " bytes");
    
    // Generate code to load the method address from the vtable
    std::string vtable_ptr_reg = register_manager_.acquire_scratch_reg(*this);
    std::string method_addr_reg = register_manager_.acquire_scratch_reg(*this);
    
    // LDR vtable_ptr_reg, [X0, #0] - Load vtable pointer from object
    emit(Encoder::create_ldr_imm(vtable_ptr_reg, "X0", 0, "Load vtable pointer"));
    
    // LDR method_addr_reg, [vtable_ptr_reg, #offset] - Load method address from vtable
    emit(Encoder::create_ldr_imm(method_addr_reg, vtable_ptr_reg, vtable_offset, 
         "Load virtual method address for " + method_info.name));
    
    register_manager_.release_register(vtable_ptr_reg);
    
    // The result of this expression is the callable address of the method
    expression_result_reg_ = method_addr_reg;
    
    debug_print("Virtual method address loaded into: " + method_addr_reg);
}

void NewCodeGenerator::generate_jit_virtual_call(const ClassMethodInfo& method_info) {
    debug_print("Generating JIT virtual method call for: " + method_info.name);
    
    // For now, JIT mode uses the same vtable dispatch as AOT
    // TODO: In Phase 3, this will be enhanced with PIC optimization
    generate_aot_virtual_call(method_info);
    
    debug_print("JIT virtual call generated (using standard vtable dispatch)");
}