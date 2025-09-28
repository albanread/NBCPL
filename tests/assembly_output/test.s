.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _FWRITE
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
.globl _FPND
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _WRITEF
.globl _DEEPCOPYLIST
.globl _WRITEF2
.globl _REVERSE
.globl _BCPL_GET_ATOM_TYPE
.globl _WRITEF3
.globl _BCPL_LIST_GET_NTH
.globl _BCPL_FREE_LIST_SAFE
.globl _SDL2_GET_EVENT_BUTTON
.globl _SDL2_SET_DRAW_COLOR
.globl _UNPACKSTRING
.globl _BCPL_GET_LAST_ERROR
.globl _LPND
.globl _JOIN
.globl _WRITES
.globl _WRITEF5
.globl _BCPL_ALLOC_WORDS
.globl _MALLOC
.globl _BCPL_LIST_CREATE_EMPTY
.globl _SDL2_INIT_SUBSYSTEMS
.globl _FREEVEC
.globl _RDCH
.globl _SDL2_GET_EVENT_MOUSE
.globl _WRITEF1
.globl _SDL2_GET_EVENT_KEY
.globl _BCPL_LIST_APPEND_FLOAT
.globl _SDL2_CREATE_WINDOW
.globl _SDL2_DELAY
.globl _RUNTIME_METHOD_LOOKUP
.globl _BCPL_LIST_GET_HEAD_AS_INT
.globl _SDL2_FILL_RECT
.globl _OBJECT_HEAP_FREE
.globl _SPND
.globl _SDL2_POLL_EVENT
.globl _STRLEN
.globl _SDL2_DRAW_POINT
.globl _SDL2_DESTROY_RENDERER
.globl _SDL2_CLEAR
.globl _BCPL_LIST_GET_REST
.globl _PIC_RUNTIME_HELPER
.globl _BCPL_LIST_APPEND_STRING
.globl _SDL2_GET_VERSION
.globl _SETTYPE
.globl _OBJECT_HEAP_ALLOC
.globl _SDL2_GET_DISPLAY_MODES
.globl _COPYLIST
.globl _SDL2_DRAW_RECT
.globl ___SDL2_STATIC_BUILD
.globl _RETURNNODETOFREELIST
.globl _BCPL_LIST_GET_TAIL
.globl _SDL2_TEST_BASIC
.globl _BCPL_ALLOC_CHARS
.globl _BCPL_CLEAR_ERRORS
.globl _DEEPCOPYLITERALLIST
.globl _FIND
.globl _STRCMP
.globl _SDL2_PRESENT
.globl _SDL2_CREATE_RENDERER_EX
.globl _PACKSTRING
.globl _BCPL_LIST_APPEND_INT
.globl _SPIT
.globl _SPLIT
.globl _SDL2_QUIT
.globl _NEWLINE
.globl _BCPL_FREE_LIST
.globl _SLURP
.globl _BCPL_LIST_GET_HEAD_AS_FLOAT
.globl _BCPL_FREE_CELLS
.globl _BCPL_CONCAT_LISTS
.globl _SDL2_SET_WINDOW_TITLE
.globl _SDL2_DESTROY_WINDOW
.globl _SDL2_INIT
.globl _APND
.globl _SDL2_GET_CURRENT_VIDEO_DRIVER
.globl _SDL2_CREATE_RENDERER
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_SET_WINDOW_SIZE
.globl _WRITEF7
.globl _FILTER
.globl _SDL2_GET_TICKS
.globl _STRCOPY
.globl _SDL2_CREATE_WINDOW_EX
.globl _CONCAT
.globl _SDL2_DRAW_LINE
.globl _SDL2_GET_ERROR
.globl _WRITEN
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _WRITEC
.globl _FINISH
.globl _WRITEF6
.globl _WRITEF4
.p2align 2
_start:
_START:
    B L_START
L_START:
    STP X29, X30, [SP, #-80]!
    MOV X29, SP
    STR X19, [X29, #24] ; Saved Reg: X19 @ FP+24
    STR X20, [X29, #32] ; Saved Reg: X20 @ FP+32
    STR X21, [X29, #40] ; Saved Reg: X21 @ FP+40
    STR X25, [X29, #48] ; Saved Reg: X25 @ FP+48
    STR X26, [X29, #56] ; Saved Reg: X26 @ FP+56
    STR X27, [X29, #64] ; Saved Reg: X27 @ FP+64
    STR X28, [X29, #72] ; Saved Reg: X28 @ FP+72
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L_START_Entry_0:
    MOVZ X9, #100
    MOV X25, X9
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITES
    MOVZ X9, #77
    MOV X0, X9
    BLR X27
    B L_START_BlockCleanup_1
L_START_BlockCleanup_1:
    B L_START_Exit_2
L_START_Exit_2:
    B L_0
L_0:
    LDR X19, [X29, #24] ; Restored Reg: X19 @ FP+24
    LDR X20, [X29, #32] ; Restored Reg: X20 @ FP+32
    LDR X21, [X29, #40] ; Restored Reg: X21 @ FP+40
    LDR X25, [X29, #48] ; Restored Reg: X25 @ FP+48
    LDR X26, [X29, #56] ; Restored Reg: X26 @ FP+56
    LDR X27, [X29, #64] ; Restored Reg: X27 @ FP+64
    LDR X28, [X29, #72] ; Restored Reg: X28 @ FP+72
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x8
    ; (upper half)
    .long 0x58
    .long 0x20
    .long 0x69
    .long 0x73
    .long 0x20
    .long 0x31
    .long 0x30
    .long 0x30
    .long 0x0
    .long 0x0

.section __DATA,__data
.p2align 3
L__data_segment_base:
