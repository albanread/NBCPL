.text
.global _start
_start:
    // Test FCMP with S registers (32-bit)
    fcmp s0, s1
    nop

    // Test FCMP with D registers (64-bit)
    fcmp d0, d1
    nop

    // Test FNEG with S registers
    fneg s0, s1
    nop

    // Test FNEG with D registers
    fneg d0, d1
    nop

    // Test FSQRT with S registers
    fsqrt s0, s1
    nop

    // Test FSQRT with D registers
    fsqrt d0, d1
    nop
