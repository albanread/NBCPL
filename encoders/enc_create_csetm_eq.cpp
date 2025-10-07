// Status: PASS - Tested by NewBCPL --test-encoders
#include "Encoder.h"

Instruction Encoder::create_csetm_eq(const std::string& xd) {
    return create_csetm(xd, "EQ");
}
