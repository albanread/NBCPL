// This encoder is NOT present in the test schedule. Test will be added via wrapper and results updated here.
#include "Encoder.h"

Instruction Encoder::create_csetm_ne(const std::string& xd) {
    return create_csetm(xd, "NE");
}
