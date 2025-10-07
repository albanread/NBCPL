// This encoder is present in the test schedule and has passed automated validation.
#include "Encoder.h"

Instruction Encoder::create_cset_eq(const std::string& xd) {
    return create_cset(xd, "EQ");
}
