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
L_Shape_CREATE:
    STP X29, X30, [SP, #-80]!
    MOV X29, SP
    STR X19, [X29, #32] ; Saved Reg: X19 @ FP+32
    STR X20, [X29, #40] ; Saved Reg: X20 @ FP+40
    STR X25, [X29, #48] ; Saved Reg: X25 @ FP+48
    STR X26, [X29, #56] ; Saved Reg: X26 @ FP+56
    STR X27, [X29, #64] ; Saved Reg: X27 @ FP+64
    STR X28, [X29, #72] ; Saved Reg: X28 @ FP+72
    MOV X27, X0    // Move parameter '_this' from X0 to X27
    MOV X25, X1    // Move parameter 'init_color' from X1 to X25
L_Shape_CREATE_Entry_0:
    STR X25, [X27, #8] ; Store to member color
    MOV X26, X25
    B L_Shape_CREATE_Exit_1
L_Shape_CREATE_Exit_1:
    B L_0
L_0:
    LDR X19, [X29, #32] ; Restored Reg: X19 @ FP+32
    LDR X20, [X29, #40] ; Restored Reg: X20 @ FP+40
    LDR X25, [X29, #48] ; Restored Reg: X25 @ FP+48
    LDR X26, [X29, #56] ; Restored Reg: X26 @ FP+56
    LDR X27, [X29, #64] ; Restored Reg: X27 @ FP+64
    LDR X28, [X29, #72] ; Restored Reg: X28 @ FP+72
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Shape_getColor:
    LDR X0, [X0, #8] ; Optimized accessor load (int/ptr)
    RET
L_Shape_draw:
    STP X29, X30, [SP, #-96]!
    MOV X29, SP
    STR X19, [X29, #24] ; Saved Reg: X19 @ FP+24
    STR X20, [X29, #32] ; Saved Reg: X20 @ FP+32
    STR X21, [X29, #40] ; Saved Reg: X21 @ FP+40
    STR X22, [X29, #48] ; Saved Reg: X22 @ FP+48
    STR X24, [X29, #56] ; Saved Reg: X24 @ FP+56
    STR X25, [X29, #64] ; Saved Reg: X25 @ FP+64
    STR X26, [X29, #72] ; Saved Reg: X26 @ FP+72
    STR X27, [X29, #80] ; Saved Reg: X27 @ FP+80
    STR X28, [X29, #88] ; Saved Reg: X28 @ FP+88
    MOV X25, X0    // Move parameter '_this' from X0 to X25
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L_Shape_draw_Entry_0:
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X24, [X25, #8] ; Load member color
    MOV X0, X24
    BL _WRITEN
    B L_Shape_draw_Exit_1
L_Shape_draw_Exit_1:
    B L_2
L_2:
    LDR X19, [X29, #24] ; Restored Reg: X19 @ FP+24
    LDR X20, [X29, #32] ; Restored Reg: X20 @ FP+32
    LDR X21, [X29, #40] ; Restored Reg: X21 @ FP+40
    LDR X22, [X29, #48] ; Restored Reg: X22 @ FP+48
    LDR X24, [X29, #56] ; Restored Reg: X24 @ FP+56
    LDR X25, [X29, #64] ; Restored Reg: X25 @ FP+64
    LDR X26, [X29, #72] ; Restored Reg: X26 @ FP+72
    LDR X27, [X29, #80] ; Restored Reg: X27 @ FP+80
    LDR X28, [X29, #88] ; Restored Reg: X28 @ FP+88
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Shape_RELEASE:
    STP X29, X30, [SP, #-80]!
    MOV X29, SP
    STR X19, [X29, #24] ; Saved Reg: X19 @ FP+24
    STR X20, [X29, #32] ; Saved Reg: X20 @ FP+32
    STR X21, [X29, #40] ; Saved Reg: X21 @ FP+40
    STR X26, [X29, #48] ; Saved Reg: X26 @ FP+48
    STR X27, [X29, #56] ; Saved Reg: X27 @ FP+56
    STR X28, [X29, #64] ; Saved Reg: X28 @ FP+64
    MOV X26, X0    // Move parameter '_this' from X0 to X26
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L_Shape_RELEASE_Entry_0:
    MOV X0, X26
    BL _OBJECT_HEAP_FREE
    B L_Shape_RELEASE_Exit_1
L_Shape_RELEASE_Exit_1:
    B L_3
L_3:
    LDR X19, [X29, #24] ; Restored Reg: X19 @ FP+24
    LDR X20, [X29, #32] ; Restored Reg: X20 @ FP+32
    LDR X21, [X29, #40] ; Restored Reg: X21 @ FP+40
    LDR X26, [X29, #48] ; Restored Reg: X26 @ FP+48
    LDR X27, [X29, #56] ; Restored Reg: X27 @ FP+56
    LDR X28, [X29, #64] ; Restored Reg: X28 @ FP+64
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Circle_CREATE:
    STP X29, X30, [SP, #-112]!
    MOV X29, SP
    STR X19, [X29, #40] ; Saved Reg: X19 @ FP+40
    STR X20, [X29, #48] ; Saved Reg: X20 @ FP+48
    STR X21, [X29, #56] ; Saved Reg: X21 @ FP+56
    STR X24, [X29, #64] ; Saved Reg: X24 @ FP+64
    STR X25, [X29, #72] ; Saved Reg: X25 @ FP+72
    STR X26, [X29, #80] ; Saved Reg: X26 @ FP+80
    STR X27, [X29, #88] ; Saved Reg: X27 @ FP+88
    STR X28, [X29, #96] ; Saved Reg: X28 @ FP+96
    MOV X27, X0    // Move parameter '_this' from X0 to X27
    MOV X25, X1    // Move parameter 'init_color' from X1 to X25
    MOV X24, X2    // Move parameter 'init_radius' from X2 to X24
L_Circle_CREATE_Entry_0:
    STR X25, [X27, #8] ; Store to member color
    MOV X26, X25
    STR X24, [X27, #16] ; Store to member radius
    MOV X25, X24
    B L_Circle_CREATE_Exit_1
L_Circle_CREATE_Exit_1:
    B L_4
L_4:
    LDR X19, [X29, #40] ; Restored Reg: X19 @ FP+40
    LDR X20, [X29, #48] ; Restored Reg: X20 @ FP+48
    LDR X21, [X29, #56] ; Restored Reg: X21 @ FP+56
    LDR X24, [X29, #64] ; Restored Reg: X24 @ FP+64
    LDR X25, [X29, #72] ; Restored Reg: X25 @ FP+72
    LDR X26, [X29, #80] ; Restored Reg: X26 @ FP+80
    LDR X27, [X29, #88] ; Restored Reg: X27 @ FP+88
    LDR X28, [X29, #96] ; Restored Reg: X28 @ FP+96
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Circle_getRadius:
    LDR X0, [X0, #16] ; Optimized accessor load (int/ptr)
    RET
L_Circle_draw:
    STP X29, X30, [SP, #-112]!
    MOV X29, SP
    STR X19, [X29, #24] ; Saved Reg: X19 @ FP+24
    STR X20, [X29, #32] ; Saved Reg: X20 @ FP+32
    STR X21, [X29, #40] ; Saved Reg: X21 @ FP+40
    STR X22, [X29, #48] ; Saved Reg: X22 @ FP+48
    STR X23, [X29, #56] ; Saved Reg: X23 @ FP+56
    STR X24, [X29, #64] ; Saved Reg: X24 @ FP+64
    STR X25, [X29, #72] ; Saved Reg: X25 @ FP+72
    STR X26, [X29, #80] ; Saved Reg: X26 @ FP+80
    STR X27, [X29, #88] ; Saved Reg: X27 @ FP+88
    STR X28, [X29, #96] ; Saved Reg: X28 @ FP+96
    MOV X25, X0    // Move parameter '_this' from X0 to X25
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L_Circle_draw_Entry_0:
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    LDR X23, [X25, #16] ; Load member radius
    MOV X0, X23
    BL _WRITEN
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOV X0, X24
    BL _WRITEN
    B L_Circle_draw_Exit_1
L_Circle_draw_Exit_1:
    B L_6
L_6:
    LDR X19, [X29, #24] ; Restored Reg: X19 @ FP+24
    LDR X20, [X29, #32] ; Restored Reg: X20 @ FP+32
    LDR X21, [X29, #40] ; Restored Reg: X21 @ FP+40
    LDR X22, [X29, #48] ; Restored Reg: X22 @ FP+48
    LDR X23, [X29, #56] ; Restored Reg: X23 @ FP+56
    LDR X24, [X29, #64] ; Restored Reg: X24 @ FP+64
    LDR X25, [X29, #72] ; Restored Reg: X25 @ FP+72
    LDR X26, [X29, #80] ; Restored Reg: X26 @ FP+80
    LDR X27, [X29, #88] ; Restored Reg: X27 @ FP+88
    LDR X28, [X29, #96] ; Restored Reg: X28 @ FP+96
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-80]!
    MOV X29, SP
    STR X19, [X29, #32] ; Saved Reg: X19 @ FP+32
    STR X20, [X29, #40] ; Saved Reg: X20 @ FP+40
    STR X25, [X29, #48] ; Saved Reg: X25 @ FP+48
    STR X26, [X29, #56] ; Saved Reg: X26 @ FP+56
    STR X27, [X29, #64] ; Saved Reg: X27 @ FP+64
    STR X28, [X29, #72] ; Saved Reg: X28 @ FP+72
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L_START_Entry_0:
    MOVZ X0, #16
    BL _OBJECT_HEAP_ALLOC
    MOV X20, X0
    ADRP X9, L_Shape_vtable@PAGE
    ADD X9, X9, L_Shape_vtable@PAGEOFF
    STR X9, [X20, #0] ; store vtable ptr
    MOV X26, X20
    MOVZ X0, #24
    BL _OBJECT_HEAP_ALLOC
    MOV X20, X0
    ADRP X9, L_Circle_vtable@PAGE
    ADD X9, X9, L_Circle_vtable@PAGEOFF
    STR X9, [X20, #0] ; store vtable ptr
    MOV X25, X20
    MOVZ X9, #1
    MOV X10, X9
    MOV X0, X26
    MOV X1, X10
    BL L_Shape_CREATE
    MOVZ X9, #2
    MOV X10, X9
    MOVZ X9, #5
    MOV X11, X9
    MOV X0, X25
    MOV X1, X10
    MOV X2, X11
    BL L_Circle_CREATE
    MOV X0, X26
    BL L_Shape_draw
    MOV X0, X25
    BL L_Circle_draw
    LDR X9, [X25, #0] ; Load vtable pointer
    LDR X10, [X9, #32] ; Load method address
    MOV X0, X25
    BLR X10
    BL _WRITEN
    LDR X9, [X25, #0] ; Load vtable pointer
    LDR X10, [X9, #16] ; Load method address
    MOV X0, X25
    BLR X10
    BL _WRITEN
    MOV X0, X25
    BL L_Shape_RELEASE
    MOV X0, X26
    BL L_Shape_RELEASE
    B L_START_Exit_1
L_START_Exit_1:
    B L_7
L_7:
    LDR X19, [X29, #32] ; Restored Reg: X19 @ FP+32
    LDR X20, [X29, #40] ; Restored Reg: X20 @ FP+40
    LDR X25, [X29, #48] ; Restored Reg: X25 @ FP+48
    LDR X26, [X29, #56] ; Restored Reg: X26 @ FP+56
    LDR X27, [X29, #64] ; Restored Reg: X27 @ FP+64
    LDR X28, [X29, #72] ; Restored Reg: X28 @ FP+72
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #21496
    movk x16, #1231, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x24
    ; (upper half)
    .long 0x44
    .long 0x72
    .long 0x61
    .long 0x77
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x61
    .long 0x20
    .long 0x67
    .long 0x65
    .long 0x6e
    .long 0x65
    .long 0x72
    .long 0x69
    .long 0x63
    .long 0x20
    .long 0x73
    .long 0x68
    .long 0x61
    .long 0x70
    .long 0x65
    .long 0x20
    .long 0x77
    .long 0x69
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x6c
    .long 0x6f
    .long 0x72
    .long 0x3a
    .long 0x20
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x1e
    ; (upper half)
    .long 0x44
    .long 0x72
    .long 0x61
    .long 0x77
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x61
    .long 0x20
    .long 0x63
    .long 0x69
    .long 0x72
    .long 0x63
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x77
    .long 0x69
    .long 0x74
    .long 0x68
    .long 0x20
    .long 0x72
    .long 0x61
    .long 0x64
    .long 0x69
    .long 0x75
    .long 0x73
    .long 0x3a
    .long 0x20
    .long 0x0
    .long 0x0
L_str2:
    .quad 0xd
    ; (upper half)
    .long 0x20
    .long 0x20
    .long 0x61
    .long 0x6e
    .long 0x64
    .long 0x20
    .long 0x63
    .long 0x6f
    .long 0x6c
    .long 0x6f
    .long 0x72
    .long 0x3a
    .long 0x20
    .long 0x0
    .long 0x0
L_Circle_vtable:
    .quad L_Circle_CREATE
    .quad L_Shape_RELEASE
    .quad L_Shape_getColor
    .quad L_Circle_draw
    .quad L_Circle_getRadius
L_Shape_vtable:
    .quad L_Shape_CREATE
    .quad L_Shape_RELEASE
    .quad L_Shape_getColor
    .quad L_Shape_draw

.section __DATA,__data
.p2align 3
    .long 0x0
L__data_segment_base:
