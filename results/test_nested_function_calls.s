.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _WRITEN
.p2align 2
_start:
    B L_START
L_START:
L_MULTIPLY:
L_CALCULATE:
L_ADD:
    STP X29, X30, [SP, #-288]!
    MOV X29, SP
    STR X0, [X29, #16]
    STR X1, [X29, #24]
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    L_ADD X10, X21, X22
    LDP X29, X30, [SP], #288
    RET
    STP X29, X30, [SP, #-304]!
    MOV X29, SP
    STR X0, [X29, #32]
    STR X1, [X29, #40]
    LDR X21, [X29, #32]
    LDR X22, [X29, #40]
    MUL X11, X21, X22
    LDP X29, X30, [SP], #304
    RET
    STP X29, X30, [SP, #-336]!
    MOV X29, SP
    STR X0, [X29, #40]
    STR X1, [X29, #48]
    STR X2, [X29, #56]
    ADRP X12, L_MULTIPLY
    L_ADD X12, X12, #:lo12:L_MULTIPLY
    ADRP X13, L_ADD
    L_ADD X13, X13, #:lo12:L_ADD
    LDR X23, [X29, #40]
    MOV X0, X23
    LDR X24, [X29, #48]
    MOV X1, X24
    BL L_ADD
    MOV X0, X0
    LDR X25, [X29, #56]
    MOV X1, X25
    BL L_MULTIPLY
    LDP X29, X30, [SP], #336
    RET
    STP X29, X30, [SP, #-304]!
    MOV X29, SP
    ADRP X12, L_CALCULATE
    L_ADD X12, X12, #:lo12:L_CALCULATE
    MOVZ X13, #2
    MOV X0, X13
    MOVZ X13, #3
    MOV X1, X13
    MOVZ X13, #4
    MOV X2, X13
    BL L_CALCULATE
    STR X0, [X29, #16]
    STP X10, X11, [SP, #-16]!
    LDR X26, [X29, #16]
    MOV X0, X26
    BL _WRITEN
    LDP X10, X11, [SP], #16
    ADRP X12, L_CALCULATE
    L_ADD X12, X12, #:lo12:L_CALCULATE
    MOVZ X13, #5
    MOV X0, X13
    MOVZ X13, #5
    MOV X1, X13
    MOVZ X13, #10
    MOV X2, X13
    BL L_CALCULATE
    MOV X26, X0
    STR X26, [X29, #16]
    STP X10, X11, [SP, #-16]!
    LDR X26, [X29, #16]
    MOV X0, X26
    BL _WRITEN
    LDP X10, X11, [SP], #16
    LDP X29, X30, [SP], #304
    RET
    B L_START
    RET
