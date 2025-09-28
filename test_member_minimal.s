.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
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
    STR X19, [X29, #96] ; Saved Reg: X19 @ FP+96
    STR X24, [X29, #104] ; Saved Reg: X24 @ FP+104
    STR X25, [X29, #112] ; Saved Reg: X25 @ FP+112
    STR X26, [X29, #120] ; Saved Reg: X26 @ FP+120
    STR X27, [X29, #128] ; Saved Reg: X27 @ FP+128
    STR X28, [X29, #136] ; Saved Reg: X28 @ FP+136
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    BLR X9
    MOVZ X0, #24
    BL _OBJECT_HEAP_ALLOC
    MOV X20, X0
    ADRP X9, L_Point_vtable@PAGE
    ADD X9, X9, L_Point_vtable@PAGEOFF
    STR X9, [X20, #0] ; store vtable ptr
    MOV X27, X20
    MOVZ X9, #10
    MOV X20, X9
    MOVZ X9, #20
    MOV X21, X9
    MOV X0, X27
    MOV X1, X20
    MOV X2, X21
    BL L_Point_init
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #24] ; Load method address
    MOV X0, X27
    BLR X10
    MOV X15, X0
    LDR X9, [X27, #0] ; Load vtable pointer
    LDR X10, [X9, #32] ; Load method address
    MOV X0, X27
    BLR X10
    MOV X26, X0
    LDR X9, [X27, #8] ; Load member x
    MOV X15, X9
    LDR X9, [X27, #16] ; Load member y
    MOV X15, X9
    MOVZ X9, #30
    STR X9, [X27, #8] ; Store to member x
    MOVZ X9, #40
    STR X9, [X27, #16] ; Store to member y
    LDR X9, [X27, #8] ; Load member x
    MOV X26, X9
    LDR X9, [X27, #16] ; Load member y
    MOV X25, X9
    LDR X9, [X27, #8] ; Load member x
    LDR X10, [X27, #16] ; Load member y
    ADD X9, X9, X10
    MOV X15, X9
    MOV X9, X26
    MUL X9, X9, X25
    MOV X24, X9
    LDR X9, [X27, #8] ; Load member x
    CMP X9, #25
    CSET X10, GT
    CMP X10, XZR
    B.EQ L_START_Else_2
    B L_START_Then_1
    BLR X9
    LDR X9, [X27, #8] ; Load member x
    SUB X9, X9, #5
    MOV X24, X9
    BLR X9
    B L_START_Join_3
L_START_Exit_4:
    B L_5
L_START_Join_3:
    MOV X0, X27
    BL L_Point_RELEASE
    BLR X9
    B L_START_Exit_4
    BLR X9
    LDR X9, [X27, #8] ; Load member x
    ADD X9, X9, #5
    MOV X24, X9
    BLR X9
    B L_START_Join_3
L_5:
    LDR X19, [X29, #96] ; Restored Reg: X19 @ FP+96
    LDR X24, [X29, #104] ; Restored Reg: X24 @ FP+104
    LDR X25, [X29, #112] ; Restored Reg: X25 @ FP+112
    LDR X26, [X29, #120] ; Restored Reg: X26 @ FP+120
    LDR X27, [X29, #128] ; Restored Reg: X27 @ FP+128
    LDR X28, [X29, #136] ; Restored Reg: X28 @ FP+136
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #2232
    movk x16, #727, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_Point_vtable:
    .quad L_Point_CREATE
    .quad L_Point_RELEASE
    .quad L_Point_init
    .quad L_Point_getX
    .quad L_Point_getY

.section __DATA,__data
.p2align 3
L__data_segment_base:
