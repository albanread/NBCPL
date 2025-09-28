.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _WRITES
.globl _WRITEN
.p2align 2
_start:
    B L_START
L_START:
    STP X29, X30, [SP, #-480]!
    MOV X29, SP
    MOVZ X10, #0
    STR X10, [X29, #16]
L_0:
    LDR X21, [X29, #16]
    MOVZ X10, #1
    ADD X11, X21, X10
    MOV X21, X11
    STR X21, [X29, #16]
    LDR X21, [X29, #16]
    MOV X0, X21
    BL _WRITEN
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_1:
    LDR X21, [X29, #16]
    MOVZ X10, #5
    CMP X21, X10
    CSET X11, EQ
    B.NE L_0
    MOVZ X10, #0
    MOV X0, X10
    LDP X29, X30, [SP], #480
    RET
    B L_START
    RET
    nop

.section __DATA,__data
L__data_segment_base:
.p2align 2
    .word 0x1
L_str0:
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
