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
L_CircleArea:
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
    STR D0, [X29, #32]
L_CircleArea_Entry_0:
    B L_CircleArea_VALOF_BODY1
L_CircleArea_Exit_2:
    B L_0
L_CircleArea_VALOF_BODY1:
    ADRP X11, L_float0@PAGE
    ADD X11, X11, L_float0@PAGEOFF
    LDR D0, [X11, #0]
    LDR D8, [X29, #32]
    FMUL D1, D0, D8
    LDR D9, [X29, #32]
    FMUL D0, D1, D9
    B L_0
    B L_CircleArea_Exit_2
L_CircleArea_VALOF_CONT3:
    B L_CircleArea_Exit_2
L_0:
    LDR X19, [X29, #40] ; [JIT_RESTORE] ; Restored Reg: X19 @ FP+40
    LDR X20, [X29, #48] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+48
    LDR X10, [X29, #16] ; Load Upper Stack Canary for check
    MOVZ X11, #30600
    MOVK X11, #21862, LSL #16
    MOVK X11, #13124, LSL #32
    MOVK X11, #4386, LSL #48 ; Load Expected UPPER_CANARY_VALUE
    CMP X10, X11 ; Compare Upper Canary
    B.NE L_CircleArea_stackprot_upper ; Branch if Upper Canary Corrupted
    LDR X10, [X29, #24] ; Load Lower Stack Canary for check
    MOVZ X11, #17
    MOVK X11, #61183, LSL #16
    MOVK X11, #52445, LSL #32
    MOVK X11, #43707, LSL #48 ; Load Expected LOWER_CANARY_VALUE
    CMP X10, X11 ; Compare Lower Canary
    B.NE L_CircleArea_stackprot_lower ; Branch if Lower Canary Corrupted
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #64 ; Deallocate space for saved FP/LR
    RET
L_CircleArea_stackprot_upper:
    BRK #0 ; Stack Corruption Detected for Upper Canary!
L_CircleArea_stackprot_lower:
    BRK #0 ; Stack Corruption Detected for Lower Canary!
L_START:
    STP X29, X30, [SP, #-128]!
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
    STR X19, [X29, #104] ; [JIT_STORE] ; Saved Reg: X19 @ FP+104
    STR X20, [X29, #112] ; [JIT_STORE] ; Saved Reg: X20 @ FP+112
L_START_Entry_0:
    B L_START_BB_1
L_START_BB_1:
    ADRP X12, L_float1@PAGE
    ADD X12, X12, L_float1@PAGEOFF
    LDR D1, [X12, #0]
    STR D1, [X29, #96]
    LDR D10, [X29, #96]
    FMOV D11, D10
    FMOV D0, D11
    BL L_CircleArea
    STR D0, [X29, #64]
    ADRP X13, L_float2@PAGE
    ADD X13, X13, L_float2@PAGEOFF
    LDR D0, [X13, #0]
    STR D0, [X29, #88]
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X21, X10
    MOV X0, X21
    BL _WRITES
    LDR D8, [X29, #64]
    FMOV D9, D8
    FMOV D0, D9
    BL _WRITEF
    MOVZ X10, #10
    MOV X21, X10
    MOV X0, X21
    BL _WRITEC
    LDR D8, [X29, #88]
    LDR D9, [X29, #64]
    FMUL D0, D8, D9
    LDR D10, [X29, #96]
    FADD D1, D0, D10
    LDR X22, [X29, #80]
    SCVTF D2, X22
    FSUB D0, D1, D2
    STR D0, [X29, #72]
    LDR D8, [X29, #72]
    FMOV D9, D8
    FMOV D0, D9
    BL _WRITEF
    MOVZ X10, #10
    MOV X21, X10
    MOV X0, X21
    BL _WRITEC
    B L_START_BB_2
L_START_BB_2:
    B L_START_Exit_3
L_START_Exit_3:
    B L_1
L_1:
    LDR X19, [X29, #104] ; [JIT_RESTORE] ; Restored Reg: X19 @ FP+104
    LDR X20, [X29, #112] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+112
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
    ADD SP, SP, #128 ; Deallocate space for saved FP/LR
    RET
L_START_stackprot_upper:
    BRK #0 ; Stack Corruption Detected for Upper Canary!
L_START_stackprot_lower:
    BRK #0 ; Stack Corruption Detected for Lower Canary!
    nop
    nop

.section __TEXT,__const
.p2align 3
L_str0:
    
    .long 0x21
    .long 0x52
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x20
    .long 0x6f
    .long 0x66
    .long 0x20
    .long 0x43
    .long 0x20
    .long 0x28
    .long 0x66
    .long 0x75
    .long 0x6e
    .long 0x63
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x20
    .long 0x63
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x29
    .long 0x3a
    .long 0x20
    .long 0x0
    .long 0x0, 0x0, 0x0, 0x0
L_float0:
    
    .quad 0x400921f9f01b866e
    ; (upper half)
L_float1:
    
    .quad 0x40283d70a3d70a3d
    ; (upper half)
L_float2:
    
    .quad 0x3fbf7ced916872b0
    ; (upper half)

.section __DATA,__data
.p2align 3
