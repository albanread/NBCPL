.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _WRITES
.p2align 2
_start:
    B L_START
L_START:
    STP X29, X30, [SP, #-464]!
    MOV X29, SP
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDP X29, X30, [SP], #464
    RET
    B L_START
    RET
    nop
    nop

.section __DATA,__data
L__data_segment_base:
.p2align 2
    .word 0x4
L_str0:
    .word 0x48
    .word 0x65
    .word 0x79
    .word 0x20
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
