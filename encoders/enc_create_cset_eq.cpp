// Status: PASS - Tested by NewBCPL --test-encoders
#include "Encoder.h"

Instruction Encoder::create_cset_eq(const std::string& xd) {
    return create_cset(xd, "EQ");
}
