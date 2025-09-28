.section __TEXT,__text,regular,pure_instructions
.globl _start
.p2align 2
_start:
    B L_START
L_START:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    MOVZ X9, #30600
    MOVK X9, #21862, LSL #16
    MOVK X9, #13124, LSL #32
    MOVK X9, #4386, LSL #48 ; Load UPPER_CANARY_VALUE
    STR X9, [X29, #16] ; Store Upper Stack Canary
    MOVZ X9, #17
    MOVK X9, #61183, LSL #16
    MOVK X9, #52445, LSL #32
    MOVK X9, #43707, LSL #48 ; Load LOWER_CANARY_VALUE
    STR X9, [X29, #24] ; Store Lower Stack Canary
    MOVZ X10, #1
    STR X10, [X29, #32]
    MOVZ X10, #2
    STR X10, [X29, #40] ; Y
    LDR X21, [X29, #32]
    MOVZ X10, #1
    CMP X21, X10
    CSET X11, EQ
    B.NE L_0
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X0, X10
    MOVZ X19, #46548 ; load WRITES addr (lo16)
    MOVK X19, #767, LSL #16 ; load WRITES addr (hi16)
    MOVK X19, #1, LSL #32 ; load WRITES addr (hi32)
    BL _WRITES
    B L_1
L_0:
L_1:
    LDR X21, [X29, #32]
    MOVZ X10, #2
    CMP X21, X10
    CSET X11, EQ
    B.NE L_2
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    MOV X0, X10
    BL _WRITES
    B L_3
L_2:
    ADRP X10, L_str2@PAGE
    ADD X10, X10, L_str2@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_3:
    LDR X22, [X29, #40]
    MOVZ X10, #2
    CMP X22, X10
    CSET X11, EQ
    B.NE L_4
    ADRP X10, L_str3@PAGE
    ADD X10, X10, L_str3@PAGEOFF
    MOV X0, X10
    BL _WRITES
    B L_5
L_4:
    ADRP X10, L_str4@PAGE
    ADD X10, X10, L_str4@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_5:
    LDR X22, [X29, #40]
    MOVZ X10, #3
    CMP X22, X10
    CSET X11, EQ
    B.NE L_6
    ADRP X10, L_str5@PAGE
    ADD X10, X10, L_str5@PAGEOFF
    MOV X0, X10
    BL _WRITES
    B L_7
L_6:
    ADRP X10, L_str6@PAGE
    ADD X10, X10, L_str6@PAGEOFF
    MOV X0, X10
    BL _WRITES
L_7:
    LDR X10, [X29, #16] ; Load Upper Stack Canary for check
    MOVZ X11, #30600
    MOVK X11, #21862, LSL #16
    MOVK X11, #13124, LSL #32
    MOVK X11, #4386, LSL #48 ; Load Expected UPPER_CANARY_VALUE
    CMP X10, X11 ; Compare Upper Canary
    B.NE L_START_stackprot_upper ; Branch if Upper Canary Corrupted
    LDR X10, [X29, #24] ; Load Lower Stack Canary for check
    MOVZ X11, #17
    MOVK X11, #61183, LSL #16
    MOVK X11, #52445, LSL #32
    MOVK X11, #43707, LSL #48 ; Load Expected LOWER_CANARY_VALUE
    CMP X10, X11 ; Compare Lower Canary
    B.NE L_START_stackprot_lower ; Branch if Lower Canary Corrupted
    LDP X29, X30, [SP], #64
    RET
L_START_stackprot_upper:
    BRK #0 ; Stack Corruption Detected for Upper Canary!
L_START_stackprot_lower:
    BRK #0 ; Stack Corruption Detected for Lower Canary!
    nop
    nop
    nop
    nop
    nop
    nop

.section __DATA,__data
L__data_segment_base:
.p2align 2
    .word 0x17
L_str0:
    .word 0x54
    .word 0x45
    .word 0x53
    .word 0x54
    .word 0x20
    .word 0x58
    .word 0x20
    .word 0x3d
    .word 0x20
    .word 0x31
    .word 0x20
    .word 0x54
    .word 0x48
    .word 0x45
    .word 0x4e
    .word 0x20
    .word 0x62
    .word 0x72
    .word 0x61
    .word 0x6e
    .word 0x63
    .word 0x68
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0x17
L_str1:
    .word 0x54
    .word 0x45
    .word 0x53
    .word 0x54
    .word 0x20
    .word 0x58
    .word 0x20
    .word 0x3d
    .word 0x20
    .word 0x32
    .word 0x20
    .word 0x54
    .word 0x48
    .word 0x45
    .word 0x4e
    .word 0x20
    .word 0x62
    .word 0x72
    .word 0x61
    .word 0x6e
    .word 0x63
    .word 0x68
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0x17
L_str2:
    .word 0x54
    .word 0x45
    .word 0x53
    .word 0x54
    .word 0x20
    .word 0x58
    .word 0x20
    .word 0x3d
    .word 0x20
    .word 0x32
    .word 0x20
    .word 0x45
    .word 0x4c
    .word 0x53
    .word 0x45
    .word 0x20
    .word 0x62
    .word 0x72
    .word 0x61
    .word 0x6e
    .word 0x63
    .word 0x68
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0x17
L_str3:
    .word 0x54
    .word 0x45
    .word 0x53
    .word 0x54
    .word 0x20
    .word 0x59
    .word 0x20
    .word 0x3d
    .word 0x20
    .word 0x32
    .word 0x20
    .word 0x54
    .word 0x48
    .word 0x45
    .word 0x4e
    .word 0x20
    .word 0x62
    .word 0x72
    .word 0x61
    .word 0x6e
    .word 0x63
    .word 0x68
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0x17
L_str4:
    .word 0x54
    .word 0x45
    .word 0x53
    .word 0x54
    .word 0x20
    .word 0x59
    .word 0x20
    .word 0x3d
    .word 0x20
    .word 0x32
    .word 0x20
    .word 0x45
    .word 0x4c
    .word 0x53
    .word 0x45
    .word 0x20
    .word 0x62
    .word 0x72
    .word 0x61
    .word 0x6e
    .word 0x63
    .word 0x68
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0x17
L_str5:
    .word 0x54
    .word 0x45
    .word 0x53
    .word 0x54
    .word 0x20
    .word 0x59
    .word 0x20
    .word 0x3d
    .word 0x20
    .word 0x33
    .word 0x20
    .word 0x54
    .word 0x48
    .word 0x45
    .word 0x4e
    .word 0x20
    .word 0x62
    .word 0x72
    .word 0x61
    .word 0x6e
    .word 0x63
    .word 0x68
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0x17
L_str6:
    .word 0x54
    .word 0x45
    .word 0x53
    .word 0x54
    .word 0x20
    .word 0x59
    .word 0x20
    .word 0x3d
    .word 0x20
    .word 0x33
    .word 0x20
    .word 0x45
    .word 0x4c
    .word 0x53
    .word 0x45
    .word 0x20
    .word 0x62
    .word 0x72
    .word 0x61
    .word 0x6e
    .word 0x63
    .word 0x68
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
