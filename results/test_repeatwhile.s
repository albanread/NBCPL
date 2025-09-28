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
    MOVZ X10, #0
    STR X10, [X29, #32] ; I
L_0:
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X0, X10
    MOVZ X19, #14028 ; load WRITES addr (lo16)
    MOVK X19, #654, LSL #16 ; load WRITES addr (hi16)
    MOVK X19, #1, LSL #32 ; load WRITES addr (hi32)
    BL _WRITES
    LDR X21, [X29, #32]
    MOVZ X10, #1
    ADD X11, X21, X10
    MOV X21, X11
    STR X21, [X29, #32] ; I ; I @ FP+32
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X21, [X29, #32]
    MOV X0, X21
    MOVZ X20, #14776 ; load WRITEN addr (lo16)
    MOVK X20, #654, LSL #16 ; load WRITEN addr (hi16)
    MOVK X20, #1, LSL #32 ; load WRITEN addr (hi32)
    BL _WRITEN
    ADRP X10, L_str2@PAGE
    ADD X10, X10, L_str2@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X21, [X29, #32]
    MOVZ X10, #9
    CMP X21, X10
    CSET X11, LT
    MOV X0, X11
    BL _WRITEN
    ADRP X10, L_str3@PAGE
    ADD X10, X10, L_str3@PAGEOFF
    MOV X0, X10
    BL _WRITES
    ADRP X10, L_str4@PAGE
    ADD X10, X10, L_str4@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X21, [X29, #32]
    MOVZ X10, #9
    CMP X21, X10
    B.LT L_0
L_1:
    ADRP X10, L_str5@PAGE
    ADD X10, X10, L_str5@PAGEOFF
    MOV X0, X10
    BL _WRITES
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
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #64 ; Deallocate space for saved FP/LR
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
    .word 0x18
L_str0:
    .word 0x45
    .word 0x6e
    .word 0x74
    .word 0x65
    .word 0x72
    .word 0x20
    .word 0x72
    .word 0x65
    .word 0x70
    .word 0x65
    .word 0x61
    .word 0x74
    .word 0x20
    .word 0x77
    .word 0x68
    .word 0x69
    .word 0x6c
    .word 0x65
    .word 0x20
    .word 0x74
    .word 0x65
    .word 0x73
    .word 0x74
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0x2
L_str1:
    .word 0x49
    .word 0x3d
    .word 0x0
    .word 0x0
    .word 0x8
L_str2:
    .word 0x20
    .word 0x49
    .word 0x3c
    .word 0x39
    .word 0x20
    .word 0x3f
    .word 0x3a
    .word 0x20
    .word 0x0
    .word 0x0
    .word 0x1
L_str3:
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0x17
L_str4:
    .word 0x4c
    .word 0x65
    .word 0x61
    .word 0x76
    .word 0x65
    .word 0x72
    .word 0x65
    .word 0x70
    .word 0x65
    .word 0x61
    .word 0x74
    .word 0x20
    .word 0x77
    .word 0x68
    .word 0x69
    .word 0x6c
    .word 0x65
    .word 0x20
    .word 0x74
    .word 0x65
    .word 0x73
    .word 0x74
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0x5
L_str5:
    .word 0x42
    .word 0x59
    .word 0x45
    .word 0x21
    .word 0xa
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
