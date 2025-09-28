.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _HeapManager_exit_scope
.globl _writef
.globl _WRITEN
.globl _WRITES
.globl _BCPL_LIST_APPEND_STRING
.globl _WRITEF
.globl _BCPL_LIST_GET_NTH
.globl _FILE_OPEN_READ
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
.globl _RDCH
.globl _BCPL_ALLOC_WORDS
.globl _FILE_READ
.globl _WRITEC
.globl _HEAPMANAGER_ISSAMMENABLED
.globl _SPLIT
.globl _BCPL_LIST_CREATE_EMPTY
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _UNPACKSTRING
.globl _SDL2_DELAY
.globl _FREEVEC
.globl _BCPL_LIST_APPEND_FLOAT
.globl _BCPL_CONCAT_LISTS
.globl _SDL2_GET_EVENT_KEY
.globl _RUNTIME_METHOD_LOOKUP
.globl _BCPL_LIST_GET_HEAD_AS_INT
.globl _NEWLINE
.globl _BCPL_LIST_GET_HEAD_AS_FLOAT
.globl _OBJECT_HEAP_FREE
.globl _FPND
.globl _FEXP
.globl _WRITEF4
.globl _HEAPMANAGER_ENTER_SCOPE
.globl _FCOS
.globl _APND
.globl _WRITEF2
.globl _BCPL_CLEAR_ERRORS
.globl _FILTER
.globl _FLOG
.globl _GETVEC
.globl _LPND
.globl _BCPL_LIST_APPEND_INT
.globl _RETURNNODETOFREELIST
.globl _MALLOC
.globl _JOIN
.globl _BCPL_FREE_CELLS
.globl _STRCOPY
.globl _PACKSTRING
.globl _BCPL_FREE_LIST_SAFE
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST
.globl _FILE_CLOSE
.globl _FIND
.globl _WRITEF3
.globl _BCPL_GET_ATOM_TYPE
.globl _FILE_READS
.globl _FTAN
.globl _FRND
.globl _SPIT
.globl _SDL2_QUIT
.globl _FILE_WRITES
.globl _PIC_RUNTIME_HELPER
.globl _SDL2_CREATE_WINDOW
.globl _SLURP
.globl _RAND
.globl _WRITEF5
.globl _BCPL_ALLOC_CHARS
.globl _OBJECT_HEAP_ALLOC
.globl _SDL2_TEST_BASIC
.globl _BCPL_LIST_GET_TAIL
.globl _REVERSE
.globl _BCPL_BOUNDS_ERROR
.globl _FILE_EOF
.globl _STRCMP
.globl _FILE_TELL
.globl _FILE_OPEN_APPEND
.globl _SDL2_CREATE_WINDOW_EX
.globl _FSIN
.globl _SDL2_GET_CURRENT_VIDEO_DRIVER
.globl _SDL2_INIT_SUBSYSTEMS
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _FINISH
.globl _SDL2_GET_EVENT_MOUSE
.globl _SDL2_SET_WINDOW_TITLE
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _HeapManager_enter_scope
.globl _FABS
.globl _DEEPCOPYLITERALLIST
.globl _FIX
.globl _WRITEF1
.globl _FILE_WRITE
.globl _COPYLIST
.globl _SDL2_DRAW_RECT
.globl _SDL2_GET_DISPLAY_MODES
.globl _BCPL_GET_LAST_ERROR
.globl _SDL2_PRESENT
.globl _SPND
.globl _SDL2_CREATE_RENDERER_EX
.globl _BCPL_LIST_GET_REST
.globl _SDL2_CLEAR
.globl _SDL2_DESTROY_RENDERER
.globl _SDL2_SET_DRAW_COLOR
.globl _SDL2_INIT
.globl _SDL2_FILL_RECT
.globl _FWRITE
.globl _CONCAT
.globl _SDL2_DRAW_LINE
.globl _SDL2_CREATE_RENDERER
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_SET_WINDOW_SIZE
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_GET_VERSION
.globl _SDL2_DRAW_POINT
.globl _HEAPMANAGER_EXIT_SCOPE
.globl _SDL2_GET_EVENT_BUTTON
.globl _WRITEF6
.globl _HEAPMANAGER_SETSAMMENABLED
.globl _FILE_OPEN_WRITE
.globl _SDL2_GET_TICKS
.globl ___SDL2_STATIC_BUILD
.globl _HEAPMANAGER_WAITFORSAMM
.globl _RND
.globl _STRLEN
.globl _SDL2_POLL_EVENT
.globl _SDL2_GET_ERROR
.p2align 2
_start:
_START:
    B L_START
    
; --- Veneer Section ---
    ; --- End Veneer Section ---

