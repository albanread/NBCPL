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
L_START:
    STP X29, X30, [SP, #-112]!
    MOV X29, SP
    STR X19, [X29, #32] ; Saved Reg: X19 @ FP+32
    STR X20, [X29, #40] ; Saved Reg: X20 @ FP+40
    STR X21, [X29, #48] ; Saved Reg: X21 @ FP+48
    STR X22, [X29, #56] ; Saved Reg: X22 @ FP+56
    STR X23, [X29, #64] ; Saved Reg: X23 @ FP+64
    STR X24, [X29, #72] ; Saved Reg: X24 @ FP+72
    STR X25, [X29, #80] ; Saved Reg: X25 @ FP+80
    STR X26, [X29, #88] ; Saved Reg: X26 @ FP+88
    STR X27, [X29, #96] ; Saved Reg: X27 @ FP+96
    STR X28, [X29, #104] ; Saved Reg: X28 @ FP+104
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L_START_Entry_0:
    ADRP X0, L_list0_header@PAGE
    ADD X0, X0, L_list0_header@PAGEOFF
    BL _DEEPCOPYLITERALLIST
    MOV X25, X0
    MOV X20, X25
    ADRP X9, L_str6@PAGE
    ADD X9, X9, L_str6@PAGEOFF
    ADD X9, X9, #8
    MOV X21, X9
    MOV X0, X20
    MOV X1, X21
    LDR X10, [X19, #320]
    BLR X10
    MOV X23, X0
    MOV X0, X23
    BL _WRITES
    B L_START_BlockCleanup_1
L_START_BlockCleanup_1:
    MOV X0, X25
    BL _BCPL_FREE_LIST
    B L_START_Exit_2
L_START_Exit_2:
    B L_0
L_0:
    LDR X19, [X29, #32] ; Restored Reg: X19 @ FP+32
    LDR X20, [X29, #40] ; Restored Reg: X20 @ FP+40
    LDR X21, [X29, #48] ; Restored Reg: X21 @ FP+48
    LDR X22, [X29, #56] ; Restored Reg: X22 @ FP+56
    LDR X23, [X29, #64] ; Restored Reg: X23 @ FP+64
    LDR X24, [X29, #72] ; Restored Reg: X24 @ FP+72
    LDR X25, [X29, #80] ; Restored Reg: X25 @ FP+80
    LDR X26, [X29, #88] ; Restored Reg: X26 @ FP+88
    LDR X27, [X29, #96] ; Restored Reg: X27 @ FP+96
    LDR X28, [X29, #104] ; Restored Reg: X28 @ FP+104
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #21004
    movk x16, #1300, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x4
    ; (upper half)
    .long 0x54
    .long 0x68
    .long 0x69
    .long 0x73
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x2
    ; (upper half)
    .long 0x69
    .long 0x73
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x3
    ; (upper half)
    .long 0x74
    .long 0x68
    .long 0x65
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x3
    ; (upper half)
    .long 0x61
    .long 0x67
    .long 0x65
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x2
    ; (upper half)
    .long 0x6f
    .long 0x66
    .long 0x0
    .long 0x0
L_str5:
    .quad 0x5
    ; (upper half)
    .long 0x74
    .long 0x72
    .long 0x61
    .long 0x69
    .long 0x6e
    .long 0x0
    .long 0x0
L_str6:
    .quad 0x1
    ; (upper half)
    .long 0x5f
    .long 0x0
    .long 0x0
L_list0_header:
    .long 0x0
    .long 0x0
    .quad L_list0_node_6
    .quad L_list0_node_0
    .quad 0x7
    ; (upper half)
L_list0_node_0:
    .long 0x3
    .long 0x0
    .quad L_str0
    .quad L_list0_node_1
L_list0_node_1:
    .long 0x3
    .long 0x0
    .quad L_str1
    .quad L_list0_node_2
L_list0_node_2:
    .long 0x3
    .long 0x0
    .quad L_str2
    .quad L_list0_node_3
L_list0_node_3:
    .long 0x3
    .long 0x0
    .quad L_str3
    .quad L_list0_node_4
L_list0_node_4:
    .long 0x3
    .long 0x0
    .quad L_str4
    .quad L_list0_node_5
L_list0_node_5:
    .long 0x3
    .long 0x0
    .quad L_str2
    .quad L_list0_node_6
L_list0_node_6:
    .long 0x3
    .long 0x0
    .quad L_str5
    .quad 0x0
    ; (upper half)

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
L__data_segment_base:
