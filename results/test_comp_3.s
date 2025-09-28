.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _WRITEN
.p2align 2
_start:
    B L_START
L_START:
    STP X29, X30, [SP, #-496]!
    MOV X29, SP
    MOVZ X10, #5
    STR X10, [X29, #16]
    MOVZ X10, #10
    STR X10, [X29, #24]
    MOVZ X10, #5
    STR X10, [X29, #40]
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    CMP X21, X22
    CSET X10, LE
    MOV X23, X10
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDR X21, [X29, #16]
    LDR X24, [X29, #40]
    CMP X21, X24
    CSET X10, LE
    MOV X23, X10
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDR X22, [X29, #24]
    LDR X21, [X29, #16]
    CMP X22, X21
    CSET X10, LE
    MOV X23, X10
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDR X22, [X29, #24]
    LDR X21, [X29, #16]
    CMP X22, X21
    CSET X10, GE
    MOV X23, X10
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDR X21, [X29, #16]
    LDR X24, [X29, #40]
    CMP X21, X24
    CSET X10, GE
    MOV X23, X10
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    CMP X21, X22
    CSET X10, GE
    MOV X23, X10
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDP X29, X30, [SP], #496
    RET
    B L_START
    RET
