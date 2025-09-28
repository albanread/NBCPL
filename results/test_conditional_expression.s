.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _WRITEN
.p2align 2
_start:
    B L_START
L_START:
    STP X29, X30, [SP, #-496]!
    MOV X29, SP
    MOVZ X10, #10
    STR X10, [X29, #16]
    MOVZ X10, #5
    STR X10, [X29, #24]
    MOVZ X10, #0
    STR X10, [X29, #32]
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    CMP X21, X22
    CSET X10, GT
    CMP X10, XZR
    B.EQ L_0
    MOVZ X10, #100
    MOV X11, X10
    B L_1
L_0:
    MOVZ X10, #200
    MOV X11, X10
L_1:
    MOV X23, X11
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    CMP X21, X22
    CSET X10, LT
    CMP X10, XZR
    B.EQ L_2
    MOVZ X10, #100
    MOV X11, X10
    B L_3
L_2:
    MOVZ X10, #200
    MOV X11, X10
L_3:
    MOV X23, X11
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDR X21, [X29, #16]
    MOVZ X10, #10
    CMP X21, X10
    CSET X11, EQ
    CMP X11, XZR
    B.EQ L_4
    LDR X22, [X29, #24]
    MOVZ X10, #5
    CMP X22, X10
    CSET X11, EQ
    CMP X11, XZR
    B.EQ L_6
    MOVZ X10, #300
    MOV X11, X10
    B L_7
L_6:
    MOVZ X10, #400
    MOV X11, X10
L_7:
    MOV X10, X11
    B L_5
L_4:
    MOVZ X11, #500
    MOV X10, X11
L_5:
    MOV X23, X10
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDR X21, [X29, #16]
    MOVZ X10, #10
    CMP X21, X10
    CSET X11, EQ
    CMP X11, XZR
    B.EQ L_8
    LDR X22, [X29, #24]
    MOVZ X10, #6
    CMP X22, X10
    CSET X11, EQ
    CMP X11, XZR
    B.EQ L_10
    MOVZ X10, #300
    MOV X11, X10
    B L_11
L_10:
    MOVZ X10, #400
    MOV X11, X10
L_11:
    MOV X10, X11
    B L_9
L_8:
    MOVZ X11, #500
    MOV X10, X11
L_9:
    MOV X23, X10
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    MOVZ X10, #1
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    CMP X21, X22
    CSET X11, GT
    ADD X12, X10, X11
    CMP X12, XZR
    B.EQ L_12
    MOVZ X10, #10
    MOV X11, X10
    B L_13
L_12:
    MOVZ X10, #20
    MOV X11, X10
L_13:
    MOV X23, X11
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    CMP X21, X22
    CSET X10, LT
    CMP X10, XZR
    B.EQ L_14
    MOVZ X10, #10
    MOV X11, X10
    B L_15
L_14:
    MOVZ X10, #20
    MOV X11, X10
L_15:
    MOVZ X10, #1
    ADD X12, X11, X10
    MOV X23, X12
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDR X21, [X29, #16]
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    CMP X21, X22
    CSET X10, GT
    ADD X11, X21, X10
    CMP X11, XZR
    B.EQ L_16
    MOVZ X10, #1
    MOV X11, X10
    B L_17
L_16:
    MOVZ X10, #0
    LDR X22, [X29, #24]
    MUL X12, X10, X22
    MOV X11, X12
L_17:
    MOV X23, X11
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDR X21, [X29, #16]
    LDR X21, [X29, #16]
    LDR X22, [X29, #24]
    CMP X21, X22
    CSET X10, LT
    ADD X11, X21, X10
    CMP X11, XZR
    B.EQ L_18
    MOVZ X10, #1
    MOV X11, X10
    B L_19
L_18:
    MOVZ X10, #0
    LDR X22, [X29, #24]
    MUL X12, X10, X22
    MOV X11, X12
L_19:
    MOV X23, X11
    STR X23, [X29, #32]
    LDR X23, [X29, #32]
    MOV X0, X23
    BL _WRITEN
    LDP X29, X30, [SP], #496
    RET
    B L_START
    RET
