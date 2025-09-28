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
    MOVZ X10, #5
    MOV X0, X10
    ADRP X1, L_str0@PAGE
    ADD X1, X1, L_str0@PAGEOFF
    ADRP X2, L_str1@PAGE
    ADD X2, X2, L_str1@PAGEOFF
    MOVZ X19, #21144 ; load BCPL_ALLOC_WORDS addr (lo16)
    MOVK X19, #204, LSL #16 ; load BCPL_ALLOC_WORDS addr (hi16)
    MOVK X19, #1, LSL #32 ; load BCPL_ALLOC_WORDS addr (hi32)
    BL _BCPL_ALLOC_WORDS
    STR X0, [X29, #32] ; V
    MOVZ X10, #102
    LDR X21, [X29, #32]
    MOVZ X11, #0
    LSL X11, X11, #3
    ADD X12, X21, X11
    STR X10, [X12]
    ADRP X10, L_str2@PAGE
    ADD X10, X10, L_str2@PAGEOFF
    MOV X0, X10
    MOVZ X20, #22812 ; load WRITES addr (lo16)
    MOVK X20, #204, LSL #16 ; load WRITES addr (hi16)
    MOVK X20, #1, LSL #32 ; load WRITES addr (hi32)
    BL _WRITES
    LDR X21, [X29, #32]
    MOVZ X10, #0
    LSL X10, X10, #3
    ADD X12, X21, X10
    LDR X11, [X12]
    MOV X0, X11
    MOVZ X19, #23700 ; load WRITEN addr (lo16)
    MOVK X19, #204, LSL #16 ; load WRITEN addr (hi16)
    MOVK X19, #1, LSL #32 ; load WRITEN addr (hi32)
    BL _WRITEN
    MOVZ X10, #10
    MOV X0, X10
    MOVZ X20, #22136 ; load WRITEC addr (lo16)
    MOVK X20, #204, LSL #16 ; load WRITEC addr (hi16)
    MOVK X20, #1, LSL #32 ; load WRITEC addr (hi32)
    BL _WRITEC
    LDR X21, [X29, #32]
    MOV X0, X21
    MOVZ X19, #21620 ; load FREEVEC addr (lo16)
    MOVK X19, #204, LSL #16 ; load FREEVEC addr (hi16)
    MOVK X19, #1, LSL #32 ; load FREEVEC addr (hi32)
    BL _FREEVEC
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

.section __DATA,__data
L__data_segment_base:
.p2align 2
    .word 0x5
L_str0:
    .word 0x53
    .word 0x54
    .word 0x41
    .word 0x52
    .word 0x54
    .word 0x0
    .word 0x0
    .word 0x0
L_str1:
    .word 0x0
    .word 0x0
    .word 0x7
L_str2:
    .word 0x56
    .word 0x5b
    .word 0x30
    .word 0x5d
    .word 0x20
    .word 0x3d
    .word 0x20
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
    .word 0x0
