.section __TEXT,__text,regular,pure_instructions
.globl _start
.p2align 2
_start:
    B L_START
L_START:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    MOVZ X10, #10
    STR X10, [X29, #16]
    MOVZ X10, #5
    STR X10, [X29, #24]
    MOVZ X10, #2
    STR X10, [X29, #32]
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    ADD X10, X21, X22
    MOV X23, X10
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    MOVZ X19, #4644 ; load WRITEN addr (lo16)
    MOVK X19, #191, LSL #16 ; load WRITEN addr (hi16)
    MOVK X19, #1, LSL #32 ; load WRITEN addr (hi32)
    BL _WRITEN
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    SUB X10, X21, X22
    MOV X23, X10
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    MUL X10, X21, X22
    MOV X23, X10
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    SDIV X10, X21, X22
    MOV X23, X10
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    SDIV X11, X21, X22
    MUL X11, X11, X22
    SUB X10, X21, X11
    MOV X23, X10
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    MOVZ X10, #10
    SUB X11, XZR, X10
    MOV X21, X11
    STR X21, [X29, #16]
    MOVZ X10, #3
    MOV X22, X10
    STR X22, [X29, #24]
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    SDIV X10, X21, X22
    MOV X23, X10
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    SDIV X11, X21, X22
    MUL X11, X11, X22
    SUB X10, X21, X11
    MOV X23, X10
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X0, X10
    MOVZ X20, #4152 ; load WRITES addr (lo16)
    MOVK X20, #191, LSL #16 ; load WRITES addr (hi16)
    MOVK X20, #1, LSL #32 ; load WRITES addr (hi32)
    BL _WRITES
    MOVZ X10, #0
    MOV X0, X10
    MOVZ X11, #1
    MOVK X11, #512, LSL #16
    MOV X16, X11
    SVC #128
    LDP X29, X30, [SP, #64]!
    RET
    nop
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
