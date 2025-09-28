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
    STP X29, X30, [SP, #-112]!
    MOV X29, SP
    STR X20, [X29, #72] ; [JIT_STORE] ; Saved Reg: X20 @ FP+72
    STR X21, [X29, #80] ; Saved Reg: X21 @ FP+80
    STR X22, [X29, #88] ; Saved Reg: X22 @ FP+88
    STR X23, [X29, #96] ; Saved Reg: X23 @ FP+96
L_START_Entry_0:
    ADRP X11, L_float0@PAGE
    ADD X11, X11, L_float0@PAGEOFF
    LDR D0, [X11, #0]
    FSQRT D1, D0
    STR D1, [X29, #24]
    LDR D8, [X29, #24]
    FMOV D0, D8
    BL _WRITEF
    MOVZ X10, #10
    MOV X0, X10
    BL _WRITEC
    LDR D8, [X29, #24]
    FCVTZS X10, D8
    STR X10, [X29, #32]
    MOV x0, x10
    BL _WRITEN
    MOVZ X10, #10
    MOV X0, X10
    BL _WRITEC
    LDR D8, [X29, #24]
    FCVTMS X10, D8
    STR X10, [X29, #16]
    MOV x0, x10
    BL _WRITEN
    MOVZ X10, #10
    MOV X0, X10
    BL _WRITEC
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X0, X10
    BL _WRITES
    B L_START_Exit_1
L_START_Exit_1:
    B L_0
L_0:
    LDR X20, [X29, #72] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+72
    LDR X21, [X29, #80] ; Restored Reg: X21 @ FP+80
    LDR X22, [X29, #88] ; Restored Reg: X22 @ FP+88
    LDR X23, [X29, #96] ; Restored Reg: X23 @ FP+96
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __TEXT,__const
.p2align 3
L_str0:
    
    .long 0x1
    .long 0xa
    .long 0x0
    .long 0x0
    .long 0x0, 0x0, 0x0, 0x0
L_float0:
    
    .quad 0x405e200000000000
    ; (upper half)

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
