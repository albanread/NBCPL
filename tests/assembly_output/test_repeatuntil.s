.section __TEXT,__text,regular,pure_instructions
.globl _start
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
    STR X19, [X29, #48] ; [JIT_STORE] ; Saved Reg: X19 @ FP+48
    STR X20, [X29, #56] ; [JIT_STORE] ; Saved Reg: X20 @ FP+56
L_START_Entry_0:
    MOVZ X10, #0
    STR X10, [X29, #40]
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X21, X10
    MOV X0, X21
    BL _WRITES
    LDR X22, [X29, #40]
    MOVZ X10, #1
    MOV X11, X22
    ADD X11, X22, X10
    STR X11, [X29, #40]
L_1:
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X21, X10
    MOV X0, X21
    BL _WRITES
    LDR X22, [X29, #40]
    MOVZ X10, #1
    MOV X11, X22
    ADD X11, X22, X10
    STR X11, [X29, #40]
    LDR X23, [X29, #40]
    MOVZ X10, #3
    CMP X23, X10
    B.NE L_1
L_2:
    LDR X24, [X29, #40]
    MOVZ X10, #3
    MOV X11, X24
    CMP X24, X10
    CSETM X11, EQ
    CMP X11, XZR
    B.NE L_START_Entry_0
    B L_START_RepeatExit_1
L_START_Exit_2:
    B L_0
L_START_RepeatExit_1:
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    MOV X21, X10
    MOV X0, X21
    BL _WRITES
    B L_START_Exit_2
L_0:
    LDR X19, [X29, #48] ; [JIT_RESTORE] ; Restored Reg: X19 @ FP+48
    LDR X20, [X29, #56] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+56
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
    
    .long 0xb
    .long 0x68
    .long 0x65
    .long 0x6c
    .long 0x6c
    .long 0x6f
    .long 0x20
    .long 0x77
    .long 0x6f
    .long 0x72
    .long 0x6c
    .long 0x64
    .long 0xa
    .long 0x0
L_str1:
    
    .long 0x2d
    .long 0xa
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0x2d
    .long 0xa
    .long 0x0
    .long 0x0, 0x0, 0x0, 0x0

.section __DATA,__data
.p2align 3
