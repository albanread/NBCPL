.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _WRITEN
.globl _SDL2_SET_DRAW_COLOR
.globl _WRITES
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _BCPL_LIST_APPEND_STRING
.globl _WRITEF
.globl _BCPL_LIST_GET_TAIL
.globl _CONCAT
.globl _WRITEF2
.globl _WRITEF6
.globl _SDL2_DESTROY_RENDERER
.globl _BCPL_LIST_GET_NTH
.globl _FILE_OPEN_READ
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST_SAFE
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
.globl _FEXP
.globl _FPND
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
.globl _BCPL_FREE_LIST
.globl _FILE_CLOSE
.globl _WRITEF3
.globl _BCPL_GET_ATOM_TYPE
.globl _FILE_READS
.globl _FTAN
.globl _FRND
.globl _SPIT
.globl _SPLIT
.globl _SDL2_QUIT
.globl _FILE_WRITES
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
.globl _REVERSE
.globl _BCPL_BOUNDS_ERROR
.globl _FILE_EOF
.globl _STRCMP
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _FINISH
.globl _RDCH
.globl _SDL2_GET_EVENT_MOUSE
.globl _SDL2_INIT
.globl _SDL2_SET_WINDOW_TITLE
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _FABS
.globl _DEEPCOPYLITERALLIST
.globl _BCPL_LIST_GET_REST
.globl _SDL2_CLEAR
.globl _SDL2_GET_TICKS
.globl _FILE_OPEN_WRITE
.globl _SDL2_CREATE_RENDERER
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_SET_WINDOW_SIZE
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_TEST_BASIC
.globl _OBJECT_HEAP_ALLOC
.globl _SDL2_GET_VERSION
.globl _SDL2_GET_EVENT_BUTTON
.globl _STRCOPY
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
.globl ___SDL2_STATIC_BUILD
.globl _STRLEN
.globl _SDL2_DRAW_POINT
.globl _SDL2_POLL_EVENT
.globl _FIND
.globl _SDL2_GET_ERROR
.p2align 2
_start:
_START:
    B L_START
L_ReturnInt:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STR X19, [X29, #32] ; Saved Reg: X19 @ FP+32
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
L_ReturnInt_Entry_0:
    B L_ReturnInt_Exit_1
L_ReturnInt_Exit_1:
    B L_0
L_0:
    LDR X19, [X29, #32] ; Restored Reg: X19 @ FP+32
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_ReturnFloat:
    STP X29, X30, [SP, #-48]!
    MOV X29, SP
    STR X19, [X29, #32] ; Saved Reg: X19 @ FP+32
    STR X28, [X29, #40] ; Saved Reg: X28 @ FP+40
L_ReturnFloat_Entry_0:
    B L_ReturnFloat_Exit_1
L_ReturnFloat_Exit_1:
    B L_1
L_1:
    LDR X19, [X29, #32] ; Restored Reg: X19 @ FP+32
    LDR X28, [X29, #40] ; Restored Reg: X28 @ FP+40
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDR X29, [SP, #0] ; Restore caller's Frame Pointer
    LDR X30, [SP, #8] ; Restore Link Register
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L_START:
    STP X29, X30, [SP, #-96]!
    MOV X29, SP
    STR X19, [X29, #48] ; Saved Reg: X19 @ FP+48
    STR X25, [X29, #56] ; Saved Reg: X25 @ FP+56
    STR X26, [X29, #64] ; Saved Reg: X26 @ FP+64
    STR X27, [X29, #72] ; Saved Reg: X27 @ FP+72
    STR X28, [X29, #80] ; Saved Reg: X28 @ FP+80
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
    MOVZ X9, #8, LSL #16
    ADD X19, X28, X9
L_START_Entry_0:
    MOVZ X9, #42
    MOV X27, X9
    ADRP X10, L_float0@PAGE
    ADD X10, X10, L_float0@PAGEOFF
    LDR D0, [X10, #0]
    FCVTZS X10, D0
    MOV X26, X10
    ADRP X10, L_str0@PAGE
    ADD X10, X10, L_str0@PAGEOFF
    ADD X10, X10, #8
    MOV X0, X10
    BL _WRITEF
    MOV X20, X27
    MOV X21, X26
    MOV X0, X20
    MOV X1, X21
    BL L_ReturnInt
    MOV X25, X0
    ADRP X9, L_str1@PAGE
    ADD X9, X9, L_str1@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X25
    BL _WRITEF1
    MOV X20, X27
    MOV X21, X26
    MOV X0, X20
    MOV X1, X21
    BL L_ReturnFloat
    MOV X25, X0
    ADRP X9, L_str2@PAGE
    ADD X9, X9, L_str2@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    MOV X1, X25
    BL _WRITEF1
    ADRP X9, L_str3@PAGE
    ADD X9, X9, L_str3@PAGEOFF
    ADD X9, X9, #8
    MOV X0, X9
    BL _WRITEF
    B L_START_BlockCleanup_1
L_START_BlockCleanup_1:
    B L_START_Exit_2
L_START_Exit_2:
    B L_2
L_2:
    LDR X19, [X29, #48] ; Restored Reg: X19 @ FP+48
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
    .quad 0x23
    ; (upper half)
    .long 0x54
    .long 0x65
    .long 0x73
    .long 0x74
    .long 0x69
    .long 0x6e
    .long 0x67
    .long 0x20
    .long 0x73
    .long 0x69
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x72
    .long 0x61
    .long 0x6d
    .long 0x65
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x72
    .long 0x65
    .long 0x74
    .long 0x75
    .long 0x72
    .long 0x6e
    .long 0x2e
    .long 0x2e
    .long 0x2e
    .long 0xa
    .long 0x0
    .long 0x0
L_str1:
    .quad 0x19
    ; (upper half)
    .long 0x52
    .long 0x65
    .long 0x74
    .long 0x75
    .long 0x72
    .long 0x6e
    .long 0x49
    .long 0x6e
    .long 0x74
    .long 0x28
    .long 0x34
    .long 0x32
    .long 0x2c
    .long 0x20
    .long 0x33
    .long 0x2e
    .long 0x31
    .long 0x34
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0xa
    .long 0x0
    .long 0x0
L_str2:
    .quad 0x1b
    ; (upper half)
    .long 0x52
    .long 0x65
    .long 0x74
    .long 0x75
    .long 0x72
    .long 0x6e
    .long 0x46
    .long 0x6c
    .long 0x6f
    .long 0x61
    .long 0x74
    .long 0x28
    .long 0x34
    .long 0x32
    .long 0x2c
    .long 0x20
    .long 0x33
    .long 0x2e
    .long 0x31
    .long 0x34
    .long 0x29
    .long 0x20
    .long 0x3d
    .long 0x20
    .long 0x25
    .long 0x66
    .long 0xa
    .long 0x0
    .long 0x0
L_str3:
    .quad 0x21
    ; (upper half)
    .long 0x53
    .long 0x69
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x70
    .long 0x61
    .long 0x72
    .long 0x61
    .long 0x6d
    .long 0x65
    .long 0x74
    .long 0x65
    .long 0x72
    .long 0x20
    .long 0x74
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
    .long 0xa
    .long 0x0
    .long 0x0
L_float0:
    .quad 0x40091eb851eb851f
    ; (upper half)

.section __DATA,__data
.p2align 3
L__data_segment_base:
