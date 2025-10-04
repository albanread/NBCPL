#include <arm_neon.h>
void test() { int32x2_t v = {10, 20}; int a = vget_lane_s32(v, 0); int b = vget_lane_s32(v, 1); }
