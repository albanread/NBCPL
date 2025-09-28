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
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    STR X19, [X29, #40] ; Saved Reg: X19 @ FP+40
    STR X20, [X29, #48] ; Saved Reg: X20 @ FP+48
    STR X28, [X29, #56] ; Saved Reg: X28 @ FP+56
    MOVZ X10, #8, LSL #16
    ADD X19, X28, X10
L_START_Entry_0:
    MOVZ X10, #0
    STR X10, [X29, #16] ; m
    MOVZ X10, #8
    STR X10, [X29, #24] ; w
    MOVZ X10, #200
    LDR X20, [X29, #16] ; m
    BFI X20, X10, #0, #8
    STR X20, [X29, #16] ; m
    MOV x21, x20
    MOVZ X10, #0
    LDR X22, [X29, #24] ; w
    LSR X11, X21, X10
    MOVZ X13, #1
    LSL X12, X13, X22
    SUB X12, X12, #1
    AND X11, X11, X12
    STR X11, [X29, #32] ; x
    MOV x0, x11
    BL _WRITEN
    B L_START_Exit_1
L_START_Exit_1:
    B L_0
L_0:
    LDR X19, [X29, #40] ; Restored Reg: X19 @ FP+40
    LDR X20, [X29, #48] ; Restored Reg: X20 @ FP+48
    LDR X28, [X29, #56] ; Restored Reg: X28 @ FP+56
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
