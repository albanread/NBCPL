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
.globl _BCPL_LIST_CREATE
.globl _BCPL_LIST_APPEND_FLOAT
.globl _FREEVEC
.globl _LPND
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _SPND
.globl _STRCMP
.globl _WRITES
.globl _BCPL_LIST_CREATE_EMPTY
.globl _WRITEF
.globl _FPND
.globl _BCPL_FREE_CELLS
.globl _BCPL_LIST_APPEND_STRING
.globl _UNPACKSTRING
.globl _BCPL_LIST_APPEND_INT
.globl _MALLOC
.globl _WRITEN
.globl _SPIT
.globl _FINISH
.globl _APND
.globl _PACKSTRING
.globl _WRITEC
.globl _BCPL_FREE_LIST
.globl _SLURP
.p2align 2
_start:
_START:
    B L_START
L_START:
    STP X29, X30, [SP, #-112]!
    MOV X29, SP
    STR X19, [X29, #48] ; Saved Reg: X19 @ FP+48
    STR X20, [X29, #56] ; Saved Reg: X20 @ FP+56
    STR X21, [X29, #64] ; Saved Reg: X21 @ FP+64
    STR X22, [X29, #72] ; Saved Reg: X22 @ FP+72
    STR X23, [X29, #80] ; Saved Reg: X23 @ FP+80
    STR X24, [X29, #88] ; Saved Reg: X24 @ FP+88
    STR X28, [X29, #96] ; Saved Reg: X28 @ FP+96
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X10, #8, LSL #16
    ADD X19, X28, X10
L_START_Entry_0:
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    LDR X20, [X28, #8] ; H
    LDR X21, [X28] ; G
    MUL X21, X21, X20
    MOV X0, X21
    BL _WRITEN
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    MOVZ X10, #10
    MOV X0, X10
    BL _WRITEC
    MOVZ X10, #256
    STR X10, [X28] ; G
    MOVZ X10, #2
    STR X10, [X28, #8] ; H
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    MOVZ X10, #512
    MOV X0, X10
    BL _WRITEN
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    ADRP X10, L_str2@PAGE
    ADD X10, X10, L_str2@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    MOVZ X10, #10
    MOV X0, X10
    ADRP X1, L_str3@PAGE
    ADD X1, X1, L_str3@PAGEOFF
    ADRP X2, L_str4@PAGE
    ADD X2, X2, L_str4@PAGEOFF
    BL _BCPL_ALLOC_WORDS
    STR X0, [X29, #40] ; V
    MOVZ X10, #10
    MOV X0, X10
    BL _BCPL_ALLOC_CHARS
    STR X0, [X29, #24]
    ADRP X10, L_str5@PAGE
    ADD X10, X10, L_str5@PAGEOFF
    ADD X10, X10, #8
    STR X10, [X29, #32] ; S2
    LDR X22, [X29, #32] ; S2
    MOV X0, X22
    BL _WRITES
    MOVZ X10, #0
    LDR X23, [X29, #40] ; V
    CMP X23, X10
    CSET X23, EQ
    CMP X23, XZR
    B.EQ L_START_Join_2
    B L_START_Then_1
L_START_Exit_15:
    B L_0
L_START_ForBody_12:
    MOVZ X10, #7
    MOVZ X11, #3
    LDR X24, [X29, #16] ; I_for_var_0
    MUL X24, X24, X11
    MOVZ X11, #4
    MOVZ X12, #120
    LDR X25, [X29, #16] ; I_for_var_0
    MUL X25, X25, X12
    SDIV X25, X25, X11
    ADD X25, X25, X24
    SUB X25, X25, X10
    LDR X26, [X29, #40] ; V
    LDR X27, [X29, #16] ; I_for_var_0
    LSL X27, X27, #3
    ADD X10, X26, X27
    STR X25, [X10]
    ADRP X10, L_str6@PAGE
    ADD X10, X10, L_str6@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    LDR X20, [X29, #16] ; I_for_var_0
    MOV X0, X20
    BL _WRITEN
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    LDR X21, [X29, #40] ; V
    LDR X22, [X29, #16] ; I_for_var_0
    LSL X22, X22, #3
    ADD X10, X21, X22
    LDR X11, [X10]
    MOV X0, X11
    BL _WRITEN
    B L_START_ForIncrement_13
L_START_ForBody_4:
    MOVZ X10, #5
    LDR X23, [X29, #16] ; I_for_var_0
    MUL X23, X23, X10
    LDR X24, [X29, #40] ; V
    LDR X25, [X29, #16] ; I_for_var_0
    LSL X25, X25, #3
    ADD X10, X24, X25
    STR X23, [X10]
    B L_START_ForIncrement_5
L_START_ForBody_8:
    ADRP X10, L_str6@PAGE
    ADD X10, X10, L_str6@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    LDR X26, [X29, #16] ; I_for_var_0
    MOV X0, X26
    BL _WRITEN
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    LDR X27, [X29, #40] ; V
    LDR X20, [X29, #16] ; I_for_var_0
    LSL X20, X20, #3
    ADD X10, X27, X20
    LDR X11, [X10]
    MOV X0, X11
    BL _WRITEN
    B L_START_ForIncrement_9
L_START_ForExit_10:
    MOVZ X10, #0
    STR X10, [X29, #16] ; I_for_var_0
    B L_START_ForHeader_11
L_START_ForExit_14:
    ADRP X10, L_str7@PAGE
    ADD X10, X10, L_str7@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    B L_START_Exit_15
L_START_ForExit_6:
    ADRP X10, L_str6@PAGE
    ADD X10, X10, L_str6@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    LDR X21, [X29, #16] ; I_for_var_0
    MOV X0, X21
    BL _WRITEN
    ADRP X10, L_str1@PAGE
    ADD X10, X10, L_str1@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    MOVZ X10, #0
    STR X10, [X29, #16] ; I_for_var_0
    B L_START_ForHeader_7
L_START_ForHeader_11:
    LDR X10, [X29, #16] ; I_for_var_0
    MOVZ X11, #1
    LDR X22, [X29, #40] ; V
    SUB X12, X22, #8
    LDR X13, [X12]
    SUB X13, X13, X11
    CMP X10, X13
    B.GT L_START_ForExit_14
    B L_START_ForBody_12
L_START_ForHeader_3:
    LDR X10, [X29, #16] ; I_for_var_0
    MOVZ X11, #1
    LDR X23, [X29, #40] ; V
    SUB X12, X23, #8
    LDR X13, [X12]
    SUB X13, X13, X11
    CMP X10, X13
    B.GT L_START_ForExit_6
    B L_START_ForBody_4
L_START_ForHeader_7:
    LDR X10, [X29, #16] ; I_for_var_0
    MOVZ X11, #1
    LDR X24, [X29, #40] ; V
    SUB X12, X24, #8
    LDR X13, [X12]
    SUB X13, X13, X11
    CMP X10, X13
    B.GT L_START_ForExit_10
    B L_START_ForBody_8
L_START_ForIncrement_13:
    MOVZ X10, #1
    LDR X25, [X29, #16] ; I_for_var_0
    ADD X25, X25, X10
    STR X25, [X29, #16] ; I_for_var_0
    B L_START_ForHeader_11
L_START_ForIncrement_5:
    MOVZ X10, #1
    LDR X26, [X29, #16] ; I_for_var_0
    ADD X26, X26, X10
    STR X26, [X29, #16] ; I_for_var_0
    B L_START_ForHeader_3
L_START_ForIncrement_9:
    MOVZ X10, #1
    LDR X27, [X29, #16] ; I_for_var_0
    ADD X27, X27, X10
    STR X27, [X29, #16] ; I_for_var_0
    B L_START_ForHeader_7
L_START_Join_2:
    MOVZ X10, #0
    STR X10, [X29, #16] ; I_for_var_0
    B L_START_ForHeader_3
L_START_Then_1:
    ADRP X10, L_str8@PAGE
    ADD X10, X10, L_str8@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITES
    MOVZ X10, #0
    MOV X0, X10
    MOVZ X10, #1
    MOVK X10, #512, LSL #16
    MOV X16, X10
    SVC #128
L_0:
    LDR X19, [X29, #48] ; Restored Reg: X19 @ FP+48
    LDR X20, [X29, #56] ; Restored Reg: X20 @ FP+56
    LDR X21, [X29, #64] ; Restored Reg: X21 @ FP+64
    LDR X22, [X29, #72] ; Restored Reg: X22 @ FP+72
    LDR X23, [X29, #80] ; Restored Reg: X23 @ FP+80
    LDR X24, [X29, #88] ; Restored Reg: X24 @ FP+88
    LDR X28, [X29, #96] ; Restored Reg: X28 @ FP+96
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __TEXT,__const
.p2align 3
L_str0:
    
    .quad 0x4
    ; (upper half)
    .long 0x47
    .long 0x2a
    .long 0x48
    .long 0x3d
    .long 0x0
    .long 0x0
L_str1:
    
    .quad 0x1
    ; (upper half)
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    
    .quad 0x5
    ; (upper half)
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0xa
    .long 0x0
    .long 0x0
L_str3:
    
    .quad 0x5
    ; (upper half)
    .long 0x53
    .long 0x54
    .long 0x41
    .long 0x52
    .long 0x54
    .long 0x0
    .long 0x0
L_str4:
    
    .quad 0x0
    ; (upper half)
    .long 0x0
    .long 0x0
L_str5:
    
    .quad 0x7
    ; (upper half)
    .long 0x48
    .long 0x65
    .long 0x6c
    .long 0x6c
    .long 0x6f
    .long 0x21
    .long 0xa
    .long 0x0
    .long 0x0
L_str6:
    
    .quad 0x2
    ; (upper half)
    .long 0x49
    .long 0x3d
    .long 0x0
    .long 0x0
L_str7:
    
    .quad 0x4
    ; (upper half)
    .long 0x45
    .long 0x4e
    .long 0x44
    .long 0xa
    .long 0x0
    .long 0x0
L_str8:
    
    .quad 0x5
    ; (upper half)
    .long 0x46
    .long 0x41
    .long 0x49
    .long 0x4c
    .long 0xa
    .long 0x0
    .long 0x0

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
L__data_segment_base:
    .quad 0x100
    ; (upper half)
    .quad 0x2
    ; (upper half)
