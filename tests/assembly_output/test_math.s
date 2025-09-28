.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _WRITES
.globl _WRITEN
.globl _BCPL_ALLOC_CHARS
.globl _PACKSTRING
.globl _STRCMP
.globl _RDCH
.globl _STRCOPY
.globl _WRITEF
.globl _SPIT
.globl _MALLOC
.globl _BCPL_ALLOC_WORDS
.globl _FINISH
.globl _WRITEC
.globl _SLURP
.globl _STRLEN
.globl _UNPACKSTRING
.globl _FREEVEC
.p2align 2
_start:
_START:
    B L_START
L_START:
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
    STR X19, [X29, #80] ; [JIT_STORE] ; Saved Reg: X19 @ FP+80
    STR X20, [X29, #88] ; [JIT_STORE] ; Saved Reg: X20 @ FP+88
L_START_Entry_0:
    B L_START_BB_1
L_START_BB_1:
    MOVZ X10, #10
    STR X10, [X29, #56]
    MOVZ X10, #5
    STR X10, [X29, #64]
    MOVZ X10, #2
    STR X10, [X29, #72]
    LDR X21, [X29, #56]
    LDR X22, [X29, #64]
    MOV X10, X21
    ADD X10, X21, X22
    STR X10, [X29, #72]
    LDR X21, [X29, #72]
    MOV X22, X21
    MOV X0, X22
    BL _WRITEN
    LDR X23, [X29, #56]
    LDR X24, [X29, #64]
    MOV X10, X23
    SUB X10, X23, X24
    STR X10, [X29, #72]
    LDR X21, [X29, #72]
    MOV X22, X21
    MOV X0, X22
    BL _WRITEN
    LDR X23, [X29, #56]
    LDR X24, [X29, #64]
    MOV X10, X23
    MUL X10, X23, X24
    STR X10, [X29, #72]
    LDR X21, [X29, #72]
    MOV X22, X21
    MOV X0, X22
    BL _WRITEN
    LDR X23, [X29, #56]
    LDR X24, [X29, #64]
    MOV X10, X23
    SDIV X10, X23, X24
    STR X10, [X29, #72]
    LDR X21, [X29, #72]
    MOV X22, X21
    MOV X0, X22
    BL _WRITEN
    LDR X23, [X29, #56]
    LDR X24, [X29, #64]
    MOV X10, X23
    SDIV X11, X23, X24
    MUL X11, X11, X24
    SUB X10, X23, X11
    STR X10, [X29, #72]
    LDR X21, [X29, #72]
    MOV X22, X21
    MOV X0, X22
    BL _WRITEN
    MOVZ X10, #10
    SUB X10, XZR, X10
    STR X10, [X29, #56]
    MOVZ X10, #3
    STR X10, [X29, #64]
    LDR X23, [X29, #56]
    LDR X24, [X29, #64]
    MOV X10, X23
    SDIV X10, X23, X24
    STR X10, [X29, #72]
    LDR X21, [X29, #72]
    MOV X22, X21
    MOV X0, X22
    BL _WRITEN
    LDR X23, [X29, #56]
    LDR X24, [X29, #64]
    MOV X10, X23
    SDIV X11, X23, X24
    MUL X11, X11, X24
    SUB X10, X23, X11
    STR X10, [X29, #72]
    LDR X21, [X29, #72]
    MOV X22, X21
    MOV X0, X22
    BL _WRITEN
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X21, X10
    MOV X0, X21
    BL _WRITES
    B L_START_BB_2
L_START_BB_2:
    B L_START_Exit_3
L_START_Exit_3:
    B L_0
L_0:
    LDR X19, [X29, #80] ; [JIT_RESTORE] ; Restored Reg: X19 @ FP+80
    LDR X20, [X29, #88] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+88
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
    ADD SP, SP, #96 ; Deallocate space for saved FP/LR
    RET
L_START_stackprot_upper:
    BRK #0 ; Stack Corruption Detected for Upper Canary!
L_START_stackprot_lower:
    BRK #0 ; Stack Corruption Detected for Lower Canary!

.section __TEXT,__const
.p2align 3
L_str0:
    
    .long 0x0
    .long 0xa
    .long 0x0
    .long 0x0, 0x0, 0x0, 0x0
