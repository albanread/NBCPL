.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _WRITEN
.globl _WRITES
.p2align 2
_start:
    B L_START
L_START:
    STP X29, X30, [SP, #-256]!
    MOV X29, SP
    MOVZ X10, #0
    STR X10, [X29, #16]
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X21, [X29, #16]
    MOV X0, X21
    BL _WRITEN
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X21, [X29, #16]
    MOVZ X10, #10
    CMP X21, X10
    CSET X11, LE
    MOV X0, X11
    BL _WRITEN
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X21, [X29, #16]
    MOVZ X10, #10
    CMP X21, X10
    CSET X11, NE
    B.EQ L_0
L_1:
    LDR X21, [X29, #16]
    MOVZ X10, #1
    ADD X11, X21, X10
    MOV X21, X11
    STR X21, [X29, #16]
L_3:
    LDR X21, [X29, #16]
    MOVZ X10, #10
    CMP X21, X10
    CSET X11, NE
    CMP X11, XZR
    CSET X10, EQ
    SUB X10, XZR, X10
    CMP X10, XZR
    B.EQ L_1
L_2:
L_0:
    LDR X21, [X29, #16]
    MOV X0, X21
    BL _WRITEN
    LDR X21, [X29, #16]
    MOV X0, X21
    LDP X29, X30, [SP], #256
    RET
    B L_START
    RET
    nop
    nop

.section __DATA,__data
L__data_segment_base:
.p2align 2
    .word 0xb
L_str0:
    .word 0x49
    .word 0x6e
    .word 0x69
    .word 0x74
    .word 0x69
    .word 0x61
    .word 0x6c
    .word 0x20
    .word 0x49
    .word 0x3d
    .word 0x20
    .word 0x0
    .word 0x0
    .word 0x1
L_str1:
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
