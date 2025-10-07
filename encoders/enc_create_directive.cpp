// This encoder is NOT present in the test schedule. Test will be added via wrapper and results updated here.
#include "Encoder.h"
#include <sstream>

Instruction Encoder::create_directive(const std::string& directive_text, uint64_t data_value, const std::string& target_label, bool is_data) {
    // For label definitions (like ".L6:"), use the directive_text directly
    // For data directives, use the data_value with .quad
    
    std::string assembly_text;
    
    if (directive_text.find(":") != std::string::npos) {
        // This is a label definition - use the directive_text as-is
        assembly_text = directive_text;
    } else {
        // This is a data directive - generate .quad with the data_value
        std::stringstream ss;
        ss << ".quad 0x" << std::hex << data_value;
        
        // Append the target label as a comment for clarity, if provided.
        if (!target_label.empty()) {
            ss << " ; " << target_label;
        }
        
        assembly_text = ss.str();
    }

    // Return the instruction with the appropriate assembly text
    Instruction instr(static_cast<uint32_t>(data_value & 0xFFFFFFFF), assembly_text, RelocationType::NONE, target_label, is_data, false);
    instr.opcode = InstructionDecoder::OpType::DIRECTIVE;
    return instr;
}