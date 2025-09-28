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
L_FACT:
    STP X29, X30, [SP, #-96]!
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
    STR X19, [X29, #72] ; [JIT_STORE] ; Saved Reg: X19 @ FP+72
    STR X20, [X29, #80] ; [JIT_STORE] ; Saved Reg: X20 @ FP+80
    STR X0, [X29, #32] ; N
    MOVZ X10, #1
    STR X10, [X29, #64]
    MOVZ X10, #1
    STR X10, [X29, #40] ; loop_counter
    MOVZ X10, #1
    STR X10, [X29, #56] ; loop_step
    LDR X21, [X29, #32]
    STR X21, [X29, #48]
    B L_1
L_0:
    LDR X22, [X29, #64]
    LDR X23, [X29, #40]
    MOV X10, X22
    MUL X10, X10, X23
    STR X10, [X29, #64]
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X22, [X29, #64]
    MOV X0, X22
    BL _WRITEN
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X10, [X29, #40]
    LDR X11, [X29, #56]
    ADD X10, X10, X11
    STR X10, [X29, #40]
L_1:
    LDR X10, [X29, #40]
    LDR X11, [X29, #48]
    CMP X10, X11
    B.LE L_0
    LDR X22, [X29, #64]
    MOV X0, X22
    LDR X19, [X29, #72] ; [JIT_RESTORE] ; Restored Reg: X19 @ FP+72
    LDR X20, [X29, #80] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+80
    LDR X10, [X29, #16] ; Load Upper Stack Canary for check
    MOVZ X11, #30600
    MOVK X11, #21862, LSL #16
    MOVK X11, #13124, LSL #32
    MOVK X11, #4386, LSL #48 ; Load Expected UPPER_CANARY_VALUE
    CMP X10, X11 ; Compare Upper Canary
    B.NE L_FACT_stackprot_upper ; Branch if Upper Canary Corrupted
    LDR X10, [X29, #24] ; Load Lower Stack Canary for check
    MOVZ X11, #17
    MOVK X11, #61183, LSL #16
    MOVK X11, #52445, LSL #32
    MOVK X11, #43707, LSL #48 ; Load Expected LOWER_CANARY_VALUE
    CMP X10, X11 ; Compare Lower Canary
    B.NE L_FACT_stackprot_lower ; Branch if Lower Canary Corrupted
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #96 ; Deallocate space for saved FP/LR
    RET
L_FACT_stackprot_upper:
    BRK #0 ; Stack Corruption Detected for Upper Canary!
L_FACT_stackprot_lower:
    BRK #0 ; Stack Corruption Detected for Lower Canary!
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
    STR X19, [X29, #40] ; [JIT_STORE] ; Saved Reg: X19 @ FP+40
    STR X20, [X29, #48] ; [JIT_STORE] ; Saved Reg: X20 @ FP+48
    MOVZ X10, #5
    MOV X0, X10
    BL L_FACT
    STR X0, [X29, #32] ; U
    ADRP X10, L_str2@PAGE
    ADD X10, X10, L_str2@PAGEOFF
    MOV X0, X10
    BL _WRITES
    ADRP X10, L_str3@PAGE
    ADD X10, X10, L_str3@PAGEOFF
    MOV X0, X10
    BL _WRITES
    LDR X19, [X29, #40] ; [JIT_RESTORE] ; Restored Reg: X19 @ FP+40
    LDR X20, [X29, #48] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+48
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

.section __TEXT,__const
.p2align 3
L_str0:
    
    .long 0x5
    .long 0x20
    .long 0x52
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x0
    .long 0x0
L_str1:
    
    .long 0x1
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    
    .long 0x6
    .long 0x42
    .long 0x45
    .long 0x47
    .long 0x49
    .long 0x4e
    .long 0xa
    .long 0x0
    .long 0x0
L_str3:
    
    .long 0x4
    .long 0x45
    .long 0x4e
    .long 0x44
    .long 0xa
    .long 0x0
    .long 0x0
    .long 0x0, 0x0, 0x0, 0x0

.section __DATA,__data
.p2align 3
