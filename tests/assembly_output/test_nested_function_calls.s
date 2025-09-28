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
L_ADD:
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
    STR X19, [X29, #48] ; [JIT_STORE] ; Saved Reg: X19 @ FP+48
    STR X20, [X29, #56] ; [JIT_STORE] ; Saved Reg: X20 @ FP+56
    STR X0, [X29, #32] ; A
    STR X1, [X29, #40] ; B
    LDR X21, [X29, #32]
    LDR X22, [X29, #40]
    MOV X23, X21
    ADD X23, X23, X22
    MOV X0, X23
    LDR X19, [X29, #48] ; [JIT_RESTORE] ; Restored Reg: X19 @ FP+48
    LDR X20, [X29, #56] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+56
    LDR X10, [X29, #16] ; Load Upper Stack Canary for check
    MOVZ X11, #30600
    MOVK X11, #21862, LSL #16
    MOVK X11, #13124, LSL #32
    MOVK X11, #4386, LSL #48 ; Load Expected UPPER_CANARY_VALUE
    CMP X10, X11 ; Compare Upper Canary
    B.NE L_ADD_stackprot_upper ; Branch if Upper Canary Corrupted
    LDR X10, [X29, #24] ; Load Lower Stack Canary for check
    MOVZ X11, #17
    MOVK X11, #61183, LSL #16
    MOVK X11, #52445, LSL #32
    MOVK X11, #43707, LSL #48 ; Load Expected LOWER_CANARY_VALUE
    CMP X10, X11 ; Compare Lower Canary
    B.NE L_ADD_stackprot_lower ; Branch if Lower Canary Corrupted
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #64 ; Deallocate space for saved FP/LR
    RET
L_ADD_stackprot_upper:
    BRK #0 ; Stack Corruption Detected for Upper Canary!
L_ADD_stackprot_lower:
    BRK #0 ; Stack Corruption Detected for Lower Canary!
L_MULTIPLY:
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
    STR X19, [X29, #48] ; [JIT_STORE] ; Saved Reg: X19 @ FP+48
    STR X20, [X29, #56] ; [JIT_STORE] ; Saved Reg: X20 @ FP+56
    STR X0, [X29, #32] ; A
    STR X1, [X29, #40] ; B
    LDR X21, [X29, #32]
    LDR X22, [X29, #40]
    MOV X24, X21
    MUL X24, X24, X22
    MOV X0, X24
    LDR X19, [X29, #48] ; [JIT_RESTORE] ; Restored Reg: X19 @ FP+48
    LDR X20, [X29, #56] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+56
    LDR X10, [X29, #16] ; Load Upper Stack Canary for check
    MOVZ X11, #30600
    MOVK X11, #21862, LSL #16
    MOVK X11, #13124, LSL #32
    MOVK X11, #4386, LSL #48 ; Load Expected UPPER_CANARY_VALUE
    CMP X10, X11 ; Compare Upper Canary
    B.NE L_MULTIPLY_stackprot_upper ; Branch if Upper Canary Corrupted
    LDR X10, [X29, #24] ; Load Lower Stack Canary for check
    MOVZ X11, #17
    MOVK X11, #61183, LSL #16
    MOVK X11, #52445, LSL #32
    MOVK X11, #43707, LSL #48 ; Load Expected LOWER_CANARY_VALUE
    CMP X10, X11 ; Compare Lower Canary
    B.NE L_MULTIPLY_stackprot_lower ; Branch if Lower Canary Corrupted
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #64 ; Deallocate space for saved FP/LR
    RET
L_MULTIPLY_stackprot_upper:
    BRK #0 ; Stack Corruption Detected for Upper Canary!
L_MULTIPLY_stackprot_lower:
    BRK #0 ; Stack Corruption Detected for Lower Canary!
L_CALCULATE:
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
    STR X19, [X29, #56] ; [JIT_STORE] ; Saved Reg: X19 @ FP+56
    STR X20, [X29, #64] ; [JIT_STORE] ; Saved Reg: X20 @ FP+64
    STR X0, [X29, #32]
    STR X1, [X29, #40] ; Y
    STR X2, [X29, #48] ; Z
    LDR X25, [X29, #32]
    MOV X26, X25
    LDR X27, [X29, #40]
    MOV X23, X27
    MOV X0, X26
    MOV X1, X23
    BL L_ADD
    MOV X21, X0
    LDR X22, [X29, #48]
    MOV X24, X22
    MOV X0, X21
    MOV X1, X24
    BL L_MULTIPLY
    LDR X19, [X29, #56] ; [JIT_RESTORE] ; Restored Reg: X19 @ FP+56
    LDR X20, [X29, #64] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+64
    LDR X10, [X29, #16] ; Load Upper Stack Canary for check
    MOVZ X11, #30600
    MOVK X11, #21862, LSL #16
    MOVK X11, #13124, LSL #32
    MOVK X11, #4386, LSL #48 ; Load Expected UPPER_CANARY_VALUE
    CMP X10, X11 ; Compare Upper Canary
    B.NE L_CALCULATE_stackprot_upper ; Branch if Upper Canary Corrupted
    LDR X10, [X29, #24] ; Load Lower Stack Canary for check
    MOVZ X11, #17
    MOVK X11, #61183, LSL #16
    MOVK X11, #52445, LSL #32
    MOVK X11, #43707, LSL #48 ; Load Expected LOWER_CANARY_VALUE
    CMP X10, X11 ; Compare Lower Canary
    B.NE L_CALCULATE_stackprot_lower ; Branch if Lower Canary Corrupted
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #80 ; Deallocate space for saved FP/LR
    RET
L_CALCULATE_stackprot_upper:
    BRK #0 ; Stack Corruption Detected for Upper Canary!
L_CALCULATE_stackprot_lower:
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
    MOVZ X10, #2
    MOV X25, X10
    MOVZ X10, #3
    MOV X26, X10
    MOVZ X10, #4
    MOV X27, X10
    MOV X0, X25
    MOV X1, X26
    MOV X2, X27
    BL L_CALCULATE
    STR X0, [X29, #32] ; RESULT
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X23, X10
    MOV X0, X23
    BL _WRITES
    LDR X21, [X29, #32]
    MOV X22, X21
    MOV X0, X22
    BL _WRITEN
    MOVZ X10, #10
    MOV X24, X10
    MOV X0, X24
    BL _WRITEC
    MOVZ X10, #5
    MOV X25, X10
    MOVZ X10, #5
    MOV X26, X10
    MOVZ X10, #10
    MOV X27, X10
    MOV X0, X25
    MOV X1, X26
    MOV X2, X27
    BL L_CALCULATE
    STR X0, [X29, #32] ; RESULT
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    MOV X23, X10
    MOV X0, X23
    BL _WRITES
    LDR X21, [X29, #32]
    MOV X22, X21
    MOV X0, X22
    BL _WRITEN
    MOVZ X10, #10
    MOV X24, X10
    MOV X0, X24
    BL _WRITEC
    MOV X0, X10
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
    
    .long 0xc
    .long 0x65
    .long 0x78
    .long 0x70
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x20
    .long 0x32
    .long 0x30
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x0
    .long 0x0
L_str1:
    
    .long 0xd
    .long 0x65
    .long 0x78
    .long 0x70
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x20
    .long 0x31
    .long 0x30
    .long 0x30
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x0
    .long 0x0
    .long 0x0, 0x0, 0x0, 0x0

.section __DATA,__data
.p2align 3