L_Point_init:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    STR X19, [X29, #40] ; Saved Reg: X19 @ FP+40
    STR X20, [X29, #48] ; Saved Reg: X20 @ FP+48
    STR X28, [X29, #56] ; Saved Reg: X28 @ FP+56
    MOV X15, X0
    MOV X14, X1
    MOV X13, X2
    MOV X15, X0    // Move parameter '_this' from X0 to X15
    MOV X14, X1    // Move parameter 'newX' from X1 to X14
    MOV X13, X2    // Move parameter 'newY' from X2 to X13
L_Point_init_Entry_0:
    STR X14, [X15, #8] ; Store to member x
    STR X13, [X15, #16] ; Store to member y
    B L_Point_init_Exit_1
L_Point_init_Exit_1:
    B L_0
L_0:
    LDR X19, [X29, #40] ; Restored Reg: X19 @ FP+40
    LDR X20, [X29, #48] ; Restored Reg: X20 @ FP+48
    LDR X28, [X29, #56] ; Restored Reg: X28 @ FP+56
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Point_getX:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STR X19, [X29, #24] ; Saved Reg: X19 @ FP+24
    STR X28, [X29, #32] ; Saved Reg: X28 @ FP+32
    MOV X15, X0
L_Point_getX_Entry_0:
    B L_Point_getX_Exit_1
L_Point_getX_Exit_1:
    B L_1
L_1:
    LDR X19, [X29, #24] ; Restored Reg: X19 @ FP+24
    LDR X28, [X29, #32] ; Restored Reg: X28 @ FP+32
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Point_getY:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STR X19, [X29, #24] ; Saved Reg: X19 @ FP+24
    STR X28, [X29, #32] ; Saved Reg: X28 @ FP+32
    MOV X15, X0
L_Point_getY_Entry_0:
    B L_Point_getY_Exit_1
L_Point_getY_Exit_1:
    B L_2
L_2:
    LDR X19, [X29, #24] ; Restored Reg: X19 @ FP+24
    LDR X28, [X29, #32] ; Restored Reg: X28 @ FP+32
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Point_CREATE:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STR X19, [X29, #24] ; Saved Reg: X19 @ FP+24
    STR X28, [X29, #32] ; Saved Reg: X28 @ FP+32
    MOV X15, X0
L_Point_CREATE_Entry_0:
    B L_Point_CREATE_Exit_1
L_Point_CREATE_Exit_1:
    B L_3
L_3:
    LDR X19, [X29, #24] ; Restored Reg: X19 @ FP+24
    LDR X28, [X29, #32] ; Restored Reg: X28 @ FP+32
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_Point_RELEASE:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STR X19, [X29, #24] ; Saved Reg: X19 @ FP+24
    STR X28, [X29, #32] ; Saved Reg: X28 @ FP+32
    MOV X15, X0
L_Point_RELEASE_Entry_0:
    B L_Point_RELEASE_Exit_1
L_Point_RELEASE_Exit_1:
    B L_4
L_4:
    LDR X19, [X29, #24] ; Restored Reg: X19 @ FP+24
    LDR X28, [X29, #32] ; Restored Reg: X28 @ FP+32
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-144]!
    MOV X29, SP
    STR X19, [X29, #56] ; Saved Reg: X19 @ FP+56
    STR X20, [X29, #64] ; Saved Reg: X20 @ FP+64
    STR X21, [X29, #72] ; Saved Reg: X21 @ FP+72
    STR X22, [X29, #80] ; Saved Reg: X22 @ FP+80
    STR X23, [X29, #88] ; Saved Reg: X23 @ FP+88
    STR X24, [X29, #96] ; Saved Reg: X24 @ FP+96
    STR X25, [X29, #104] ; Saved Reg: X25 @ FP+104
    STR X26, [X29, #112] ; Saved Reg: X26 @ FP+112
    STR X27, [X29, #120] ; Saved Reg: X27 @ FP+120
    STR X28, [X29, #128] ; Saved Reg: X28 @ FP+128
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    BL _HeapManager_enter_scope
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _writef
    MOVZ X0, #24
    BL _OBJECT_HEAP_ALLOC
    MOV X20, X0
    ADRP X9, L_Point_vtable@PAGE
    ADD X9, X9, L_Point_vtable@PAGEOFF
    STR X9, [X20, #0] ; store vtable ptr
    MOV X23, X20
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _writef
    MOVZ X9, #10
    MOV X20, X9
    MOVZ X9, #20
    MOV X21, X9
    MOV X0, X23
    MOV X1, X20
    MOV X2, X21
    BL L_Point_init
    LDR X9, [X23, #0] ; Load vtable pointer
    LDR X10, [X9, #24] ; Load method address
    MOV X0, X23
    BLR X10
    MOV X25, X0
    LDR X9, [X23, #0] ; Load vtable pointer
    LDR X10, [X9, #32] ; Load method address
    MOV X0, X23
    BLR X10
    MOV X24, X0
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X25
    MOV X2, X24
    BL _writef
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _writef
    LDR X9, [X23, #8] ; Load member x
    MOV X27, X9
    LDR X9, [X23, #16] ; Load member y
    MOV X26, X9
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X27
    MOV X2, X26
    BL _writef
    MOVZ X9, #30
    STR X9, [X23, #8] ; Store to member x
    MOVZ X9, #40
    STR X9, [X23, #16] ; Store to member y
    ADRP X9, L_str5@PAGE
    ADD X9, X9, L_str5@PAGEOFF
    ADD X9, X9, #8
    LDR X10, [X23, #8] ; Load member x
    LDR X11, [X23, #16] ; Load member y
    MOV X0, X9
    MOV X1, X10
    MOV X2, X11
    BL _writef
    ADRP X9, L_str6@PAGE
    ADD X9, X9, L_str6@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _writef
    MOV X0, X23
    BL L_Point_RELEASE
    BL _HeapManager_exit_scope
    B L_START_Exit_1
L_START_Exit_1:
    B L_5
L_5:
    LDR X19, [X29, #56] ; Restored Reg: X19 @ FP+56
    LDR X20, [X29, #64] ; Restored Reg: X20 @ FP+64
    LDR X21, [X29, #72] ; Restored Reg: X21 @ FP+72
    LDR X22, [X29, #80] ; Restored Reg: X22 @ FP+80
    LDR X23, [X29, #88] ; Restored Reg: X23 @ FP+88
    LDR X24, [X29, #96] ; Restored Reg: X24 @ FP+96
    LDR X25, [X29, #104] ; Restored Reg: X25 @ FP+104
    LDR X26, [X29, #112] ; Restored Reg: X26 @ FP+112
    LDR X27, [X29, #120] ; Restored Reg: X27 @ FP+120
    LDR X28, [X29, #128] ; Restored Reg: X28 @ FP+128
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #35000
    movk x16, #651, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x1c
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x53
    .long 0x69
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x4d
    .long 0x65
    .long 0x6d
    .long 0x62
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x41
    .long 0x63
    .long 0x63
    .long 0x65
    .long 0x73
    .long 0x73
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x17
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x6d
    .long 0x65
    .long 0x74
    .long 0x68
    .long 0x6f
    .long 0x64
    .long 0x20
    .long 0x63
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x73
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x22
    ; (upper half)
    .long 0x4d
    .long 0x65
    .long 0x74
    .long 0x68
    .long 0x6f
    .long 0x64
    .long 0x20
    .long 0x63
    .long 0x61
    .long 0x6c
    .long 0x6c
    .long 0x73
    .long 0x3a
    .long 0x20
    .long 0x67
    .long 0x65
    .long 0x74
    .long 0x58
    .long 0x28
    .long 0x29
    .long 0x3d
    .long 0x25
    .long 0x64
    .long 0x2c
    .long 0x20
    .long 0x67
    .long 0x65
    .long 0x74
    .long 0x59
    .long 0x28
    .long 0x29
    .long 0x3d
    .long 0x25
    .long 0x64
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x1f
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x64
    .long 0x69
    .long 0x72
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x20
    .long 0x6d
    .long 0x65
    .long 0x6d
    .long 0x62
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x61
    .long 0x63
    .long 0x63
    .long 0x65
    .long 0x73
    .long 0x73
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0x0
    .long 0x0
L_str4:
    .quad 0x1d
    ; (upper half)
    .long 0x44
    .long 0x69
    .long 0x72
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x20
    .long 0x61
    .long 0x63
    .long 0x63
    .long 0x65
    .long 0x73
    .long 0x73
    .long 0x3a
    .long 0x20
    .long 0x70
    .long 0x2e
    .long 0x78
    .long 0x3d
    .long 0x25
    .long 0x64
    .long 0x2c
    .long 0x20
    .long 0x70
    .long 0x2e
    .long 0x79
    .long 0x3d
    .long 0x25
    .long 0x64
    .long 0x0
    .long 0x0
L_str5:
    .quad 0x20
    ; (upper half)
    .long 0x41
    .long 0x66
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x61
    .long 0x73
    .long 0x73
    .long 0x69
    .long 0x67
    .long 0x6e
    .long 0x6d
    .long 0x65
    .long 0x6e
    .long 0x74
    .long 0x3a
    .long 0x20
    .long 0x70
    .long 0x2e
    .long 0x78
    .long 0x3d
    .long 0x25
    .long 0x64
    .long 0x2c
    .long 0x20
    .long 0x70
    .long 0x2e
    .long 0x79
    .long 0x3d
    .long 0x25
    .long 0x64
    .long 0x0
    .long 0x0
L_str6:
    .quad 0xe
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
    .long 0x0
    .long 0x0
L_Point_vtable:
    .quad L_Point_CREATE
    .quad L_Point_RELEASE
    .quad L_Point_init
    .quad L_Point_getX
    .quad L_Point_getY

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
L__data_segment_base:
