.section __TEXT,__text,regular,pure_instructions
.globl _start
.globl _START
.globl _WRITEN
.globl _GETVEC
.globl _WRITEF
.globl _NEWLINE
.globl _SDL2_QUIT
.globl _SDL2_INIT
.globl _RDCH
.globl _BCPL_LIST_GET_NTH
.globl _BCPL_ALLOC_WORDS
.globl _SPLIT
.globl _BCPL_CHECK_AND_DISPLAY_ERRORS
.globl _FIND
.globl _SDL2_GET_EVENT_BUTTON
.globl _LPND
.globl _FGETVEC
.globl _SLURP
.globl _DEEPCOPYLIST
.globl _BCPL_FREE_LIST_SAFE
.globl _WRITEC
.globl _TIMER_DISPLAY
.globl _FILE_READ
.globl _UNPACKSTRING
.globl _SDL2_DELAY
.globl _FREEVEC
.globl _BCPL_CONCAT_LISTS
.globl _SDL2_GET_EVENT_KEY
.globl _LIST_CREATE
.globl _LIST_APPEND_INT
.globl _FILTER
.globl _BCPL_CLEAR_ERRORS
.globl _FLOG
.globl _BCPL_BOUNDS_ERROR
.globl _REVERSE
.globl _GET_FREE_LIST_HEAD_ADDR
.globl _SPIT
.globl _FRND
.globl _FTAN
.globl _DEEPCOPYLITERALLIST
.globl _FCOS
.globl _APND
.globl _LIST_HEAD_INT
.globl _STRCOPY
.globl _LIST_APPEND_STRING
.globl _FILE_OPEN_READ
.globl _LIST_APPEND_FLOAT
.globl _JOIN
.globl _BCPL_FREE_CELLS
.globl _MALLOC
.globl _RETURNNODETOFREELIST
.globl _RAND
.globl _SDL2_FILL_RECT
.globl _FWRITE
.globl _FILE_TELL
.globl _WRITEF2
.globl _STRLEN
.globl _RND
.globl _PACKSTRING
.globl _BCPL_LIST_APPEND_LIST
.globl _HEAPMANAGER_ISSAMMENABLED
.globl _FILE_OPEN_APPEND
.globl _SDL2_CREATE_WINDOW_EX
.globl _FSIN
.globl _WRITES
.globl _BCPL_ALLOC_CHARS
.globl _WRITEF5
.globl _FABS
.globl _SDL2_GET_CURRENT_VIDEO_DRIVER
.globl _SDL2_INIT_SUBSYSTEMS
.globl _LIST_FREE
.globl _FILE_READS
.globl _WRITEF3
.globl _BCPL_GET_ATOM_TYPE
.globl _BCPL_FREE_LIST
.globl _FILE_CLOSE
.globl _SDL2_GET_EVENT_MOUSE
.globl _STRCMP
.globl _SDL2_GET_VIDEO_DRIVERS
.globl _FINISH
.globl _LIST_HEAD_FLOAT
.globl _HEAPMANAGER_ENTER_SCOPE
.globl _SDL2_DRAW_POINT
.globl _HEAPMANAGER_EXIT_SCOPE
.globl _FILE_OPEN_WRITE
.globl _SDL2_GET_TICKS
.globl _WRITEF6
.globl _HEAPMANAGER_SETSAMMENABLED
.globl _SETTYPE
.globl _FILE_SEEK
.globl _SDL2_GET_VERSION
.globl _SDL2_TEST_BASIC
.globl _OBJECT_HEAP_ALLOC
.globl _FILE_WRITES
.globl _PIC_RUNTIME_HELPER
.globl _SDL2_DRAW_RECT
.globl _TIMER_CLEAR
.globl _SDL2_CLEAR_ERROR
.globl _SDL2_SET_WINDOW_SIZE
.globl _SDL2_CREATE_RENDERER
.globl _WRITEF4
.globl _FEXP
.globl _FPND
.globl _FIX
.globl _WRITEF1
.globl _FILE_WRITE
.globl _TIMER_GET_CALL_COUNT
.globl _COPYLIST
.globl _SDL2_GET_DISPLAY_MODES
.globl _BCPL_GET_LAST_ERROR
.globl _SDL2_PRESENT
.globl _SPND
.globl _SDL2_CREATE_RENDERER_EX
.globl _BCPL_LIST_GET_REST
.globl _SDL2_DESTROY_RENDERER
.globl _SDL2_CLEAR
.globl _WRITEF7
.globl _SDL2_DESTROY_WINDOW
.globl _SDL2_SET_WINDOW_TITLE
.globl _HEAPMANAGER_WAITFORSAMM
.globl _SDL2_POLL_EVENT
.globl _LIST_TAIL
.globl _SDL2_GET_ERROR
.globl _FILE_EOF
.globl _SDL2_CREATE_WINDOW
.globl _TIMER_GET_TOTAL_NS
.globl _RUNTIME_METHOD_LOOKUP
.globl _TIMER_START
.globl _SDL2_SET_DRAW_COLOR
.globl _CONCAT
.globl _SDL2_DRAW_LINE
.globl _OBJECT_HEAP_FREE
.globl _TIMER_END
.p2align 2
_start:
_START:
    B L_START
    
