.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _WRITEN
.globl _SDL2_SET_DRAW_COLOR
.globl _WRITES
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _BCPL_LIST_APPEND_STRING
.globl _WRITEF
.globl _CONCAT
.globl _WRITEF2
.globl _WRITEF6
.globl _SDL2_DESTROY_RENDERER
.globl _BCPL_LIST_GET_NTH
.globl _FILE_OPEN_READ
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
.globl _SDL2_DELAY
.globl _FREEVEC
.globl _UNPACKSTRING
.globl _SDL2_PRESENT
.globl _BCPL_GET_LAST_ERROR
.globl _BCPL_LIST_APPEND_INT
.globl _LPND
.globl _MALLOC
.globl _BCPL_FREE_CELLS
.globl _JOIN
.globl _BCPL_LIST_CREATE_EMPTY
.globl _WRITEC
.globl _FILE_READ
.globl _BCPL_CONCAT_LISTS
.globl _SDL2_GET_EVENT_KEY
.globl _BCPL_LIST_APPEND_FLOAT
.globl _FPND
.globl _FEXP
.globl _WRITEF4
.globl _RUNTIME_METHOD_LOOKUP
.globl _BCPL_LIST_GET_HEAD_AS_INT
.globl _NEWLINE
.globl _BCPL_LIST_GET_HEAD_AS_FLOAT
.globl _FWRITE
.globl _SDL2_FILL_RECT
.globl _OBJECT_HEAP_FREE
.globl _RETURNNODETOFREELIST
.globl _APND
.globl _FCOS
.globl _PACKSTRING
.globl _BCPL_FREE_LIST_SAFE
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST
.globl _FILE_CLOSE
.globl _FIND
.globl _WRITEF3
.globl _BCPL_GET_ATOM_TYPE
.globl _FILE_READS
.globl _PIC_RUNTIME_HELPER
.globl _SDL2_CREATE_WINDOW
.globl _SLURP
.globl _RAND
.globl _FILE_TELL
.globl _FILE_OPEN_APPEND
.globl _SDL2_CREATE_WINDOW_EX
.globl _FSIN
.globl _BCPL_ALLOC_WORDS
.globl _BCPL_ALLOC_CHARS
.globl _WRITEF5
.globl _SDL2_GET_CURRENT_VIDEO_DRIVER
.globl _SDL2_INIT_SUBSYSTEMS
.globl _FTAN
.globl _SPIT
.globl _FRND
.globl _OBJECT_HEAP_ALLOC
.globl _SDL2_TEST_BASIC
.globl _BCPL_LIST_GET_TAIL
.globl _REVERSE
.globl _BCPL_BOUNDS_ERROR
.globl _FILE_EOF
.globl _STRCMP
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _FINISH
.globl _RDCH
.globl _SDL2_GET_EVENT_MOUSE
.globl _SPLIT
.globl _FILE_WRITES
.globl _SDL2_QUIT
.globl _SDL2_INIT
.globl _SDL2_SET_WINDOW_TITLE
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _FABS
.globl _DEEPCOPYLITERALLIST
.globl _BCPL_LIST_GET_REST
.globl _SDL2_CLEAR
.globl _SDL2_CREATE_RENDERER
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_SET_WINDOW_SIZE
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_GET_VERSION
.globl _SDL2_GET_EVENT_BUTTON
.globl _STRCOPY
.globl _FIX
.globl _WRITEF1
.globl _FILE_WRITE
.globl _COPYLIST
.globl _SDL2_DRAW_RECT
.globl _SDL2_GET_DISPLAY_MODES
.globl _SPND
.globl _SDL2_CREATE_RENDERER_EX
.globl _SDL2_DRAW_LINE
.globl _BCPL_CLEAR_ERRORS
.globl _FLOG
.globl _FILTER
.globl _FILE_OPEN_WRITE
.globl _SDL2_GET_TICKS
.globl ___SDL2_STATIC_BUILD
.globl _STRLEN
.globl _SDL2_DRAW_POINT
.globl _SDL2_POLL_EVENT
.globl _SDL2_GET_ERROR
.p2align 2
_start:
_START:
    B L_START
