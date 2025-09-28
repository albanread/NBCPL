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
    STR X19, [X29, #24] ; [JIT_STORE] ; Saved Reg: X19 @ FP+24
    STR X20, [X29, #32] ; [JIT_STORE] ; Saved Reg: X20 @ FP+32
    STR X21, [X29, #40] ; Saved Reg: X21 @ FP+40
    STR X22, [X29, #48] ; Saved Reg: X22 @ FP+48
    STR X23, [X29, #56] ; Saved Reg: X23 @ FP+56
    STR X24, [X29, #64] ; Saved Reg: X24 @ FP+64
L_START_Entry_0:
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    MOV X21, X10
    MOV X0, X21
    BL _WRITES
    LDR X22, [X29, #16]
    MOV X23, X22
    MOV X0, X23
    BL _WRITEN
    MOVZ X10, #10
    MOV X24, X10
    MOV X0, X24
    BL _WRITEC
    B L_START_Exit_1
L_START_Exit_1:
    B L_0
L_0:
    LDR X19, [X29, #24] ; [JIT_RESTORE] ; Restored Reg: X19 @ FP+24
    LDR X20, [X29, #32] ; [JIT_RESTORE] ; Restored Reg: X20 @ FP+32
    LDR X21, [X29, #40] ; Restored Reg: X21 @ FP+40
    LDR X22, [X29, #48] ; Restored Reg: X22 @ FP+48
    LDR X23, [X29, #56] ; Restored Reg: X23 @ FP+56
    LDR X24, [X29, #64] ; Restored Reg: X24 @ FP+64
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #80 ; Deallocate space for saved FP/LR
    RET

.section __TEXT,__const
.p2align 3
L_str0:
    
    .long 0xf
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
    .long 0x21
    .long 0x21
    .long 0x21
    .long 0x21
    .long 0xa
    .long 0x0
    .long 0x0, 0x0, 0x0, 0x0

.section __DATA,__data
.p2align 3
