// Status: PASS - Tested by NewBCPL --test-encoders
#include "Encoder.h"
// this is "dumbish" LOL
Instruction Encoder::create_dmb() {
    // The opcode for DMB ISH (Data Memory Barrier, Inner Shareable) is fixed.
    // This instruction ensures that all memory accesses appearing before
    // the DMB are observed before any memory accesses appearing after the DMB.
        const uint32_t encoding = 0xD5033BBF;

    Instruction instr(encoding, "DMB ISH");
    instr.opcode = InstructionDecoder::OpType::DMB;
    return instr;
}