L___GLOBAL_INIT:
    STP X29, X30, [SP, #-32]!
    MOV X29, SP
    STR X19, [X29, #16] ; Saved Reg: X19 @ FP+16
    STR X28, [X29, #24] ; Saved Reg: X28 @ FP+24
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L___GLOBAL_INIT_Entry_0:
    MOVZ X9, #123
    STR X9, [X28, #0] ; global_int
    MOV X15, X9
    ADRP X9, L_str0@PAGE
    ADD X9, X9, L_str0@PAGEOFF
    ADD X9, X9, #8
    STR X9, [X28, #8] ; global_str
    MOV X15, X9
    ADRP X10, L_float0@PAGE
    ADD X10, X10, L_float0@PAGEOFF
    LDR D0, [X10, #0]
    STR D0, [X28, #16]
    FMOV D31, D0
    B L___GLOBAL_INIT_Exit_1
L___GLOBAL_INIT_Exit_1:
    B L_0
L_0:
    LDR X19, [X29, #16] ; Restored Reg: X19 @ FP+16
    LDR X28, [X29, #24] ; Restored Reg: X28 @ FP+24
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-64]!
    MOV X29, SP
    STR D15, [X29, #16] ; Saved Reg: D15 @ FP+16
    STR X19, [X29, #24] ; Saved Reg: X19 @ FP+24
    STR X20, [X29, #32] ; Saved Reg: X20 @ FP+32
    STR X21, [X29, #40] ; Saved Reg: X21 @ FP+40
    STR X27, [X29, #48] ; Saved Reg: X27 @ FP+48
    STR X28, [X29, #56] ; Saved Reg: X28 @ FP+56
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X10, #8, LSL #16
    ADD X19, X28, X10
L_START_Entry_0:
    BL L___GLOBAL_INIT
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    LDR X9, [X28, #0] ; global_int
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    LDR D0, [X28, #16]
    FMOV X9, D0
    MOV X1, X9
    BL _WRITEF1
    BL _NEWLINE
    ADRP X9, L_str4@PAGE
    ADD X9, X9, L_str4@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_Exit_1
L_START_Exit_1:
    B L_1
L_1:
    LDR D15, [X29, #16] ; Restored Reg: D15 @ FP+16
    LDR X19, [X29, #24] ; Restored Reg: X19 @ FP+24
    LDR X20, [X29, #32] ; Restored Reg: X20 @ FP+32
    LDR X21, [X29, #40] ; Restored Reg: X21 @ FP+40
    LDR X27, [X29, #48] ; Restored Reg: X27 @ FP+48
    LDR X28, [X29, #56] ; Restored Reg: X28 @ FP+56
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
    .long 0x68
    .long 0x65
    .long 0x6c
    .long 0x6c
    .long 0x6f
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x14
    ; (upper half)
    .long 0x53
    .long 0x69
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x47
    .long 0x4c
    .long 0x4f
    .long 0x42
    .long 0x41
    .long 0x4c
    .long 0x53
    .long 0x20
    .long 0x74
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x10
    ; (upper half)
    .long 0x67
    .long 0x6c
    .long 0x6f
    .long 0x62
    .long 0x61
    .long 0x6c
    .long 0x5f
    .long 0x69
    .long 0x6e
    .long 0x74
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0xa
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x10
    ; (upper half)
    .long 0x67
    .long 0x6c
    .long 0x6f
    .long 0x62
    .long 0x61
    .long 0x6c
    .long 0x5f
    .long 0x66
    .long 0x6c
    .long 0x74
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x66
    .long 0xa
    .long 0x0
    .long 0x0
L_str4:
    .quad 0xf
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
    .long 0xa
    .long 0x0
    .long 0x0
L_float0:
    .quad 0x40091eb851eb851f
    ; (upper half)

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
L__data_segment_base:
    .quad 0x0
    ; (upper half)
    .quad 0x0
    ; (upper half)
    .quad 0x0
    ; (upper half)