; --- Veneer Section ---
    ; --- End Veneer Section ---

L_START:
    STP X29, X30, [SP, #-128]!
    MOV X29, SP
    STP x19, x20, [x29, #40]
    STP x21, x22, [x29, #56]
    STP x23, x24, [x29, #72]
    STP x25, x26, [x29, #88]
    STP x27, x28, [x29, #104]
    ADRP X28, L__data_segment_base@PAGE
    ADD X28, X28, L__data_segment_base@PAGEOFF
L_START_Entry_0:
    ADRP X9, L_str0_plus_8@PAGE
    ADD X9, X9, L_str0_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #2
    MOV X0, X9
    BL _GETVEC
    MOV X20, X0
    MOV X25, X20
    MOVZ X9, #2
    MOV X0, X9
    BL _GETVEC
    MOV X20, X0
    MOV X24, X20
    ADRP X9, L_str1_plus_8@PAGE
    ADD X9, X9, L_str1_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #1
    ins v0.b[0], w9
    MOVZ X9, #2
    ins v0.b[1], w9
    MOVZ X9, #3
    ins v0.b[2], w9
    MOVZ X9, #4
    ins v0.b[3], w9
    MOVZ X9, #5
    ins v0.b[4], w9
    MOVZ X9, #6
    ins v0.b[5], w9
    MOVZ X9, #7
    ins v0.b[6], w9
    MOVZ X9, #8
    ins v0.b[7], w9
    fmov X20, D0
    MOVZ X9, #0
    LSL X10, X9, #3
    ADD X11, X25, X10
    STR X20, [X11, #0]
    MOVZ X9, #10
    ins v0.b[0], w9
    MOVZ X9, #11
    ins v0.b[1], w9
    MOVZ X9, #12
    ins v0.b[2], w9
    MOVZ X9, #13
    ins v0.b[3], w9
    MOVZ X9, #14
    ins v0.b[4], w9
    MOVZ X9, #15
    ins v0.b[5], w9
    MOVZ X9, #16
    ins v0.b[6], w9
    MOVZ X9, #17
    ins v0.b[7], w9
    fmov X20, D0
    MOVZ X9, #1
    LSL X10, X9, #3
    ADD X11, X25, X10
    STR X20, [X11, #0]
    MOVZ X9, #1
    ins v0.b[0], w9
    MOVZ X9, #1
    ins v0.b[1], w9
    MOVZ X9, #1
    ins v0.b[2], w9
    MOVZ X9, #1
    ins v0.b[3], w9
    MOVZ X9, #1
    ins v0.b[4], w9
    MOVZ X9, #1
    ins v0.b[5], w9
    MOVZ X9, #1
    ins v0.b[6], w9
    MOVZ X9, #1
    ins v0.b[7], w9
    fmov X20, D0
    MOVZ X9, #0
    LSL X10, X9, #3
    ADD X11, X24, X10
    STR X20, [X11, #0]
    MOVZ X9, #2
    ins v0.b[0], w9
    MOVZ X9, #2
    ins v0.b[1], w9
    MOVZ X9, #2
    ins v0.b[2], w9
    MOVZ X9, #2
    ins v0.b[3], w9
    MOVZ X9, #2
    ins v0.b[4], w9
    MOVZ X9, #2
    ins v0.b[5], w9
    MOVZ X9, #2
    ins v0.b[6], w9
    MOVZ X9, #2
    ins v0.b[7], w9
    fmov X20, D0
    MOVZ X9, #1
    LSL X10, X9, #3
    ADD X11, X24, X10
    STR X20, [X11, #0]
    ADRP X9, L_str2_plus_8@PAGE
    ADD X9, X9, L_str2_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    MOVZ X9, #2
    MOV X0, X9
    BL _GETVEC
    MOV X20, X0
    MOV X26, X20
    SUB SP, SP, #16
    STR X25, [SP, #0]
    LDR X0, [SP, #0]
    ADD SP, SP, #16
    SUB X10, X25, #8
    LDR X9, [X10, #0] ; Load vector/string length
    MOV X27, X9
    MOV X0, X27
    BL _GETVEC
    MOV X20, X0
    MOV X26, X20
    MOVZ X9, #0
    MOV X23, X9
    B L_START_ForHeader_1
L_START_Exit_5:
    B L_0
L_START_ForBody_2:
    SUB X10, X25, #8
    LDR X9, [X10, #0] ; Load vector length for bounds check
    CMP X23, X9
    B.HS L__bounds_error_handler_START
    MOV X9, X23
    LSL X9, X9, #3
    ADD X10, X25, X9
    LDR X9, [X10, #0]
    SUB X11, X24, #8
    LDR X10, [X11, #0] ; Load vector length for bounds check
    CMP X23, X10
    B.HS L__bounds_error_handler_START
    MOV X10, X23
    LSL X10, X10, #3
    ADD X11, X24, X10
    LDR X10, [X11, #0]
    ADD X9, X9, X10
    LSL X10, X23, #3
    ADD X11, X26, X10
    STR X9, [X11, #0]
    B L_START_ForIncrement_3
L_START_ForExit_4:
    ADRP X9, L_str3_plus_8@PAGE
    ADD X9, X9, L_str3_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    ADRP X9, L_str4_plus_8@PAGE
    ADD X9, X9, L_str4_plus_8@PAGEOFF
    MOV X0, X9
    SUB X10, X26, #8
    LDR X9, [X10, #0] ; Load vector/table/string length
    MOV X1, X9
    BL _WRITEF1
    ADRP X9, L_str5_plus_8@PAGE
    ADD X9, X9, L_str5_plus_8@PAGEOFF
    MOV X0, X9
    BL _WRITEF
    B L_START_Exit_5
L_START_ForHeader_1:
    MOV X9, X23
    MOV X10, X27
    SUB X10, X10, #1
    CMP X9, X10
    B.GT L_START_ForExit_4
    B L_START_ForBody_2
L_START_ForIncrement_3:
    ADD X23, X23, #1
    B L_START_ForHeader_1
L__bounds_error_handler_START:
    MOVZ X0, #0
    MOVZ X1, #65535
    MOVZ X2, #0
    BL _BCPL_BOUNDS_ERROR
    BRK #0
L_0:
    LDP x19, x20, [x29, #40]
    LDP x21, x22, [x29, #56]
    LDP x23, x24, [x29, #72]
    LDP x25, x26, [x29, #88]
    LDP x27, x28, [x29, #104]
    MOV SP, X29 ; Deallocate frame by moving FP to SP
    LDP x29, x30, [SP, #0]
    ADD SP, SP, #16 ; Deallocate space for saved FP/LR
    RET
L___veneer_:
    movz x16, #34392
    movk x16, #676, lsl #16
    movk x16, #1, lsl #32
    movk x16, #0, lsl #48
    blr x16

.section __DATA,__const
.p2align 3
L_str0:
    .quad 0x25
    ; (upper half)
.p2align 2
L_str0_plus_8:
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
    .long 0x4f
    .long 0x43
    .long 0x54
    .long 0x53
    .long 0x20
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x61
    .long 0x64
    .long 0x64
    .long 0x69
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_str1:
    .quad 0x16
    ; (upper half)
.p2align 2
L_str1_plus_8:
    .long 0x43
    .long 0x72
    .long 0x65
    .long 0x61
    .long 0x74
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x4f
    .long 0x43
    .long 0x54
    .long 0x53
    .long 0x20
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x73
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_str2:
    .quad 0x15
    ; (upper half)
.p2align 2
L_str2_plus_8:
    .long 0x49
    .long 0x6e
    .long 0x69
    .long 0x74
    .long 0x69
    .long 0x61
    .long 0x6c
    .long 0x69
    .long 0x7a
    .long 0x65
    .long 0x64
    .long 0x20
    .long 0x76
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x73
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_str3:
    .quad 0x1b
    ; (upper half)
.p2align 2
L_str3_plus_8:
    .long 0x56
    .long 0x65
    .long 0x63
    .long 0x74
    .long 0x6f
    .long 0x72
    .long 0x20
    .long 0x61
    .long 0x64
    .long 0x64
    .long 0x69
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
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
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_str4:
    .quad 0x13
    ; (upper half)
.p2align 2
L_str4_plus_8:
    .long 0x52
    .long 0x65
    .long 0x73
    .long 0x75
    .long 0x6c
    .long 0x74
    .long 0x20
    .long 0x6c
    .long 0x65
    .long 0x6e
    .long 0x67
    .long 0x74
    .long 0x68
    .long 0x3a
    .long 0x20
    .long 0x25
    .long 0x64
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0
.p2align 3
L_str5:
    .quad 0x27
    ; (upper half)
.p2align 2
L_str5_plus_8:
    .long 0x2705
    .long 0x20
    .long 0x4f
    .long 0x43
    .long 0x54
    .long 0x53
    .long 0x20
    .long 0x73
    .long 0x69
    .long 0x6d
    .long 0x70
    .long 0x6c
    .long 0x65
    .long 0x20
    .long 0x61
    .long 0x64
    .long 0x64
    .long 0x69
    .long 0x74
    .long 0x69
    .long 0x6f
    .long 0x6e
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
    .long 0x5c
    .long 0x6e
    .long 0x0
    .long 0x0

.section __DATA,__data
.p2align 3
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
    .long 0x0
.p2align 2
L__data_segment_base:
