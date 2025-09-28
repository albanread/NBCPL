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
L_SNAFU:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    STR X19, [X29, #24] ; Saved Reg: X19 @ FP+24
    STR X20, [X29, #32] ; Saved Reg: X20 @ FP+32
    STR X21, [X29, #40] ; Saved Reg: X21 @ FP+40
    STR X22, [X29, #48] ; Saved Reg: X22 @ FP+48
    STR X28, [X29, #56] ; Saved Reg: X28 @ FP+56
    MOVZ X10, #8, LSL #16
    ADD X19, X28, X10
L_SNAFU_Entry_0:
    MOVZ X10, #1
    STR X10, [X29, #16] ; J_for_var_0
    B L_SNAFU_ForHeader_1
L_SNAFU_Exit_5:
    B L_0
L_SNAFU_ForBody_2:
    LDR X20, [X29, #16] ; J_for_var_0
    MOV X0, X20
    BL _WRITEN
    MOVZ X10, #32
    MOV X0, X10
    BL _WRITEC
    B L_SNAFU_ForIncrement_3
L_SNAFU_ForExit_4:
    MOVZ X10, #10
    MOV X0, X10
    BL _WRITEC
    B L_SNAFU_Exit_5
L_SNAFU_ForHeader_1:
    LDR X10, [X29, #16] ; J_for_var_0
    MOVZ X11, #120
    CMP X10, X11
    B.GT L_SNAFU_ForExit_4
    B L_SNAFU_ForBody_2
L_SNAFU_ForIncrement_3:
    MOVZ X10, #1
    LDR X21, [X29, #16] ; J_for_var_0
    ADD X21, X21, X10
    STR X21, [X29, #16] ; J_for_var_0
    B L_SNAFU_ForHeader_1
L_0:
    LDR X19, [X29, #24] ; Restored Reg: X19 @ FP+24
    LDR X20, [X29, #32] ; Restored Reg: X20 @ FP+32
    LDR X21, [X29, #40] ; Restored Reg: X21 @ FP+40
    LDR X22, [X29, #48] ; Restored Reg: X22 @ FP+48
    LDR X28, [X29, #56] ; Restored Reg: X28 @ FP+56
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-32]!
    MOV X29, SP
    STR X19, [X29, #16] ; Saved Reg: X19 @ FP+16
    STR X28, [X29, #24] ; Saved Reg: X28 @ FP+24
L_START_Entry_0:
    BL L_SNAFU
    MOVZ X10, #0
    MOV X0, X10
    MOVZ X10, #1
    MOVK X10, #512, LSL #16
    MOV X16, X10
    SVC #128
    B L_START_Exit_1
L_START_Exit_1:
    B L_1
L_1:
    LDR X19, [X29, #16] ; Restored Reg: X19 @ FP+16
    LDR X28, [X29, #24] ; Restored Reg: X28 @ FP+24
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
