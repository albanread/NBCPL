.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _WRITEN
.globl _BCPL_ALLOC_CHARS
.globl _RDCH
.globl _MALLOC
.globl _BCPL_ALLOC_WORDS
.globl _WRITEC
.globl _WRITES
.globl _FREEVEC
.p2align 2
_start:
    B L_START
L_START:
    STP X29, X30, [SP, #-80]!
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
    STR X19, [X29, #64] ; [JIT_STORE] ; Saved Reg: X19 @ FP+64
    STR X20, [X29, #72] ; [JIT_STORE] ; Saved Reg: X20 @ FP+72
    MOVZ X10, #100
    STR X10, [X29, #56] ; K
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X0, X10
    BL _WRITES
    MOVZ X10, #1
    STR X10, [X29, #32] ; loop_counter
    MOVZ X10, #1
    STR X10, [X29, #48] ; loop_step
    B L_1
L_0:
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X21, [X29, #56]
    MOVZ X10, #1
    MOV X11, X21
    ADD X11, X11, X10
    STR X11, [X29, #56] ; K
    LDR X22, [X29, #32]
    MOV X0, X22
    BL _WRITEN
    LDR X10, [X29, #32]
    LDR X11, [X29, #48]
    ADD X10, X10, X11
    STR X10, [X29, #32]
L_1:
    LDR X10, [X29, #32]
    CMP X10, #20
    B.LE L_0
    LDR X21, [X29, #56]
    MOV X0, X21
    BL _WRITEN
    ADRP X10, L_str2@PAGE
    ADD X10, X10, L_str2@PAGEOFF
    MOV X0, X10
    BL _WRITES
    ADRP X10, L_str3@PAGE
    ADD X10, X10, L_str3@PAGEOFF
    MOV X0, X10
    BL _WRITES
    MOVZ X10, #0
    MOV X0, X10
    MOVZ X10, #1
    MOVK X10, #512, LSL #16
    MOVK X10, #0, LSL #32
    MOVK X10, #0, LSL #48
    MOV X16, X10
    SVC #128
    LDR X19, [X29, #64] ; [JIT_RESTORE] ; Restored Reg: X19 @ FP+64
    LDR X20, [X29, #72] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+72
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
    ADD SP, SP, #80 ; Deallocate space for saved FP/LR
    RET
L_START_stackprot_upper:
    BRK #0 ; Stack Corruption Detected for Upper Canary!
L_START_stackprot_lower:
    BRK #0 ; Stack Corruption Detected for Lower Canary!

.section __TEXT,__const
.p2align 3
L_str0:
    
    .long 0x10
    .long 0x41
    .long 0x6e
    .long 0x6f
    .long 0x74
    .long 0x68
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x73
    .long 0x74
    .long 0x72
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x2e
    .long 0xa
    .long 0x0
    .long 0x0
L_str1:
    
    .long 0x11
    .long 0x49
    .long 0x6e
    .long 0x20
    .long 0x66
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x6c
    .long 0x6f
    .long 0x6f
    .long 0x70
    .long 0x20
    .long 0x62
    .long 0x6f
    .long 0x64
    .long 0x79
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    
    .long 0xb
    .long 0x65
    .long 0x6e
    .long 0x64
    .long 0x20
    .long 0x6f
    .long 0x66
    .long 0x20
    .long 0x6c
    .long 0x6f
    .long 0x6f
    .long 0x70
    .long 0x0
    .long 0x0
L_str3:
    
    .long 0x1
    .long 0xa
    .long 0x0
    .long 0x0
    .long 0x0, 0x0, 0x0, 0x0

.section __DATA,__data
.p2align 3
