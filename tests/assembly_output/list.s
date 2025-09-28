.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _BCPL_ALLOC_CHARS
.globl _STRCOPY
.globl _BCPL_ALLOC_WORDS
.globl _STRLEN
.globl _BCPL_LIST_GET_TAIL
.globl _BCPL_LIST_GET_HEAD_AS_FLOAT
.globl _BCPL_LIST_GET_HEAD_AS_INT
.globl _APND
.globl _RUNTIME_GET_ATOM_TYPE
.globl _BCPL_LIST_APPEND_FLOAT
.globl _FREEVEC
.globl _LPND
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _GETNODEFROMFREELIST
.globl _getNodeFromFreelist
.globl _RDCH
.globl _SPND
.globl _STRCMP
.globl _WRITES
.globl _BCPL_LIST_CREATE_EMPTY
.globl _WRITEF
.globl _FPND
.globl _BCPL_FREE_CELLS
.globl _BCPL_LIST_APPEND_STRING
.globl _UNPACKSTRING
.globl _RETURNNODETOFREELIST
.globl _BCPL_LIST_APPEND_INT
.globl _MALLOC
.globl _WRITEN
.globl _SPIT
.globl _FINISH
.globl _PACKSTRING
.globl _WRITEC
.globl _BCPL_FREE_LIST
.globl _SLURP
.p2align 2
_start:
_START:
    B L_START
L_START:
    STP X29, X30, [SP, #-80]!
    MOV X29, SP
    STR X19, [X29, #40] ; Saved Reg: X19 @ FP+40
    STR X20, [X29, #48] ; Saved Reg: X20 @ FP+48
    STR X21, [X29, #56] ; Saved Reg: X21 @ FP+56
    STR X22, [X29, #64] ; Saved Reg: X22 @ FP+64
    STR X28, [X29, #72] ; Saved Reg: X28 @ FP+72
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X10, #8, LSL #16
    ADD X19, X28, X10
L_START_Entry_0:
    BL _BCPL_LIST_CREATE_EMPTY
    MOV X10, X0
    LDR X11, [X10, #8]
    MOVZ X12, #10
    BL _getNodeFromFreelist
    MOV X13, X0
    STR X12, [X13, #8]
    STR XZR, [X13, #16]
    CMP X11, #0
    B.EQ L_1
    STR X13, [X11, #16]
    B L_2
L_1:
    STR X13, [X10, #16]
L_2:
    MOV X11, X13
    STR X11, [X10, #8]
    STR X10, [X29, #32] ; L
    LDR X20, [X29, #32]
    MOVZ X10, #60
    MOV X0, X20
    MOV X1, X10
    BL _APND
    LDR X21, [X29, #32]
    MOVZ X10, #70
    MOV X0, X21
    MOV X1, X10
    BL _APND
    LDR X22, [X29, #32]
    LDR X10, [X22, #16] ; Load rest of list pointer
    LDR X11, [X10, #16] ; Load head pointer from header
    CMP X11, #0
    B.EQ L_3
    LDR X12, [X11, #8] ; Load value from node
    B L_4
L_3:
    MOV X12, XZR
L_4:
    STR X12, [X29, #16] ; H
    LDR X23, [X29, #16] ; H
    MOV X0, X23
    BL _WRITEN
    MOVZ X10, #10
    MOV X0, X10
    BL _WRITEC
    LDR X24, [X29, #32]
    LDR X10, [X24, #16] ; Load head pointer from header
    CMP X10, #0
    B.EQ L_5
    LDR X11, [X10, #8] ; Load value from node
    B L_6
L_5:
    MOV X11, XZR
L_6:
    STR X11, [X29, #24] ; K
    LDR X25, [X29, #24] ; K
    MOV X0, X25
    BL _WRITEN
    B L_START_Exit_1
L_START_Exit_1:
    B L_0
L_0:
    LDR X19, [X29, #40] ; Restored Reg: X19 @ FP+40
    LDR X20, [X29, #48] ; Restored Reg: X20 @ FP+48
    LDR X21, [X29, #56] ; Restored Reg: X21 @ FP+56
    LDR X22, [X29, #64] ; Restored Reg: X22 @ FP+64
    LDR X28, [X29, #72] ; Restored Reg: X28 @ FP+72
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__data
.p2align 3
L__data_segment_base:
