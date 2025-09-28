.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _STRCMP
.globl _RDCH
.globl _WRITEF
.globl _BCPL_LIST_GET_REST
.globl _MALLOC
.globl _BCPL_LIST_CREATE_EMPTY
.globl _WRITES
.globl _BCPL_ALLOC_WORDS
.globl _DEEPCOPYLITERALLIST
.globl _BCPL_CLEAR_ERRORS
.globl _CONCAT
.globl _FREEVEC
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
.globl _RUNTIME_METHOD_LOOKUP
.globl _BCPL_LIST_GET_HEAD_AS_INT
.globl _OBJECT_HEAP_FREE
.globl _BCPL_GET_ATOM_TYPE
.globl _LPND
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _BCPL_LIST_APPEND_STRING
.globl _OBJECT_HEAP_ALLOC
.globl _SETTYPE
.globl _DEEPCOPYLIST
.globl _SPLIT
.globl _SPIT
.globl _WRITEC
.globl _FINISH
.globl _WRITEN
.globl _BCPL_LIST_GET_TAIL
.globl _BCPL_ALLOC_CHARS
.globl _BCPL_LIST_APPEND_FLOAT
.globl _PIC_RUNTIME_HELPER
.globl _COPYLIST
.globl _BCPL_LIST_APPEND_INT
.globl _STRCOPY
.globl _RETURNNODETOFREELIST
.globl _APND
.globl _BCPL_LIST_GET_HEAD_AS_FLOAT
.globl _JOIN
.globl _BCPL_FREE_CELLS
.globl _BCPL_CONCAT_LISTS
.globl _BCPL_LIST_GET_NTH
.globl _PACKSTRING
.globl _REVERSE
.globl _NEWLINE
.globl _BCPL_FREE_LIST
.globl _SLURP
.globl _STRLEN
.globl _SPND
.globl _FIND
.globl _BCPL_FREE_LIST_SAFE
.globl _FPND
.globl _FILTER
.globl _UNPACKSTRING
.globl _BCPL_GET_LAST_ERROR
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
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    BLR X26
    MOV X23, X0
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    BLR X27
    MOV X23, X0
    MOV X0, X23
    BL _WRITEN
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    B L_START_BlockCleanup_1
L_START_BlockCleanup_1:
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

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x5
    ; (upper half)
    .long 0x49
    .long 0x6e
    .long 0x69
    .long 0x74
    .long 0xa
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x8
    ; (upper half)
    .long 0x76
    .long 0x65
    .long 0x72
    .long 0x73
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x3a
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x11
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x2e
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0

.section __DATA,__data
.p2align 3
    .long 0x0
L__data_segment_base:
