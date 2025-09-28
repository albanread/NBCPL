.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _WRITES
.p2align 2
_start:
    B L_START
L_START:
    STP X29, X30, [SP, #-256]!
    MOV X29, SP
    MOVZ X10, #5
    STR X10, [X29, #16]
    MOVZ X10, #10
    STR X10, [X29, #24]
    LDR X21, [X29, #16]
    MOVZ X10, #5
    CMP X21, X10
    CSET X11, EQ
    B.NE L_0
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_0:
    LDR X21, [X29, #16]
    MOVZ X10, #6
    CMP X21, X10
    CSET X11, EQ
    B.NE L_1
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_1:
    LDR X22, [X29, #24]
    MOVZ X10, #20
    CMP X22, X10
    CSET X11, LT
    B.GE L_2
    ADRP X10, L_str2@PAGE
    ADD X10, X10, L_str2@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_2:
    LDR X22, [X29, #24]
    MOVZ X10, #20
    CMP X22, X10
    CSET X11, GT
    B.LE L_3
    ADRP X10, L_str3@PAGE
    ADD X10, X10, L_str3@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_3:
    LDR X21, [X29, #16]
    MOVZ X10, #5
    CMP X21, X10
    CSET X11, NE
    B.EQ L_4
    ADRP X10, L_str4@PAGE
    ADD X10, X10, L_str4@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_4:
    LDR X21, [X29, #16]
    MOVZ X10, #5
    CMP X21, X10
    CSET X11, LE
    B.GT L_5
    ADRP X10, L_str5@PAGE
    ADD X10, X10, L_str5@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_5:
    LDR X22, [X29, #24]
    MOVZ X10, #10
    CMP X22, X10
    CSET X11, GE
    B.LT L_6
    ADRP X10, L_str6@PAGE
    ADD X10, X10, L_str6@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_6:
    LDR X21, [X29, #16]
    MOVZ X10, #5
    CMP X21, X10
    CSET X11, EQ
    CMP X11, XZR
    B.NE L_7
    ADRP X10, L_str7@PAGE
    ADD X10, X10, L_str7@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_7:
    LDR X21, [X29, #16]
    MOVZ X10, #6
    CMP X21, X10
    CSET X11, EQ
    CMP X11, XZR
    B.NE L_8
    ADRP X10, L_str8@PAGE
    ADD X10, X10, L_str8@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_8:
    LDR X22, [X29, #24]
    MOVZ X10, #20
    CMP X22, X10
    CSET X11, LT
    CMP X11, XZR
    B.NE L_9
    ADRP X10, L_str9@PAGE
    ADD X10, X10, L_str9@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_9:
    LDR X22, [X29, #24]
    MOVZ X10, #20
    CMP X22, X10
    CSET X11, GT
    CMP X11, XZR
    B.NE L_10
    ADRP X10, L_str10@PAGE
    ADD X10, X10, L_str10@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_10:
    LDR X21, [X29, #16]
    MOVZ X10, #5
    CMP X21, X10
    CSET X11, NE
    CMP X11, XZR
    B.NE L_11
    ADRP X10, L_str11@PAGE
    ADD X10, X10, L_str11@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_11:
    LDR X21, [X29, #16]
    MOVZ X10, #5
    CMP X21, X10
    CSET X11, LE
    CMP X11, XZR
    B.NE L_12
    ADRP X10, L_str12@PAGE
    ADD X10, X10, L_str12@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_12:
    LDR X22, [X29, #24]
    MOVZ X10, #10
    CMP X22, X10
    CSET X11, GE
    CMP X11, XZR
    B.NE L_13
    ADRP X10, L_str13@PAGE
    ADD X10, X10, L_str13@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_13:
    LDP X29, X30, [SP], #256
    RET
    B L_START
    RET
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

.section __DATA,__data
L__data_segment_base:
.p2align 2
    .word 0x9
L_str0:
    .word 0x49
    .word 0x46
    .word 0x20
    .word 0x58
    .word 0x20
    .word 0x3d
    .word 0x20
    .word 0x35
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0x9
L_str1:
    .word 0x49
    .word 0x46
    .word 0x20
    .word 0x58
    .word 0x20
    .word 0x3d
    .word 0x20
    .word 0x36
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0xa
L_str2:
    .word 0x49
    .word 0x46
    .word 0x20
    .word 0x59
    .word 0x20
    .word 0x3c
    .word 0x20
    .word 0x32
    .word 0x30
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0xa
L_str3:
    .word 0x49
    .word 0x46
    .word 0x20
    .word 0x59
    .word 0x20
    .word 0x3e
    .word 0x20
    .word 0x32
    .word 0x30
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0xa
L_str4:
    .word 0x49
    .word 0x46
    .word 0x20
    .word 0x58
    .word 0x20
    .word 0x7e
    .word 0x3d
    .word 0x20
    .word 0x35
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0xa
L_str5:
    .word 0x49
    .word 0x46
    .word 0x20
    .word 0x58
    .word 0x20
    .word 0x3c
    .word 0x3d
    .word 0x20
    .word 0x35
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0xb
L_str6:
    .word 0x49
    .word 0x46
    .word 0x20
    .word 0x59
    .word 0x20
    .word 0x3e
    .word 0x3d
    .word 0x20
    .word 0x31
    .word 0x30
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0xd
L_str7:
    .word 0x55
    .word 0x4e
    .word 0x4c
    .word 0x45
    .word 0x53
    .word 0x53
    .word 0x20
    .word 0x58
    .word 0x20
    .word 0x3d
    .word 0x20
    .word 0x35
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0xd
L_str8:
    .word 0x55
    .word 0x4e
    .word 0x4c
    .word 0x45
    .word 0x53
    .word 0x53
    .word 0x20
    .word 0x58
    .word 0x20
    .word 0x3d
    .word 0x20
    .word 0x36
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0xe
L_str9:
    .word 0x55
    .word 0x4e
    .word 0x4c
    .word 0x45
    .word 0x53
    .word 0x53
    .word 0x20
    .word 0x59
    .word 0x20
    .word 0x3c
    .word 0x20
    .word 0x32
    .word 0x30
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0xe
L_str10:
    .word 0x55
    .word 0x4e
    .word 0x4c
    .word 0x45
    .word 0x53
    .word 0x53
    .word 0x20
    .word 0x59
    .word 0x20
    .word 0x3e
    .word 0x20
    .word 0x32
    .word 0x30
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0xe
L_str11:
    .word 0x55
    .word 0x4e
    .word 0x4c
    .word 0x45
    .word 0x53
    .word 0x53
    .word 0x20
    .word 0x58
    .word 0x20
    .word 0x7e
    .word 0x3d
    .word 0x20
    .word 0x35
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0xe
L_str12:
    .word 0x55
    .word 0x4e
    .word 0x4c
    .word 0x45
    .word 0x53
    .word 0x53
    .word 0x20
    .word 0x58
    .word 0x20
    .word 0x3c
    .word 0x3d
    .word 0x20
    .word 0x35
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0xf
L_str13:
    .word 0x55
    .word 0x4e
    .word 0x4c
    .word 0x45
    .word 0x53
    .word 0x53
    .word 0x20
    .word 0x59
    .word 0x20
    .word 0x3e
    .word 0x3d
    .word 0x20
    .word 0x31
    .word 0x30
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
