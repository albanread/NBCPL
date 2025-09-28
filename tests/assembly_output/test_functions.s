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
L_FACT:
    STP X29, X30, [SP, #-128]!
    MOV X29, SP
    STR X19, [X29, #40] ; Saved Reg: X19 @ FP+40
    STR X20, [X29, #48] ; Saved Reg: X20 @ FP+48
    STR X21, [X29, #56] ; Saved Reg: X21 @ FP+56
    STR X22, [X29, #64] ; Saved Reg: X22 @ FP+64
    STR X23, [X29, #72] ; Saved Reg: X23 @ FP+72
    STR X24, [X29, #80] ; Saved Reg: X24 @ FP+80
    STR X25, [X29, #88] ; Saved Reg: X25 @ FP+88
    STR X26, [X29, #96] ; Saved Reg: X26 @ FP+96
    STR X27, [X29, #104] ; Saved Reg: X27 @ FP+104
    STR X28, [X29, #112] ; Saved Reg: X28 @ FP+112
    MOV X27, X0    // Move parameter 'N' from X0 to X27
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L_FACT_Entry_0:
    MOVZ X9, #1
    MOV X26, X9
    MOVZ X9, #1
    MOV X23, X9
    B L_FACT_ForHeader_1
L_FACT_Exit_5:
    B L_0
L_FACT_ForBody_2:
    MUL X26, X26, X23
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOV X0, X26
    BL _WRITEN
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    B L_FACT_ForIncrement_3
L_FACT_ForExit_4:
    MOV X0, X26
    B L_0
    B L_FACT_Exit_5
L_FACT_ForHeader_1:
    MOV X9, X23
    CMP X9, X27
    B.GT L_FACT_ForExit_4
    B L_FACT_ForBody_2
L_FACT_ForIncrement_3:
    MOVZ X9, #1
    ADD X23, X23, X9
    B L_FACT_ForHeader_1
L_0:
    LDR X19, [X29, #40] ; Restored Reg: X19 @ FP+40
    LDR X20, [X29, #48] ; Restored Reg: X20 @ FP+48
    LDR X21, [X29, #56] ; Restored Reg: X21 @ FP+56
    LDR X22, [X29, #64] ; Restored Reg: X22 @ FP+64
    LDR X23, [X29, #72] ; Restored Reg: X23 @ FP+72
    LDR X24, [X29, #80] ; Restored Reg: X24 @ FP+80
    LDR X25, [X29, #88] ; Restored Reg: X25 @ FP+88
    LDR X26, [X29, #96] ; Restored Reg: X26 @ FP+96
    LDR X27, [X29, #104] ; Restored Reg: X27 @ FP+104
    LDR X28, [X29, #112] ; Restored Reg: X28 @ FP+112
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-96]!
    MOV X29, SP
    STR X19, [X29, #32] ; Saved Reg: X19 @ FP+32
    STR X20, [X29, #40] ; Saved Reg: X20 @ FP+40
    STR X21, [X29, #48] ; Saved Reg: X21 @ FP+48
    STR X25, [X29, #56] ; Saved Reg: X25 @ FP+56
    STR X26, [X29, #64] ; Saved Reg: X26 @ FP+64
    STR X27, [X29, #72] ; Saved Reg: X27 @ FP+72
    STR X28, [X29, #80] ; Saved Reg: X28 @ FP+80
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L_START_Entry_0:
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOVZ X9, #12
    MOV X25, X9
    MOVZ X9, #5
    MOV X20, X9
    MOV X0, X20
    BL L_FACT
    MOV X25, X0
    ADRP X10, L_str3@PAGE
    ADD X10, X10, L_str3@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    B L_START_Exit_1
L_START_Exit_1:
    B L_1
L_1:
    LDR X19, [X29, #32] ; Restored Reg: X19 @ FP+32
    LDR X20, [X29, #40] ; Restored Reg: X20 @ FP+40
    LDR X21, [X29, #48] ; Restored Reg: X21 @ FP+48
    LDR X25, [X29, #56] ; Restored Reg: X25 @ FP+56
    LDR X26, [X29, #64] ; Restored Reg: X26 @ FP+64
    LDR X27, [X29, #72] ; Restored Reg: X27 @ FP+72
    LDR X28, [X29, #80] ; Restored Reg: X28 @ FP+80
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x5
    ; (upper half)
    .long 0x20
    .long 0x52
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x1
    ; (upper half)
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x6
    ; (upper half)
    .long 0x42
    .long 0x45
    .long 0x47
    .long 0x49
    .long 0x4e
    .long 0xa
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x4
    ; (upper half)
    .long 0x45
    .long 0x4e
    .long 0x44
    .long 0xa
    .long 0x0
    .long 0x0

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
L__data_segment_base:
