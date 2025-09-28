.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _BCPL_ALLOC_CHARS
.globl _STRCOPY
.globl _RDCH
.globl _BCPL_ALLOC_WORDS
.globl _STRLEN
.globl _BCPL_LIST_GET_TAIL
.globl _BCPL_LIST_GET_HEAD_AS_FLOAT
.globl _BCPL_LIST_GET_HEAD_AS_INT
.globl _RUNTIME_GET_ATOM_TYPE
.globl _BCPL_LIST_APPEND_FLOAT
.globl _FREEVEC
.globl _LPND
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _SPND
.globl _STRCMP
.globl _BCPL_LIST_APPEND_INT
.globl _WRITEF
.globl _BCPL_FREE_CELLS
.globl _FPND
.globl _FINISH
.globl _BCPL_LIST_APPEND_STRING
.globl _UNPACKSTRING
.globl _WRITES
.globl _BCPL_LIST_CREATE_EMPTY
.globl _APND
.globl _RETURNNODETOFREELIST
.globl _WRITEN
.globl _SPIT
.globl _MALLOC
.globl _PACKSTRING
.globl _WRITEC
.globl _BCPL_FREE_LIST
.globl _SLURP
.p2align 2
_start:
_START:
    B L_START
L_FACTORIAL_RECURSIVE:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    STR X19, [X29, #32] ; Saved Reg: X19 @ FP+32
    STR X20, [X29, #40] ; Saved Reg: X20 @ FP+40
    STR X28, [X29, #48] ; Saved Reg: X28 @ FP+48
    STR X0, [X29, #16] ; N
    LDR X20, [X29, #16] ; N
L_FACTORIAL_RECURSIVE_Entry_0:
    MOVZ X10, #0
    CMP X20, X10
    CSET X11, EQ
    CMP X11, XZR
    B.EQ L_FACTORIAL_RECURSIVE_Join_2
    B L_FACTORIAL_RECURSIVE_Then_1
L_FACTORIAL_RECURSIVE_Join_2:
    MOVZ X10, #1
    SUB X11, X20, X10
    MOV X0, X11
    BL L_FACTORIAL_RECURSIVE
    MOV X10, X0
    MUL X11, X20, X10
    MOV X0, X11
    B L_0
    B L_FACTORIAL_RECURSIVE_Exit_3
L_FACTORIAL_RECURSIVE_Then_1:
    MOVZ X10, #1
    MOV X0, X10
    B L_0
    B L_FACTORIAL_RECURSIVE_Exit_3
L_FACTORIAL_RECURSIVE_Exit_3:
    B L_0
L_0:
    LDR X19, [X29, #32] ; Restored Reg: X19 @ FP+32
    LDR X20, [X29, #40] ; Restored Reg: X20 @ FP+40
    LDR X28, [X29, #48] ; Restored Reg: X28 @ FP+48
    LDP X29, X30, [SP], #64 ; Restore FP, LR, and deallocate frame
    RET
L_START:
    STP X29, X30, [SP, #-96]!
    MOV X29, SP
    STR X19, [X29, #48] ; Saved Reg: X19 @ FP+48
    STR X20, [X29, #56] ; Saved Reg: X20 @ FP+56
    STR X21, [X29, #64] ; Saved Reg: X21 @ FP+64
    STR X22, [X29, #72] ; Saved Reg: X22 @ FP+72
    STR X23, [X29, #80] ; Saved Reg: X23 @ FP+80
    STR X28, [X29, #88] ; Saved Reg: X28 @ FP+88
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X10, #8, LSL #16
    ADD X19, X28, X10
L_START_Entry_0:
    MOVZ X10, #19
    MOV X0, X10
    BL L_FACTORIAL_RECURSIVE
    MOV X10, X0
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    MOV X0, X10
    BL _WRITEN
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    MOV X0, X10
    BL _WRITEN
    MOVZ X10, #10
    MOV X0, X10
    BL _WRITEC
    B L_START_Exit_1
L_START_Exit_1:
    B L_1
L_1:
    LDR X19, [X29, #48] ; Restored Reg: X19 @ FP+48
    LDR X20, [X29, #56] ; Restored Reg: X20 @ FP+56
    LDR X21, [X29, #64] ; Restored Reg: X21 @ FP+64
    LDR X22, [X29, #72] ; Restored Reg: X22 @ FP+72
    LDR X23, [X29, #80] ; Restored Reg: X23 @ FP+80
    LDR X28, [X29, #88] ; Restored Reg: X28 @ FP+88
    LDP X29, X30, [SP], #96 ; Restore FP, LR, and deallocate frame
    RET

.section __TEXT,__const
.p2align 3
L_str0:
    
    .quad 0x23
    ; (upper half)
    .long 0x43
    .long 0x61
    .long 0x6c
    .long 0x63
    .long 0x75
    .long 0x6c
    .long 0x61
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x63
    .long 0x75
    .long 0x72
    .long 0x73
    .long 0x69
    .long 0x76
    .long 0x65
    .long 0x20
    .long 0x46
    .long 0x61
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x69
    .long 0x61
    .long 0x6c
    .long 0x20
    .long 0x6f
    .long 0x66
    .long 0x20
    .long 0x0
    .long 0x0
L_str1:
    
    .quad 0x3
    ; (upper half)
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x0
    .long 0x0

.section __DATA,__data
.p2align 3
L__data_segment_base:
