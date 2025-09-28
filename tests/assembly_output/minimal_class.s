.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _RDCH
.globl _BCPL_LIST_GET_REST
.globl _CONCAT
.globl _FREEVEC
.globl _RUNTIME_METHOD_LOOKUP
.globl _BCPL_LIST_GET_HEAD_AS_INT
.globl _OBJECT_HEAP_FREE
.globl _BCPL_LIST_APPEND_STRING
.globl _OBJECT_HEAP_ALLOC
.globl _SETTYPE
.globl _BCPL_LIST_APPEND_FLOAT
.globl _STRCOPY
.globl _BCPL_ALLOC_CHARS
.globl _BCPL_LIST_GET_TAIL
.globl _BCPL_LIST_GET_NTH
.globl _PIC_RUNTIME_HELPER
.globl _COPYLIST
.globl _MALLOC
.globl _BCPL_LIST_CREATE_EMPTY
.globl _FILTER
.globl _UNPACKSTRING
.globl _BCPL_GET_ATOM_TYPE
.globl _LPND
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _BCPL_FREE_LIST
.globl _NEWLINE
.globl _BCPL_CONCAT_LISTS
.globl _BCPL_FREE_CELLS
.globl _FPND
.globl _WRITEF
.globl _BCPL_LIST_APPEND_INT
.globl _RETURNNODETOFREELIST
.globl _APND
.globl _DEEPCOPYLIST
.globl _FINISH
.globl _WRITEC
.globl _WRITEN
.globl _SPLIT
.globl _SPIT
.globl _BCPL_LIST_GET_HEAD_AS_FLOAT
.globl _JOIN
.globl _PACKSTRING
.globl _DEEPCOPYLITERALLIST
.globl _STRLEN
.globl _SPND
.globl _FIND
.globl _STRCMP
.globl _REVERSE
.globl _SLURP
.globl _BCPL_ALLOC_WORDS
.globl _WRITES
.p2align 2
_start:
_START:
    B L_START
L_Simple::set:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STR X19, [X29, #32] ; Saved Reg: X19 @ FP+32
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
    STR X0, [X29, #16] ; val
L_Simple::set_Entry_0:
    LDR X10, [X29, #16] ; Load 'this' pointer for store
    STR X0, [X10] ; Store to member x
    B L_Simple::set_Exit_1
L_Simple::set_Exit_1:
    B L_0
L_0:
    LDR X19, [X29, #32] ; Restored Reg: X19 @ FP+32
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Simple::get:
    STP X29, X30, [SP, #-32]!
    MOV X29, SP
    STR X19, [X29, #16] ; Saved Reg: X19 @ FP+16
    STR X28, [X29, #24] ; Saved Reg: X28 @ FP+24
L_Simple::get_Entry_0:
    LDR X10, [X29, #16] ; Load 'this' pointer
    LDR X11, [X10] ; Load member x
    MOV X0, X11
    B L_1
    B L_Simple::get_Exit_1
L_Simple::get_Exit_1:
    B L_1
L_1:
    LDR X19, [X29, #16] ; Restored Reg: X19 @ FP+16
    LDR X28, [X29, #24] ; Restored Reg: X28 @ FP+24
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STR X19, [X29, #24] ; Saved Reg: X19 @ FP+24
    STR X20, [X29, #32] ; Saved Reg: X20 @ FP+32
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X10, #8, LSL #16
    ADD X19, X28, X10
L_START_Entry_0:
    MOVZ X10, #8
    MOV X0, X10
    BL _OBJECT_HEAP_ALLOC
    MOV X10, X0
    ADRP X11, L_Simple_vtable@PAGE
    ADD X11, X11, L_Simple_vtable@PAGEOFF
    STR X11, [X10] ; store vtable ptr at offset 0
    STR X10, [X29, #16] ; s
    LDR X20, [X29, #16] ; s
    MOV X21, X20
    MOV X0, X21
    BL _WRITEN
    MOVZ X10, #42
    MOV X22, X10
    LDR X23, [X29, #16] ; s
    LDR X11, [X23] ; Load vtable pointer for method call
    LDR X12, [X11] ; Load method address for set
    MOV X0, X23
    MOV X1, X22
    BLR X12
    LDR X24, [X29, #16] ; s
    MOV X0, X24
    LDR X10, [X0] ; Load vtable pointer
    LDR X11, [X10] ; Load method address for get
    MOV X25, X0
    MOV X0, X25
    BL _WRITEN
    B L_START_Exit_1
L_START_Exit_1:
    B L_2
L_2:
    LDR X19, [X29, #24] ; Restored Reg: X19 @ FP+24
    LDR X20, [X29, #32] ; Restored Reg: X20 @ FP+32
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __TEXT,__const
.p2align 3
Simple_vtable:
    .quad Simple::set
    .quad Simple::get

.section __DATA,__data
.p2align 3
L__data_segment_base:
