.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _WRITES
.globl _WRITEN
.globl _WRITEC
.globl _STRCOPY
.globl _BCPL_ALLOC_CHARS
.globl _FLOAT
.globl _FREEVEC
.globl _MALLOC
.globl _ISNUMERIC
.globl _BCPL_ALLOC_WORDS
.globl _RDCH
.globl _STRCMP
.globl _VAL
.globl _WRITEF
.globl _STRCAT
.globl _STRPOS
.globl _STRLEN
.globl _STRVAL
.globl _SUBSTR
.globl _FINDCHAR
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
L_START_Entry_0:
    ADRP X11, L_float0@PAGE
    ADD X11, X11, L_float0@PAGEOFF
    LDR D0, [X11, #0]
    STR D0, [X29, #48]
    LDR D8, [X29, #48]
    ADRP X12, L_float1@PAGE
    ADD X12, X12, L_float1@PAGEOFF
    LDR D0, [X12, #0]
    FMUL D1, D8, D0
    STR D1, [X29, #56]
    LDR D9, [X29, #48]
    ADRP X13, L_float2@PAGE
    ADD X13, X13, L_float2@PAGEOFF
    LDR D0, [X13, #0]
    FADD D1, D9, D0
    STR D1, [X29, #48]
    LDR D8, [X29, #56]
    FMOV D9, D8
    FMOV D0, D9
    BL _WRITEF
    B L_START_Exit_1
L_START_Exit_1:
    B L_0
L_0:
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
    nop
    nop
    nop

.section __TEXT,__const
.p2align 3
L_float0:
    
    .quad 0x3ff0000000000000
    ; (upper half)
L_float1:
    
    .quad 0x4024000000000000
    ; (upper half)
L_float2:
    
    .quad 0x4000000000000000
    ; (upper half)
