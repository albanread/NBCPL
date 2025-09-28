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
    STP X29, X30, [SP, #-80]!
    MOV X29, SP
    STR X20, [X29, #56] ; [JIT_STORE] ; Saved Reg: X20 @ FP+56
    STR X21, [X29, #64] ; Saved Reg: X21 @ FP+64
    STR X22, [X29, #72] ; Saved Reg: X22 @ FP+72
L_START_Entry_0:
    B L_START_BB_1
L_START_BB_1:
    MOVZ X10, #0
    STR X10, [X29, #16]
    MOVZ X10, #2
    STR X10, [X29, #16]
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X0, X10
    BL _WRITES
    MOVZ X10, #2
    MOV X0, X10
    BL _WRITEN
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    MOV X0, X10
    BL _WRITES
    MOVZ X10, #65535
    MOVK X10, #65535, LSL #16
    MOVK X10, #65535, LSL #32
    MOVK X10, #65535, LSL #48
    MOV X0, X10
    BL _WRITEN
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    MOV X0, X10
    BL _WRITES
    MOVZ X10, #65535
    MOVK X10, #65535, LSL #16
    MOVK X10, #65535, LSL #32
    MOVK X10, #65535, LSL #48
    CMP X10, XZR
    B.EQ L_START_WhileExit_3
    B L_START_WhileBody_2
L_START_BB_5:
    B L_START_Exit_4
L_START_Exit_4:
    B L_0
L_START_WhileBody_2:
    MOVZ X10, #3
    STR X10, [X29, #24]
    MOV x20, x10
    STR X20, [X29, #16]
    ADRP X10, L_str2@PAGE
    ADD X10, X10, L_str2@PAGEOFF
    MOV X0, X10
    BL _WRITES
    B L_START_BB_1
L_START_WhileExit_3:
    MOVZ X10, #3
    MOV X0, X10
    BL _WRITEN
    MOVZ X10, #3
    MOV X0, X10
    B L_0
    B L_START_Exit_4
L_0:
    LDR X20, [X29, #56] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+56
    LDR X21, [X29, #64] ; Restored Reg: X21 @ FP+64
    LDR X22, [X29, #72] ; Restored Reg: X22 @ FP+72
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __TEXT,__const
.p2align 3
L_str0:
    
    .long 0xb
    .long 0x49
    .long 0x6e
    .long 0x69
    .long 0x74
    .long 0x69
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x49
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
    
    .long 0x8
    .long 0x20
    .long 0x68
    .long 0x65
    .long 0x6c
    .long 0x6c
    .long 0x6f
    .long 0x20
    .long 0xa
    .long 0x0
    .long 0x0
    .long 0x0, 0x0, 0x0, 0x0

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
